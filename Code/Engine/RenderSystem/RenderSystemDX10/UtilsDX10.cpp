/**
* UtilsDX10.cpp
* 26.08.2023
* (c) Denis Romakhov
*/

#include "pch.h"
#include "RenderSystem/RenderSystemDX10/UtilsDX10.h"
#include "Core/LCUtils.h"

#include <wincodec.h>


LcTextureLoaderDX10::LcTextureLoaderDX10() : factory(nullptr)
{
    HRESULT result = CoCreateInstance(
        CLSID_WICImagingFactory2, nullptr,
        CLSCTX_INPROC_SERVER, __uuidof(IWICImagingFactory2),
        (LPVOID*)&factory
    );
}

LcTextureLoaderDX10::~LcTextureLoaderDX10()
{
    if (factory)
    {
        factory->Release();
        factory = nullptr;
    }
}

bool LcTextureLoaderDX10::LoadTexture(const char* texPath, ID3D10Device* device, ID3D10Texture2D** texture, ID3D10ShaderResourceView** view)
{
    if (!device) return false;
    if (!texture && !view) return false;

    // read file
    auto texData = ReadBinaryFile(texPath);
    if (texData.empty()) return false;

    // create decoder
    ComPtr<IWICStream> stream;
    HRESULT result = factory->CreateStream(stream.GetAddressOf());
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
        if (view)
        {
            D3D10_SHADER_RESOURCE_VIEW_DESC SRVDesc{};
            SRVDesc.Format = desc.Format;
            SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
            SRVDesc.Texture2D.MipLevels = 1u;

            result = device->CreateShaderResourceView(*texture, &SRVDesc, view);
            if (FAILED(result)) return false;
        }

        return true;
    }

    return false;
}
