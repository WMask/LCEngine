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


std::vector<ComPtr<IDXGIAdapter>> LcEnumerateAdapters()
{
    std::vector<ComPtr<IDXGIAdapter>> adapters;
    ComPtr<IDXGIFactory> factory;

    if (FAILED(CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)factory.GetAddressOf())))
    {
        return adapters;
    }

    IDXGIAdapter* adapter;
    for (UINT i = 0; factory->EnumAdapters(i, &adapter) != DXGI_ERROR_NOT_FOUND; i++)
    {
        adapters.push_back(adapter);
    }

    return adapters;
}

bool LcFindDisplayMode(int width, int height, DXGI_MODE_DESC* outMode)
{
    if (!outMode) return false;

    outMode->Format = DXGI_FORMAT_UNKNOWN;

    auto adapters = LcEnumerateAdapters();
    for (auto adapter : adapters)
    {
        ComPtr<IDXGIOutput> output;
        if (SUCCEEDED(adapter->EnumOutputs(0, output.GetAddressOf())))
        {
            UINT numModes = 0;
            std::vector<DXGI_MODE_DESC> displayModes;
            DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM;

            output->GetDisplayModeList(format, 0, &numModes, NULL);
            displayModes.resize(numModes);

            output->GetDisplayModeList(format, 0, &numModes, &displayModes[0]);
            for (auto& mode : displayModes)
            {
                if (mode.Width == width &&
                    mode.Height == height &&
                    mode.RefreshRate.Numerator >= 60 &&
                    mode.Format == DXGI_FORMAT_R8G8B8A8_UNORM)
                {
                    *outMode = mode;
                    if (mode.RefreshRate.Denominator == 1) return true;
                }
            }
        }
    }

    return (outMode->Format != DXGI_FORMAT_UNKNOWN);
}

void LcMakeWindowAssociation(HWND hWnd)
{
    ComPtr<IDXGIFactory> factory;
    if (FAILED(CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)factory.GetAddressOf())))
    {
        throw std::exception("LcMakeWindowAssociation(): Cannot create factory");
    }

    if (FAILED(factory->MakeWindowAssociation(hWnd, 0)))
    {
        throw std::exception("LcMakeWindowAssociation(): Cannot make window association");
    }
}

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
            if (auto texComp = sprite->GetTextureComponent())
            {
                aliveTexList.insert(texComp->texture);
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
