/**
* UtilsDX10.cpp
* 26.08.2023
* (c) Denis Romakhov
*/

#include "pch.h"
#include "RenderSystem/RenderSystemDX10/UtilsDX10.h"
#include "RenderSystem/RenderSystemDX10/RenderSystemDX10.h"
#include "World/SpriteInterface.h"
#include "Core/LCException.h"
#include "Core/LCUtils.h"

#include <set>
#include <cmath>


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

    outMode->RefreshRate.Numerator = 1;
    outMode->RefreshRate.Denominator = 1;
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
                    float prevRate = (float)outMode->RefreshRate.Numerator / (float)outMode->RefreshRate.Denominator;
                    float curRate = (float)mode.RefreshRate.Numerator / (float)mode.RefreshRate.Denominator;
                    if (curRate > prevRate)
                    {
                        *outMode = mode;
                    }
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
    LC_TRY

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

    // read png
    std::vector<uint8_t> data;
    int width, height, bpp, rowBytes;
    ReadPngFile(texPath, &width, &height, &bpp, &rowBytes);
    data.resize(rowBytes * height);
    ReadPngFile(texPath, &width, &height, &bpp, &rowBytes, data.data());
    const BYTE* texPixelsPtr = data.data();

    // create texture
    D3D10_TEXTURE2D_DESC desc{};
    desc.Width = width;
    desc.Height = height;
    desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    desc.MipLevels = 1;
    desc.ArraySize = 1;
    desc.SampleDesc.Count = 1;
    desc.BindFlags = D3D10_BIND_SHADER_RESOURCE;

    D3D10_SUBRESOURCE_DATA initData = { texPixelsPtr, static_cast<UINT>(width * 4), static_cast<UINT>(data.size()) };

    HRESULT result = device->CreateTexture2D(&desc, &initData, texture);
    if (SUCCEEDED(result) && texture)
    {
        LcTextureDataDX10 newTexData;
        newTexData.texture = *texture;
        newTexData.texSize = LcSize{ (int)width, (int)height };

        if (outTexSize) *outTexSize = newTexData.texSize;

        if (view)
        {
            D3D10_SHADER_RESOURCE_VIEW_DESC1 SRVDesc{};
            SRVDesc.Format = desc.Format;
            SRVDesc.ViewDimension = D3D10_SRV_DIMENSION_TEXTURE2D;
            SRVDesc.Texture2D.MipLevels = 1;

            result = device->CreateShaderResourceView1(*texture, &SRVDesc, view);
            if (FAILED(result)) return false;
            newTexData.view = *view;
        }

        texturesCache.emplace(std::make_pair(std::string(texPath), newTexData));
        return true;
    }

    LC_CATCH{ LC_THROW_EX("LcTextureLoaderDX10::LoadTexture('", texPath, "')"); }

    return false;
}

void LcTextureLoaderDX10::ClearCache(IWorld* world)
{
    LC_TRY

    if (world)
    {
        std::set<std::string> aliveTexList;
        auto& visuals = world->GetVisuals();
        for (auto visual : visuals)
        {
            if (auto texComp = visual->GetTextureComponent())
            {
                aliveTexList.insert(texComp->GetTexturePath());
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

    LC_CATCH{ LC_THROW("LcTextureLoaderDX10::ClearCache()") }
}
