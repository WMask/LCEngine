/**
* UtilsDX10.cpp
* 26.08.2023
* (c) Denis Romakhov
*/

#include "pch.h"
#include "RenderSystem/RenderSystemDX10/UtilsDX10.h"
#include "World/Sprites.h"
#include "Core/LCUtils.h"

#include <set>


LcTextureLoaderDX10::~LcTextureLoaderDX10()
{
    ClearCache(nullptr);
}

bool LcTextureLoaderDX10::LoadTexture(const char* texPath, ID3D10Device1* device, ID3D10Texture2D** texture, ID3D10ShaderResourceView1** view, LcSize* outTexSize)
{
    if (!device) return false;
    if (!texture && !view) return false;

    // get from cache
    auto entry = texturesCache.find(texPath);
    if (entry != texturesCache.end())
    {
        if (outTexSize) *outTexSize = entry->second.texSize;
        if (texture) *texture = entry->second.texture.Get();
        if (view) *view = entry->second.view.Get();
        return true;
    }

    // read file
    auto texData = ReadBinaryFile(texPath);
    if (texData.empty()) return false;

    // create factory
    HRESULT result = CoCreateInstance(
        CLSID_WICImagingFactory2, nullptr,
        CLSCTX_INPROC_SERVER, __uuidof(IWICImagingFactory2),
        (LPVOID*)&factory
    );

    // create decoder
    ComPtr<IWICStream> stream;
    result = factory->CreateStream(stream.GetAddressOf());
    if (FAILED(result)) return false;

    result = stream->InitializeFromMemory(const_cast<uint8_t*>(&texData[0]), static_cast<DWORD>(texData.size()));
    if (FAILED(result)) return false;

    ComPtr<IWICBitmapDecoder> decoder;
    result = factory->CreateDecoderFromStream(stream.Get(), nullptr, WICDecodeMetadataCacheOnDemand, decoder.GetAddressOf());
    if (FAILED(result)) return false;

    ComPtr<IWICBitmapFrameDecode> frame;
    result = decoder->GetFrame(0, frame.GetAddressOf());
    if (FAILED(result)) return false;

    UINT width, height;
    result = frame->GetSize(&width, &height);
    if (FAILED(result)) return false;

    // check pixel format
    WICPixelFormatGUID pixelFormat;
    result = frame->GetPixelFormat(&pixelFormat);
    if (FAILED(result)) return false;

    DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM;
    UINT bpp = 32, support = 0;
    result = device->CheckFormatSupport(format, &support);
    if (FAILED(result)) return false;

    // allocate memory
    LcBytes texPixels;
    texPixels.resize(width * height * (bpp / 8));
    BYTE* texPixelsPtr = &texPixels[0];
    UINT rowPitch = width * (bpp / 8);

    if (pixelFormat == GUID_WICPixelFormat32bppRGBA)
    {
        // copy pixel data
        result = frame->CopyPixels(nullptr, static_cast<UINT>(rowPitch), static_cast<UINT>(texPixels.size()), texPixelsPtr);
        if (FAILED(result)) return false;
    }
    else
    {
        // convert pixel data
        ComPtr<IWICFormatConverter> converter;
        result = factory->CreateFormatConverter(converter.GetAddressOf());
        if (FAILED(result)) return false;

        BOOL canConvert = FALSE;
        result = converter->CanConvert(pixelFormat, GUID_WICPixelFormat32bppRGBA, &canConvert);
        if (FAILED(result) || !canConvert) return false;

        ComPtr<IWICBitmapScaler> scaler;
        result = factory->CreateBitmapScaler(scaler.GetAddressOf());
        if (FAILED(result)) return false;

        result = scaler->Initialize(frame.Get(), width, height, WICBitmapInterpolationModeFant);
        if (FAILED(result)) return false;

        result = converter->Initialize(scaler.Get(), GUID_WICPixelFormat32bppRGBA, WICBitmapDitherTypeErrorDiffusion, nullptr, 0, WICBitmapPaletteTypeMedianCut);
        if (FAILED(result)) return false;

        result = converter->CopyPixels(nullptr, static_cast<UINT>(rowPitch), static_cast<UINT>(texPixels.size()), texPixelsPtr);
        if (FAILED(result)) return false;
    }

    factory.Reset();

    // create texture
    D3D10_TEXTURE2D_DESC desc{};
    desc.Width = width;
    desc.Height = height;
    desc.Format = format;
    desc.MipLevels = 1;
    desc.ArraySize = 1;
    desc.SampleDesc.Count = 1;
    desc.BindFlags = D3D10_BIND_SHADER_RESOURCE;

    D3D10_SUBRESOURCE_DATA initData = { texPixelsPtr, static_cast<UINT>(rowPitch), static_cast<UINT>(texPixels.size()) };

    result = device->CreateTexture2D(&desc, &initData, texture);
    if (SUCCEEDED(result) && texture)
    {
        LcTextureDataDX10 newTexData;
        newTexData.texture = *texture;
        newTexData.texSize = LcSize(width, height);

        if (outTexSize) *outTexSize = newTexData.texSize;

        if (view)
        {
            D3D10_SHADER_RESOURCE_VIEW_DESC1 SRVDesc{};
            SRVDesc.Format = desc.Format;
            SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
            SRVDesc.Texture2D.MipLevels = 1u;

            result = device->CreateShaderResourceView1(*texture, &SRVDesc, view);
            if (FAILED(result)) return false;
            newTexData.view = *view;
        }

        texturesCache.emplace(std::make_pair(std::string(texPath), newTexData));
        return true;
    }

    return false;
}

void LcTextureLoaderDX10::ClearCache(IWorld* world)
{
    if (world)
    {
        std::set<std::string> aliveTexList;
        auto& allSprites = world->GetSprites();
        for (auto sprite : allSprites)
        {
            auto texComp = sprite->GetComponent(EVCType::Texture);
            if (auto tex = (LcSpriteTextureComponent*)texComp.get())
            {
                aliveTexList.insert(tex->texture);
            }
        }

        std::set<std::string> eraseTexList;
        for (auto tex : texturesCache)
        {
            if (aliveTexList.find(tex.first) == aliveTexList.end())
            {
                eraseTexList.insert(tex.first);
            }
        }

        for (auto entry : eraseTexList)
        {
            texturesCache.erase(entry);
        }
    }
    else
    {
        texturesCache.clear();
    }
}


LcTextRendererDX10::LcTextRendererDX10(IDXGISwapChain* swapChainPtr, HWND hWnd) : swapChain(swapChainPtr)
{
    if (!swapChain || !hWnd)
    {
        throw std::exception("LcTextRendererDX10(): Invalid arguments");
    }

    if (FAILED(D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, d2dFactory.GetAddressOf())))
    {
        throw std::exception("LcTextRendererDX10(): Cannot create Direct2D factory");
    }

    ComPtr<IDXGISurface1> backBuffer;
    if (FAILED(swapChain->GetBuffer(0, IID_PPV_ARGS(&backBuffer))))
    {
        throw std::exception("LcTextRendererDX10(): Cannot get back buffer");
    }

    float dpi = (float)GetDpiForWindow(hWnd);

    D2D1_RENDER_TARGET_PROPERTIES props = D2D1::RenderTargetProperties(D2D1_RENDER_TARGET_TYPE_DEFAULT,
        D2D1::PixelFormat(DXGI_FORMAT_UNKNOWN, D2D1_ALPHA_MODE_PREMULTIPLIED), dpi, dpi);

    if (FAILED(d2dFactory->CreateDxgiSurfaceRenderTarget(backBuffer.Get(), &props, renderTarget.GetAddressOf())))
    {
        throw std::exception("LcTextRendererDX10(): Cannot create render target");
    }

    if (FAILED(DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(dwriteFactory.Get()),
        reinterpret_cast<IUnknown**>(dwriteFactory.GetAddressOf()))))
    {
        throw std::exception("LcTextRendererDX10(): Cannot create DirectWrite factory");
    }
}

LcTextRendererDX10::~LcTextRendererDX10()
{
    textFormat.Reset();
    dwriteFactory.Reset();
    renderTarget.Reset();
    d2dFactory.Reset();
}

void LcTextRendererDX10::AddFont(const std::wstring& fontName)
{
    if (FAILED(dwriteFactory->CreateTextFormat(
        fontName.c_str(), NULL,
        DWRITE_FONT_WEIGHT_NORMAL,
        DWRITE_FONT_STYLE_NORMAL,
        DWRITE_FONT_STRETCH_NORMAL,
        20, L"", textFormat.GetAddressOf())))
    {
        throw std::exception("LcTextRendererDX10::AddFont(): Cannot create font");
    }
}
