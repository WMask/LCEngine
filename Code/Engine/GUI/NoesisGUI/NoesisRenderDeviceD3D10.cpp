/**
* NoesisRenderDeviceD3D10.cpp
* 04.02.2023
* (c) Denis Romakhov
*/

#include "pch.h"
#include "GUI/NoesisGUI/NoesisRenderDeviceD3D10.h"
#include "Core/LCUtils.h"

#include <NsCore/Log.h>
#include <NsCore/Ptr.h>
#include <NsCore/Memory.h>
#include <NsCore/Pair.h>
#include <NsCore/HighResTimer.h>
#include <NsCore/StringUtils.h>
#include <NsRender/Texture.h>
#include <NsRender/RenderTarget.h>
#include <NsApp/FastLZ.h>

#include <stdio.h>
#include <vector>


using namespace Noesis;
using namespace NoesisApp;


#define VS_CBUFFER0_SIZE 16 * sizeof(float)
#define VS_CBUFFER1_SIZE 4 * sizeof(float)
#define PS_CBUFFER0_SIZE 12 * sizeof(float)
#define PS_CBUFFER1_SIZE 128 * sizeof(float)

#define DX_RELEASE(o) \
    if (o != 0) \
    { \
        o->Release(); \
    }

#define DX_DESTROY(o) \
    if (o != 0) \
    { \
        ULONG refs = o->Release(); \
        NS_ASSERT(refs == 0); \
        o = nullptr; \
    }


class TextureD3D10 final: public Texture
{
public:
    TextureD3D10(ID3D10ShaderResourceView* view_, uint32_t width_, uint32_t height_,
        uint32_t levels_, bool isInverted_, bool hasAlpha_): view(view_), width(width_),
        height(height_), levels(levels_), isInverted(isInverted_), hasAlpha(hasAlpha_) {}

    ~TextureD3D10()
    {
        DX_DESTROY(view);
    }

    uint32_t GetWidth() const override { return width; }
    uint32_t GetHeight() const override { return height; }
    bool HasMipMaps() const override { return levels > 1; }
    bool IsInverted() const override { return isInverted; }
    bool HasAlpha() const override { return hasAlpha; }

    ID3D10ShaderResourceView* view;

    const uint32_t width;
    const uint32_t height;
    const uint32_t levels;
    const bool isInverted;
    const bool hasAlpha;
};


class RenderTargetD3D10 final: public RenderTarget
{
public:
    RenderTargetD3D10(uint32_t width_, uint32_t height_, MSAA::Enum msaa_): width(width_),
        height(height_), msaa(msaa_), textureRTV(0), color(0), colorRTV(0), colorSRV(0),
        stencil(0), stencilDSV(0) {}

    ~RenderTargetD3D10()
    {
        DX_DESTROY(textureRTV);

        DX_RELEASE(color);
        DX_DESTROY(colorRTV);
        DX_DESTROY(colorSRV);

        DX_RELEASE(stencil);
        DX_DESTROY(stencilDSV);

        texture.Reset();
    }

    Texture* GetTexture() override { return texture; }

    const uint32_t width;
    const uint32_t height;
    const MSAA::Enum msaa;

    Ptr<TextureD3D10> texture;
    ID3D10RenderTargetView* textureRTV;

    ID3D10Texture2D* color;
    ID3D10RenderTargetView* colorRTV;
    ID3D10ShaderResourceView* colorSRV;

    ID3D10Texture2D* stencil;
    ID3D10DepthStencilView* stencilDSV;
};


LcNoesisRenderDeviceD3D10::LcNoesisRenderDeviceD3D10(ID3D10Device* device, bool sRGB)
{
    mLayout = nullptr;
    mIndexBuffer = nullptr;
    mVertexShader = nullptr;
    mPixelShader = nullptr;
    mBlendState = nullptr;
    mRasterizerState = nullptr;
    mDepthStencilState = nullptr;
    mDevice = device;

    FillCaps(sRGB);

    CreateBuffers();
    CreateStateObjects();

    InvalidateStateCache();
}

LcNoesisRenderDeviceD3D10::~LcNoesisRenderDeviceD3D10()
{
    DestroyStateObjects();
    DestroyShaders();
    DestroyBuffers();

    mDevice = nullptr; // released in LcNoesisRenderContextD3D10
}

void LcNoesisRenderDeviceD3D10::LoadShaders(const char* folderPath)
{
    using namespace std::filesystem;

    for (auto& entry : directory_iterator(folderPath))
    {
        if (entry.is_regular_file())
        {
            auto name = entry.path().filename().string();
            auto content = ReadTextFile(entry.path().string().c_str());
            if (!name.empty() && !content.empty())
            {
                mShaderSource[name] = content;
            }
        }
    }

    CreateShaders();
}

Ptr<Texture> LcNoesisRenderDeviceD3D10::WrapTexture(ID3D10Texture2D* texture, uint32_t width,
    uint32_t height, uint32_t levels, bool isInverted, bool hasAlpha)
{
    NS_ASSERT(texture != 0);

    if (texture != 0)
    {
        ID3D10Device* device;
        texture->GetDevice(&device);

        D3D10_TEXTURE2D_DESC textureDesc;
        texture->GetDesc(&textureDesc);

        D3D10_SHADER_RESOURCE_VIEW_DESC viewDesc;
        viewDesc.ViewDimension = D3D10_SRV_DIMENSION_TEXTURE2D;
        viewDesc.Texture2D.MipLevels = unsigned int(-1);
        viewDesc.Texture2D.MostDetailedMip = 0;

        switch (textureDesc.Format)
        {
            case DXGI_FORMAT_R16G16B16A16_TYPELESS:
            {
                viewDesc.Format = DXGI_FORMAT_R16G16B16A16_UNORM;
                break;
            }
            case DXGI_FORMAT_R10G10B10A2_TYPELESS:
            {
                viewDesc.Format = DXGI_FORMAT_R10G10B10A2_UNORM;
                break;
            }
            case DXGI_FORMAT_R8G8B8A8_TYPELESS:
            {
                viewDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
                break;
            }
            case DXGI_FORMAT_B8G8R8X8_TYPELESS:
            {
                viewDesc.Format = DXGI_FORMAT_B8G8R8X8_UNORM;
                break;
            }
            case DXGI_FORMAT_B8G8R8A8_TYPELESS:
            {
                viewDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
                break;
            }
            case DXGI_FORMAT_BC1_TYPELESS:
            {
                viewDesc.Format = DXGI_FORMAT_BC1_UNORM;
                break;
            }
            case DXGI_FORMAT_BC2_TYPELESS:
            {
                viewDesc.Format = DXGI_FORMAT_BC2_UNORM;
                break;
            }
            case DXGI_FORMAT_BC3_TYPELESS:
            {
                viewDesc.Format = DXGI_FORMAT_BC3_UNORM;
                break;
            }
            case DXGI_FORMAT_BC4_TYPELESS:
            {
                viewDesc.Format = DXGI_FORMAT_BC4_UNORM;
                break;
            }
            case DXGI_FORMAT_BC5_TYPELESS:
            {
                viewDesc.Format = DXGI_FORMAT_BC5_UNORM;
                break;
            }
            case DXGI_FORMAT_BC7_TYPELESS:
            {
                viewDesc.Format = DXGI_FORMAT_BC7_UNORM;
                break;
            }
            default:
            {
                viewDesc.Format = textureDesc.Format;
                break;
            }
        }

        ID3D10ShaderResourceView* view;
        if (FAILED(device->CreateShaderResourceView(texture, &viewDesc, &view)))
        {
            throw std::exception("LcNoesisRenderDeviceD3D10::WrapTexture(): Cannot create shader resource view");
        }

        DX_RELEASE(device);

        return *new TextureD3D10(view, width, height, levels, isInverted, hasAlpha);
    }

    return 0;
}

void* LcNoesisRenderDeviceD3D10::CreatePixelShader(const char* label, const void* hlsl, uint32_t size)
{
    ID3D10PixelShader*& shader = mCustomShaders.EmplaceBack();
    if (FAILED(mDevice->CreatePixelShader(hlsl, size, &shader)))
    {
        throw std::exception("LcNoesisRenderDeviceD3D10::CreatePixelShader(): Cannot create pixel shader");
    }

    return (void*)(uintptr_t)mCustomShaders.Size();
}

void LcNoesisRenderDeviceD3D10::ClearPixelShaders()
{
    for (ID3D10PixelShader* shader : mCustomShaders)
    {
        DX_DESTROY(shader);
    }

    mCustomShaders.Clear();
}

const DeviceCaps& LcNoesisRenderDeviceD3D10::GetCaps() const
{
    return mCaps;
}

static MSAA::Enum ToMSAA(uint32_t sampleCount)
{
    uint32_t samples = Max(1U, Min(sampleCount, 16U));

    MSAA::Enum mssa = MSAA::x1;
    while (samples >>= 1)
    {
        mssa = (MSAA::Enum)((uint32_t)mssa + 1);
    }

    return mssa;
}

Ptr<RenderTarget> LcNoesisRenderDeviceD3D10::CreateRenderTarget(const char* label, uint32_t width,
    uint32_t height, uint32_t sampleCount, bool needsStencil)
{
    MSAA::Enum msaa = ToMSAA(mSampleDescs[(uint32_t)ToMSAA(sampleCount)].Count);

    D3D10_TEXTURE2D_DESC desc;
    desc.Width = width;
    desc.Height = height;
    desc.MipLevels = 1;
    desc.ArraySize = 1;
    desc.SampleDesc = mSampleDescs[(uint32_t)msaa];
    desc.Usage = D3D10_USAGE_DEFAULT;
    desc.CPUAccessFlags = 0;
    desc.MiscFlags = 0;

    ID3D10Texture2D* colorAA = 0;

    if (msaa != MSAA::x1)
    {
        if (mCaps.linearRendering)
        {
            desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
        }
        else
        {
            desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        }

        desc.BindFlags = D3D10_BIND_RENDER_TARGET | D3D10_BIND_SHADER_RESOURCE;

        if (FAILED(mDevice->CreateTexture2D(&desc, 0, &colorAA)))
        {
            throw std::exception("LcNoesisRenderDeviceD3D10::CreateRenderTarget(): Cannot create color texture");
        }
    }

    ID3D10Texture2D* stencil = nullptr;

    if (needsStencil)
    {
        desc.BindFlags = D3D10_BIND_DEPTH_STENCIL;
        desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
        if (FAILED(mDevice->CreateTexture2D(&desc, 0, &stencil)))
        {
            throw std::exception("LcNoesisRenderDeviceD3D10::CreateRenderTarget(): Cannot create stencil texture");
        }
    }

    return CreateRenderTarget(label, width, height, msaa, colorAA, stencil);
}

Ptr<RenderTarget> LcNoesisRenderDeviceD3D10::CloneRenderTarget(const char* label, RenderTarget* surface_)
{
    RenderTargetD3D10* surface = (RenderTargetD3D10*)surface_;

    ID3D10Texture2D* colorAA = 0;
    if (surface->msaa != MSAA::x1)
    {
        colorAA = surface->color;
        colorAA->AddRef();
    }

    ID3D10Texture2D* stencil = surface->stencil;
    stencil->AddRef();

    uint32_t width = surface->width;
    uint32_t height = surface->height;
    return CreateRenderTarget(label, width, height, surface->msaa, colorAA, stencil);
}

static DXGI_FORMAT DXGIFormat(TextureFormat::Enum format, bool sRGB)
{
    switch (format)
    {
        case TextureFormat::RGBA8:
        case TextureFormat::RGBX8:
            return sRGB ? DXGI_FORMAT_R8G8B8A8_UNORM_SRGB : DXGI_FORMAT_R8G8B8A8_UNORM;
        case TextureFormat::R8:
            return DXGI_FORMAT_R8_UNORM;
        default:
            NS_ASSERT_UNREACHABLE;
    }
}

static uint32_t TexelBytes(DXGI_FORMAT format)
{
    switch (format)
    {
        case DXGI_FORMAT_R8G8B8A8_UNORM: return 4;
        case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB: return 4;
        case DXGI_FORMAT_R8_UNORM: return 1;
        default: NS_ASSERT_UNREACHABLE;
    }
}

Ptr<Texture> LcNoesisRenderDeviceD3D10::CreateTexture(const char* label, uint32_t width, uint32_t height,
    uint32_t numLevels, TextureFormat::Enum format_, const void** data)
{
    DXGI_FORMAT format = DXGIFormat(format_, mCaps.linearRendering);

    D3D10_TEXTURE2D_DESC desc;
    desc.Width = width;
    desc.Height = height;
    desc.MipLevels = numLevels;
    desc.ArraySize = 1;
    desc.Format = format;
    desc.SampleDesc = { 1, 0 };
    desc.Usage = data ? D3D10_USAGE_IMMUTABLE : D3D10_USAGE_DEFAULT;
    desc.BindFlags = D3D10_BIND_SHADER_RESOURCE;
    desc.CPUAccessFlags = 0;
    desc.MiscFlags = 0;

    D3D10_SUBRESOURCE_DATA init[16];

    if (data)
    {
        uint32_t bpp = TexelBytes(format);

        for (uint32_t i = 0; i < numLevels; i++)
        {
            init[i].pSysMem = data[i];
            init[i].SysMemPitch = Max(width >> i, 1U) * bpp;
        }
    }

    ID3D10Texture2D* texture;
    if (FAILED(mDevice->CreateTexture2D(&desc, data ? init : 0, &texture)))
    {
        throw std::exception("LcNoesisRenderDeviceD3D10::CreateTexture(): Cannot create texture");
    }

    ID3D10ShaderResourceView* view;
    if (FAILED(mDevice->CreateShaderResourceView(texture, 0, &view)))
    {
        throw std::exception("LcNoesisRenderDeviceD3D10::CreateTexture(): Cannot create shader resource view");
    }
    DX_RELEASE(texture);

    bool hasAlpha = format_ == TextureFormat::RGBA8;
    return *new TextureD3D10(view, width, height, numLevels, false, hasAlpha);
}

void LcNoesisRenderDeviceD3D10::UpdateTexture(Texture* texture_, uint32_t level, uint32_t x, uint32_t y,
    uint32_t width, uint32_t height, const void* data)
{
    NS_ASSERT(level == 0);
    TextureD3D10* texture = (TextureD3D10*)texture_;

    ID3D10Resource* resource;
    texture->view->GetResource(&resource);

    D3D10_SHADER_RESOURCE_VIEW_DESC desc;
    texture->view->GetDesc(&desc);

    D3D10_BOX box = { x, y, 0, x + width, y + height, 1 };
    mDevice->UpdateSubresource(resource, 0, &box, data, width * TexelBytes(desc.Format), 0);

    DX_RELEASE(resource);
}

void LcNoesisRenderDeviceD3D10::BeginOffscreenRender()
{
    InvalidateStateCache();
}

void LcNoesisRenderDeviceD3D10::EndOffscreenRender()
{
    ClearTextures();
}

void LcNoesisRenderDeviceD3D10::BeginOnscreenRender()
{
    InvalidateStateCache();
}

void LcNoesisRenderDeviceD3D10::EndOnscreenRender()
{
    ClearTextures();
}

void LcNoesisRenderDeviceD3D10::SetRenderTarget(RenderTarget* surface_)
{
    ClearTextures();
    RenderTargetD3D10* surface = (RenderTargetD3D10*)surface_;
    mDevice->OMSetRenderTargets(1, &surface->colorRTV, surface->stencilDSV);

    D3D10_VIEWPORT viewport;
    viewport.TopLeftX = 0;
    viewport.TopLeftY = 0;
    viewport.Width = surface->width;
    viewport.Height = surface->height;
    viewport.MinDepth = 0.0f;
    viewport.MaxDepth = 1.0f;

    mDevice->RSSetViewports(1, &viewport);
}

void LcNoesisRenderDeviceD3D10::ResolveRenderTarget(RenderTarget* surface_, const Tile* tiles, uint32_t size)
{
    RenderTargetD3D10* surface = (RenderTargetD3D10*)surface_;

    if (surface->msaa != MSAA::x1)
    {
        SetInputLayout(0);
        SetVertexShader(mQuadVS);
        NS_ASSERT(surface->msaa - 1 < NS_COUNTOF(mResolvePS));
        SetPixelShader(mResolvePS[surface->msaa - 1]);

        SetRasterizerState(mRasterizerStateScissor);
        SetBlendState(mBlendStates[BlendMode::Src]);
        SetDepthStencilState(mDepthStencilStates[StencilMode::Disabled], 0);

        ClearTextures();
        mDevice->OMSetRenderTargets(1, &surface->textureRTV, 0);
        SetTexture(0, surface->colorSRV);

        for (uint32_t i = 0; i < size; i++)
        {
            const Tile& tile = tiles[i];

            D3D10_RECT rect;
            rect.left = tile.x;
            rect.top = surface->height - (tile.y + tile.height);
            rect.right = tile.x + tile.width;
            rect.bottom = surface->height - tile.y;
            mDevice->RSSetScissorRects(1, &rect);

            mDevice->Draw(3, 0);
        }
    }
}

void* LcNoesisRenderDeviceD3D10::MapVertices(uint32_t bytes)
{
    return MapBuffer(mVertices, bytes);
}

void LcNoesisRenderDeviceD3D10::UnmapVertices()
{
    UnmapBuffer(mVertices);
}

void* LcNoesisRenderDeviceD3D10::MapIndices(uint32_t bytes)
{
    return MapBuffer(mIndices, bytes);
}

void LcNoesisRenderDeviceD3D10::UnmapIndices()
{
    UnmapBuffer(mIndices);
}

void LcNoesisRenderDeviceD3D10::DrawBatch(const Batch& batch)
{
    NS_ASSERT(batch.shader.v < NS_COUNTOF(mPixelShaders));
    const PixelShader& psShader = mPixelShaders[batch.shader.v];

    NS_ASSERT(psShader.vsShader < NS_COUNTOF(mVertexShaders));
    const VertexShader& vsShader = mVertexShaders[psShader.vsShader];

    SetInputLayout(vsShader.layout);
    SetVertexShader(vsShader.shader);

    if (batch.pixelShader != nullptr)
    {
        NS_ASSERT((uintptr_t)batch.pixelShader <= mCustomShaders.Size());
        SetPixelShader(mCustomShaders[(int)(uintptr_t)batch.pixelShader - 1]);
    }
    else
    {
        NS_ASSERT(psShader.shader != nullptr);
        SetPixelShader(psShader.shader);
    }

    SetBuffers(batch, vsShader.stride);
    SetRenderState(batch);
    SetTextures(batch);

    mDevice->DrawIndexed(batch.numIndices, batch.startIndex + mIndices.drawPos / 2, 0);
}

void LcNoesisRenderDeviceD3D10::CreateBuffer(DynamicBuffer& buffer, uint32_t size, D3D10_BIND_FLAG flags, const char* label)
{
    buffer.size = size;
    buffer.pos = 0;
    buffer.drawPos = 0;
    buffer.flags = flags;

    D3D10_BUFFER_DESC desc = {};
    desc.ByteWidth = buffer.size;
    desc.BindFlags = buffer.flags;
    desc.Usage = D3D10_USAGE_DYNAMIC;
    desc.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE;

    if (FAILED(mDevice->CreateBuffer(&desc, 0, &buffer.obj)))
    {
        throw std::exception("LcNoesisRenderDeviceD3D10::CreateBuffer(): Cannot create buffer");
    }
}

void LcNoesisRenderDeviceD3D10::CreateBuffers()
{
    memset(mVertexCBHash, 0, sizeof(mVertexCBHash));
    memset(mPixelCBHash, 0, sizeof(mPixelCBHash));

    CreateBuffer(mVertices, DYNAMIC_VB_SIZE, D3D10_BIND_VERTEX_BUFFER, "Vertices");
    CreateBuffer(mIndices, DYNAMIC_IB_SIZE, D3D10_BIND_INDEX_BUFFER, "Indices");
    CreateBuffer(mVertexCB[0], VS_CBUFFER0_SIZE, D3D10_BIND_CONSTANT_BUFFER, "VertexCB0");
    CreateBuffer(mVertexCB[1], VS_CBUFFER1_SIZE, D3D10_BIND_CONSTANT_BUFFER, "VertexCB1");
    CreateBuffer(mPixelCB[0], PS_CBUFFER0_SIZE, D3D10_BIND_CONSTANT_BUFFER, "PixelCB0");
    CreateBuffer(mPixelCB[1], PS_CBUFFER1_SIZE, D3D10_BIND_CONSTANT_BUFFER, "PixelCB1");
}

void LcNoesisRenderDeviceD3D10::DestroyBuffers()
{
    DX_DESTROY(mVertices.obj);
    DX_DESTROY(mIndices.obj);

    for (uint32_t i = 0; i < NS_COUNTOF(mVertexCB); i++)
    {
        DX_DESTROY(mVertexCB[i].obj);
    }

    for (uint32_t i = 0; i < NS_COUNTOF(mPixelCB); i++)
    {
        DX_DESTROY(mPixelCB[i].obj);
    }
}

void* LcNoesisRenderDeviceD3D10::MapBuffer(DynamicBuffer& buffer, uint32_t size)
{
    NS_ASSERT(size <= buffer.size);

    // Make sure constant buffers are always discarded (no_overwrite not suported in Win7)
    if (buffer.flags & D3D10_BIND_CONSTANT_BUFFER)
    {
        buffer.pos = buffer.size;
    }

    D3D10_MAP type;

    if (buffer.pos + size > buffer.size)
    {
        type = D3D10_MAP_WRITE_DISCARD;
        buffer.pos = 0;
    }
    else
    {
        type = D3D10_MAP_WRITE_NO_OVERWRITE;
    }

    buffer.drawPos = buffer.pos;
    buffer.pos += size;

    void* mapped;
    if (FAILED(buffer.obj->Map(type, 0, &mapped)))
    {
        throw std::exception("LcNoesisRenderDeviceD3D10::MapBuffer(): Cannot map buffer");
    }

    return (uint8_t*)mapped + buffer.drawPos;
}

void LcNoesisRenderDeviceD3D10::UnmapBuffer(DynamicBuffer& buffer) const
{
    buffer.obj->Unmap();
}

static Pair<const char*, UINT> Semantic(uint32_t attr)
{
    switch (attr)
    {
        case Shader::Vertex::Format::Attr::Pos: return{ "POSITION", 0 };
        case Shader::Vertex::Format::Attr::Color: return { "COLOR", 0 };
        case Shader::Vertex::Format::Attr::Tex0: return { "TEXCOORD", 0 };
        case Shader::Vertex::Format::Attr::Tex1: return { "TEXCOORD", 1 };
        case Shader::Vertex::Format::Attr::Coverage: return { "COVERAGE", 0 };
        case Shader::Vertex::Format::Attr::Rect: return { "RECT", 0 };
        case Shader::Vertex::Format::Attr::Tile: return { "TILE", 0 };
        case Shader::Vertex::Format::Attr::ImagePos: return { "IMAGE_POSITION", 0 };
        default: NS_ASSERT_UNREACHABLE;
    }
}

static DXGI_FORMAT Format(uint32_t type)
{
    switch (type)
    {
        case Shader::Vertex::Format::Attr::Type::Float: return DXGI_FORMAT_R32_FLOAT;
        case Shader::Vertex::Format::Attr::Type::Float2: return DXGI_FORMAT_R32G32_FLOAT;
        case Shader::Vertex::Format::Attr::Type::Float4: return DXGI_FORMAT_R32G32B32A32_FLOAT;
        case Shader::Vertex::Format::Attr::Type::UByte4Norm: return DXGI_FORMAT_R8G8B8A8_UNORM;
        case Shader::Vertex::Format::Attr::Type::UShort4Norm: return DXGI_FORMAT_R16G16B16A16_UNORM;
        default: NS_ASSERT_UNREACHABLE;
    }
}

ID3D10InputLayout* LcNoesisRenderDeviceD3D10::CreateLayout(uint32_t attributes, const void* code, uint32_t size)
{
    D3D10_INPUT_ELEMENT_DESC descs[Shader::Vertex::Format::Attr::Count];
    uint32_t n = 0;

    for (uint32_t i = 0; i < Shader::Vertex::Format::Attr::Count; i++)
    {
        if (attributes & (1 << i))
        {
            descs[n].InputSlot = 0;
            descs[n].AlignedByteOffset = D3D10_APPEND_ALIGNED_ELEMENT;
            descs[n].InputSlotClass = D3D10_INPUT_PER_VERTEX_DATA;
            descs[n].InstanceDataStepRate = 0;
            descs[n].SemanticName = Semantic(i).first;
            descs[n].SemanticIndex = Semantic(i).second;
            descs[n].Format = Format(TypeForAttr[i]);
            n++;
        }
    }

    ID3D10InputLayout* layout;
    if (FAILED(mDevice->CreateInputLayout(descs, n, code, size, &layout)))
    {
        throw std::exception("LcNoesisRenderDeviceD3D10::CreateLayout(): Cannot create input layout");
    }
    return layout;
}

static void SetFilter(MinMagFilter::Enum minmag, MipFilter::Enum mip, D3D10_SAMPLER_DESC& desc)
{
    switch (minmag)
    {
        case MinMagFilter::Nearest:
        {
            switch (mip)
            {
                case MipFilter::Disabled:
                {
                    desc.MaxLOD = 0;
                    desc.Filter = D3D10_FILTER_MIN_MAG_MIP_POINT;
                    break;
                }
                case MipFilter::Nearest:
                {
                    desc.MaxLOD = D3D10_FLOAT32_MAX;
                    desc.Filter = D3D10_FILTER_MIN_MAG_MIP_POINT;
                    break;
                }
                case MipFilter::Linear:
                {
                    desc.MaxLOD = D3D10_FLOAT32_MAX;
                    desc.Filter = D3D10_FILTER_MIN_MAG_POINT_MIP_LINEAR;
                    break;
                }
                default:
                {
                    NS_ASSERT_UNREACHABLE;
                }
            }

            break;
        }
        case MinMagFilter::Linear:
        {
            switch (mip)
            {
                case MipFilter::Disabled:
                {
                    desc.MaxLOD = 0;
                    desc.Filter = D3D10_FILTER_MIN_MAG_LINEAR_MIP_POINT;
                    break;
                }
                case MipFilter::Nearest:
                {
                    desc.MaxLOD = D3D10_FLOAT32_MAX;
                    desc.Filter = D3D10_FILTER_MIN_MAG_LINEAR_MIP_POINT;
                    break;
                }
                case MipFilter::Linear:
                {
                    desc.MaxLOD = D3D10_FLOAT32_MAX;
                    desc.Filter = D3D10_FILTER_MIN_MAG_MIP_LINEAR;
                    break;
                }
                default:
                {
                    NS_ASSERT_UNREACHABLE;
                }
            }

            break;
        }
        default:
        {
            NS_ASSERT_UNREACHABLE;
        }
    }
}

static void SetAddress(WrapMode::Enum mode, D3D_FEATURE_LEVEL level, D3D10_SAMPLER_DESC& desc)
{
    switch (mode)
    {
        case WrapMode::ClampToEdge:
        {
            desc.AddressU = D3D10_TEXTURE_ADDRESS_CLAMP;
            desc.AddressV = D3D10_TEXTURE_ADDRESS_CLAMP;
            break;
        }
        case WrapMode::ClampToZero:
        {
            bool hasBorder = level >= D3D_FEATURE_LEVEL_9_3;
            desc.AddressU = hasBorder? D3D10_TEXTURE_ADDRESS_BORDER : D3D10_TEXTURE_ADDRESS_CLAMP;
            desc.AddressV = hasBorder? D3D10_TEXTURE_ADDRESS_BORDER : D3D10_TEXTURE_ADDRESS_CLAMP;
            break;
        }
        case WrapMode::Repeat:
        {
            desc.AddressU = D3D10_TEXTURE_ADDRESS_WRAP;
            desc.AddressV = D3D10_TEXTURE_ADDRESS_WRAP;
            break;
        }
        case WrapMode::MirrorU:
        {
            desc.AddressU = D3D10_TEXTURE_ADDRESS_MIRROR;
            desc.AddressV = D3D10_TEXTURE_ADDRESS_WRAP;
            break;
        }
        case WrapMode::MirrorV:
        {
            desc.AddressU = D3D10_TEXTURE_ADDRESS_WRAP;
            desc.AddressV = D3D10_TEXTURE_ADDRESS_MIRROR;
            break;
        }
        case WrapMode::Mirror:
        {
            desc.AddressU = D3D10_TEXTURE_ADDRESS_MIRROR;
            desc.AddressV = D3D10_TEXTURE_ADDRESS_MIRROR;
            break;
        }
        default:
        {
            NS_ASSERT_UNREACHABLE;
        }
    }
}

void LcNoesisRenderDeviceD3D10::CreateStateObjects()
{
    // Rasterized states
    {
        D3D10_RASTERIZER_DESC desc;
        desc.CullMode = D3D10_CULL_NONE;
        desc.FrontCounterClockwise = false;
        desc.DepthBias = 0;
        desc.DepthBiasClamp = 0.0f;
        desc.SlopeScaledDepthBias = 0.0f;
        desc.DepthClipEnable = true;
        desc.MultisampleEnable = true;
        desc.AntialiasedLineEnable = false;

        desc.FillMode = D3D10_FILL_SOLID;
        desc.ScissorEnable = false;
        if (FAILED(mDevice->CreateRasterizerState(&desc, &mRasterizerStates[0])))
        {
            throw std::exception("LcNoesisRenderDeviceD3D10::CreateStateObjects(): Cannot create rasterizer state");
        }

        desc.FillMode = D3D10_FILL_WIREFRAME;
        desc.ScissorEnable = false;
        if (FAILED(mDevice->CreateRasterizerState(&desc, &mRasterizerStates[1])))
        {
            throw std::exception("LcNoesisRenderDeviceD3D10::CreateStateObjects(): Cannot create rasterizer state");
        }

        desc.FillMode = D3D10_FILL_SOLID;
        desc.ScissorEnable = true;
        if (FAILED(mDevice->CreateRasterizerState(&desc, &mRasterizerStateScissor)))
        {
            throw std::exception("LcNoesisRenderDeviceD3D10::CreateStateObjects(): Cannot create rasterizer state");
        }
    }

    // Blend states
    {
        static_assert(NS_COUNTOF(mBlendStates) == BlendMode::Count, "");

        D3D10_BLEND_DESC desc;
        ZeroMemory(&desc, sizeof(D3D10_BLEND_DESC));
        desc.BlendEnable[0] = TRUE;
        desc.SrcBlend = D3D10_BLEND_ONE;
        desc.DestBlend = D3D10_BLEND_INV_SRC_ALPHA;
        desc.BlendOp = D3D10_BLEND_OP_ADD;
        desc.SrcBlendAlpha = D3D10_BLEND_ONE;
        desc.DestBlendAlpha = D3D10_BLEND_INV_SRC_ALPHA;
        desc.BlendOpAlpha = D3D10_BLEND_OP_ADD;
        desc.RenderTargetWriteMask[0] = D3D10_COLOR_WRITE_ENABLE_ALL;

        // Src
        desc.BlendEnable[0] = false;
        if (FAILED(mDevice->CreateBlendState(&desc, &mBlendStates[BlendMode::Src])))
        {
            throw std::exception("LcNoesisRenderDeviceD3D10::CreateStateObjects(): Cannot create blend state");
        }

        // SrcOver
        desc.BlendEnable[0] = true;
        if (FAILED(mDevice->CreateBlendState(&desc, &mBlendStates[BlendMode::SrcOver])))
        {
            throw std::exception("LcNoesisRenderDeviceD3D10::CreateStateObjects(): Cannot create blend state");
        }

        // SrcOver_Multiply
        desc.SrcBlend = D3D10_BLEND_DEST_COLOR;
        desc.DestBlend = D3D10_BLEND_INV_SRC_ALPHA;
        if (FAILED(mDevice->CreateBlendState(&desc, &mBlendStates[BlendMode::SrcOver_Multiply])))
        {
            throw std::exception("LcNoesisRenderDeviceD3D10::CreateStateObjects(): Cannot create blend state");
        }

        // SrcOver_Screen
        desc.SrcBlend = D3D10_BLEND_ONE;
        desc.DestBlend = D3D10_BLEND_INV_SRC_COLOR;
        if (FAILED(mDevice->CreateBlendState(&desc, &mBlendStates[BlendMode::SrcOver_Screen])))
        {
            throw std::exception("LcNoesisRenderDeviceD3D10::CreateStateObjects(): Cannot create blend state");
        }

        // SrcOver_Additive
        desc.SrcBlend = D3D10_BLEND_ONE;
        desc.DestBlend = D3D10_BLEND_ONE;
        if (FAILED(mDevice->CreateBlendState(&desc, &mBlendStates[BlendMode::SrcOver_Additive])))
        {
            throw std::exception("LcNoesisRenderDeviceD3D10::CreateStateObjects(): Cannot create blend state");
        }

        // SrcOver_Dual
        desc.SrcBlend = D3D10_BLEND_ONE;
        desc.DestBlend = D3D10_BLEND_INV_SRC1_COLOR;
        desc.SrcBlendAlpha = D3D10_BLEND_ONE;
        desc.DestBlendAlpha = D3D10_BLEND_INV_SRC1_ALPHA;
        if (FAILED(mDevice->CreateBlendState(&desc, &mBlendStates[BlendMode::SrcOver_Dual])))
        {
            throw std::exception("LcNoesisRenderDeviceD3D10::CreateStateObjects(): Cannot create blend state");
        }

        // Color disabled
        desc.BlendEnable[0] = false;
        desc.RenderTargetWriteMask[0] = 0;
        if (FAILED(mDevice->CreateBlendState(&desc, &mBlendStateNoColor)))
        {
            throw std::exception("LcNoesisRenderDeviceD3D10::CreateStateObjects(): Cannot create blend state");
        }
    }

    // Depth states
    {
        static_assert(NS_COUNTOF(mDepthStencilStates) == StencilMode::Count, "");

        D3D10_DEPTH_STENCIL_DESC desc;
        desc.DepthEnable = false;
        desc.DepthWriteMask = D3D10_DEPTH_WRITE_MASK_ZERO;
        desc.DepthFunc = D3D10_COMPARISON_NEVER;
        desc.StencilReadMask = 0xff;
        desc.StencilWriteMask = 0xff;
        desc.FrontFace.StencilFunc = D3D10_COMPARISON_EQUAL;
        desc.FrontFace.StencilDepthFailOp = D3D10_STENCIL_OP_KEEP;
        desc.FrontFace.StencilFailOp = D3D10_STENCIL_OP_KEEP;
        desc.BackFace.StencilFunc = D3D10_COMPARISON_EQUAL;
        desc.BackFace.StencilDepthFailOp = D3D10_STENCIL_OP_KEEP;
        desc.BackFace.StencilFailOp = D3D10_STENCIL_OP_KEEP;

        // Disabled
        desc.StencilEnable = false;
        desc.FrontFace.StencilPassOp = D3D10_STENCIL_OP_KEEP;
        desc.BackFace.StencilPassOp = D3D10_STENCIL_OP_KEEP;
        if (FAILED(mDevice->CreateDepthStencilState(&desc, &mDepthStencilStates[StencilMode::Disabled])))
        {
            throw std::exception("LcNoesisRenderDeviceD3D10::CreateStateObjects(): Cannot create depth stencil state");
        }

        // Equal_Keep
        desc.StencilEnable = true;
        desc.FrontFace.StencilPassOp = D3D10_STENCIL_OP_KEEP;
        desc.BackFace.StencilPassOp = D3D10_STENCIL_OP_KEEP;
        if (FAILED(mDevice->CreateDepthStencilState(&desc, &mDepthStencilStates[StencilMode::Equal_Keep])))
        {
            throw std::exception("LcNoesisRenderDeviceD3D10::CreateStateObjects(): Cannot create depth stencil state");
        }

        // Equal_Incr
        desc.StencilEnable = true;
        desc.FrontFace.StencilPassOp = D3D10_STENCIL_OP_INCR;
        desc.BackFace.StencilPassOp = D3D10_STENCIL_OP_INCR;
        if (FAILED(mDevice->CreateDepthStencilState(&desc, &mDepthStencilStates[StencilMode::Equal_Incr])))
        {
            throw std::exception("LcNoesisRenderDeviceD3D10::CreateStateObjects(): Cannot create depth stencil state");
        }

        // Equal_Decr
        desc.StencilEnable = true;
        desc.FrontFace.StencilPassOp = D3D10_STENCIL_OP_DECR;
        desc.BackFace.StencilPassOp = D3D10_STENCIL_OP_DECR;
        if (FAILED(mDevice->CreateDepthStencilState(&desc, &mDepthStencilStates[StencilMode::Equal_Decr])))
        {
            throw std::exception("LcNoesisRenderDeviceD3D10::CreateStateObjects(): Cannot create depth stencil state");
        }

        // Clear
        desc.StencilEnable = true;
        desc.FrontFace.StencilFunc = D3D10_COMPARISON_ALWAYS;
        desc.FrontFace.StencilPassOp = D3D10_STENCIL_OP_ZERO;
        desc.BackFace.StencilFunc = D3D10_COMPARISON_ALWAYS;
        desc.BackFace.StencilPassOp = D3D10_STENCIL_OP_ZERO;
        if (FAILED(mDevice->CreateDepthStencilState(&desc, &mDepthStencilStates[StencilMode::Clear])))
        {
            throw std::exception("LcNoesisRenderDeviceD3D10::CreateStateObjects(): Cannot create depth stencil state");
        }
    }

    // Sampler states
    {
        memset(mSamplerStates, 0, sizeof(mSamplerStates));

        D3D10_SAMPLER_DESC desc = {};
        desc.AddressW = D3D10_TEXTURE_ADDRESS_CLAMP;
        desc.MaxAnisotropy = 1;
        desc.ComparisonFunc = D3D10_COMPARISON_NEVER;
        desc.MinLOD = -D3D10_FLOAT32_MAX;
        desc.MipLODBias = -0.75f;

        const char* MinMagStr[] = { "Nearest", "Linear" };
        const char* MipStr[] = { "Disabled", "Nearest", "Linear" };
        const char* WrapStr[] = { "ClampToEdge", "ClampToZero", "Repeat", "MirrorU", "MirrorV", "Mirror" };

        for (uint8_t minmag = 0; minmag < MinMagFilter::Count; minmag++)
        {
            for (uint8_t mip = 0; mip < MipFilter::Count; mip++)
            {
                SetFilter(MinMagFilter::Enum(minmag), MipFilter::Enum(mip), desc);

                for (uint8_t uv = 0; uv < WrapMode::Count; uv++)
                {
                    SetAddress(WrapMode::Enum(uv), D3D_FEATURE_LEVEL::D3D_FEATURE_LEVEL_10_1, desc);

                    SamplerState s = {{uv, minmag, mip}};
                    NS_ASSERT(s.v < NS_COUNTOF(mSamplerStates));
                    if (FAILED(mDevice->CreateSamplerState(&desc, &mSamplerStates[s.v])))
                    {
                        throw std::exception("LcNoesisRenderDeviceD3D10::CreateStateObjects(): Cannot create sampler state");
                    }
                }
            }
        }
    }
}

void LcNoesisRenderDeviceD3D10::DestroyStateObjects()
{
    for (uint32_t i = 0; i < NS_COUNTOF(mRasterizerStates); i++)
    {
        DX_RELEASE(mRasterizerStates[i]);
    }

    DX_RELEASE(mRasterizerStateScissor);

    for (uint32_t i = 0; i < NS_COUNTOF(mBlendStates); i++)
    {
        DX_RELEASE(mBlendStates[i]);
    }

    DX_RELEASE(mBlendStateNoColor);

    for (uint32_t i = 0; i < NS_COUNTOF(mDepthStencilStates); i++)
    {
        DX_RELEASE(mDepthStencilStates[i]);
    }

    for (uint32_t i = 0; i < NS_COUNTOF(mSamplerStates); i++)
    {
        DX_RELEASE(mSamplerStates[i]);
    }
}

void LcNoesisRenderDeviceD3D10::CreateShaders()
{
    auto vsShaders = [this](uint32_t shader)
    {
        const char* code = mShaderSource["Pos.shader"].c_str();

        switch (shader)
        {
        case Noesis::Shader::Vertex::Pos:
        case Noesis::Shader::Vertex::PosColor:
        case Noesis::Shader::Vertex::PosTex0:
        case Noesis::Shader::Vertex::PosTex0Rect:
        case Noesis::Shader::Vertex::PosTex0RectTile:
            break;
        case Noesis::Shader::Vertex::PosColorCoverage:
            code = mShaderSource["PosColorCoverage.shader"].c_str();
            break;
        case Noesis::Shader::Vertex::PosTex0Coverage:
        case Noesis::Shader::Vertex::PosTex0CoverageRect:
        case Noesis::Shader::Vertex::PosTex0CoverageRectTile:
        case Noesis::Shader::Vertex::PosColorTex1_SDF:
        case Noesis::Shader::Vertex::PosTex0Tex1_SDF:
        case Noesis::Shader::Vertex::PosTex0Tex1Rect_SDF:
        case Noesis::Shader::Vertex::PosTex0Tex1RectTile_SDF:
        case Noesis::Shader::Vertex::PosColorTex1:
        case Noesis::Shader::Vertex::PosTex0Tex1:
        case Noesis::Shader::Vertex::PosTex0Tex1Rect:
        case Noesis::Shader::Vertex::PosTex0Tex1RectTile:
        case Noesis::Shader::Vertex::PosColorTex0Tex1:
        case Noesis::Shader::Vertex::PosTex0Tex1_Downsample:
        case Noesis::Shader::Vertex::PosColorTex1Rect:
        case Noesis::Shader::Vertex::PosColorTex0RectImagePos:
            break;
        }

        return code;
    };

    memset(mLayouts, 0, sizeof(mLayouts));
    std::vector<char> shCompiled;

    for (uint32_t i = 0; i < Shader::Vertex::Count; i++)
    {
        const char* code = vsShaders(i);

        ID3D10Blob* vertexBlob;
        if (FAILED(D3D10CompileShader(code, strlen(code), NULL, NULL, NULL, "main", "vs_4_0", 0, &vertexBlob, NULL)))
        {
            throw std::exception("LcNoesisRenderDeviceD3D10::CreateShaders(): Cannot compile vertex shader");
        }

        shCompiled.resize(vertexBlob->GetBufferSize());
        memcpy(shCompiled.data(), vertexBlob->GetBufferPointer(), vertexBlob->GetBufferSize());
        vertexBlob->Release();

        if (FAILED(mDevice->CreateVertexShader(shCompiled.data(), shCompiled.size(), &mVertexShaders[i].shader)))
        {
            throw std::exception("LcNoesisRenderDeviceD3D10::CreateShaders(): Cannot create vertex shader");
        }

        uint32_t format = FormatForVertex[i];
        NS_ASSERT(format < NS_COUNTOF(mLayouts));

        if (mLayouts[format] == 0)
        {
            uint32_t attributes = AttributesForFormat[format];
            mLayouts[format] = CreateLayout(attributes, shCompiled.data(), (uint32_t)shCompiled.size());
        }

        mVertexShaders[i].layout = mLayouts[format];
        mVertexShaders[i].stride = SizeForFormat[format];
    }

    struct ShaderPS
    {
        const BYTE* code;
        uint32_t size;
    };

    auto psShaders = [this](uint32_t shader)
    {
        const char* code = mShaderSource["Path_AA_Solid.shader"].c_str();

        switch (shader)
        {
        case Noesis::Shader::RGBA:
        case Noesis::Shader::Mask:
        case Noesis::Shader::Clear:

        case Noesis::Shader::Path_Solid:
        case Noesis::Shader::Path_Linear:
        case Noesis::Shader::Path_Radial:
        case Noesis::Shader::Path_Pattern:
        case Noesis::Shader::Path_Pattern_Clamp:
        case Noesis::Shader::Path_Pattern_Repeat:
        case Noesis::Shader::Path_Pattern_MirrorU:
        case Noesis::Shader::Path_Pattern_MirrorV:
        case Noesis::Shader::Path_Pattern_Mirror:

        case Noesis::Shader::Path_AA_Solid:
        case Noesis::Shader::Path_AA_Linear:
        case Noesis::Shader::Path_AA_Radial:
        case Noesis::Shader::Path_AA_Pattern:
        case Noesis::Shader::Path_AA_Pattern_Clamp:
        case Noesis::Shader::Path_AA_Pattern_Repeat:
        case Noesis::Shader::Path_AA_Pattern_MirrorU:
        case Noesis::Shader::Path_AA_Pattern_MirrorV:
        case Noesis::Shader::Path_AA_Pattern_Mirror:

        case Noesis::Shader::SDF_Solid:
        case Noesis::Shader::SDF_Linear:
        case Noesis::Shader::SDF_Radial:
        case Noesis::Shader::SDF_Pattern:
        case Noesis::Shader::SDF_Pattern_Clamp:
        case Noesis::Shader::SDF_Pattern_Repeat:
        case Noesis::Shader::SDF_Pattern_MirrorU:
        case Noesis::Shader::SDF_Pattern_MirrorV:
        case Noesis::Shader::SDF_Pattern_Mirror:

        case Noesis::Shader::SDF_LCD_Solid:
        case Noesis::Shader::SDF_LCD_Linear:
        case Noesis::Shader::SDF_LCD_Radial:
        case Noesis::Shader::SDF_LCD_Pattern:
        case Noesis::Shader::SDF_LCD_Pattern_Clamp:
        case Noesis::Shader::SDF_LCD_Pattern_Repeat:
        case Noesis::Shader::SDF_LCD_Pattern_MirrorU:
        case Noesis::Shader::SDF_LCD_Pattern_MirrorV:
        case Noesis::Shader::SDF_LCD_Pattern_Mirror:

        case Noesis::Shader::Opacity_Solid:
        case Noesis::Shader::Opacity_Linear:
        case Noesis::Shader::Opacity_Radial:
        case Noesis::Shader::Opacity_Pattern:
        case Noesis::Shader::Opacity_Pattern_Clamp:
        case Noesis::Shader::Opacity_Pattern_Repeat:
        case Noesis::Shader::Opacity_Pattern_MirrorU:
        case Noesis::Shader::Opacity_Pattern_MirrorV:
        case Noesis::Shader::Opacity_Pattern_Mirror:

        case Noesis::Shader::Upsample:
        case Noesis::Shader::Downsample:

        case Noesis::Shader::Shadow:
        case Noesis::Shader::Blur:
        case Noesis::Shader::Custom_Effect:
            break;
        }

        return code;
    };

    for (uint32_t i = 0; i < Shader::Count; i++)
    {
        const char* code = psShaders(i);

        ID3D10Blob* pixelBlob;
        if (FAILED(D3D10CompileShader(code, strlen(code), NULL, NULL, NULL, "main", "ps_4_0", 0, &pixelBlob, 0)))
        {
            throw std::exception("LcNoesisRenderDeviceD3D10::CreateShaders(): Cannot compile pixel shader");
        }

        shCompiled.resize(pixelBlob->GetBufferSize());
        memcpy(shCompiled.data(), pixelBlob->GetBufferPointer(), pixelBlob->GetBufferSize());
        pixelBlob->Release();

        if (FAILED(mDevice->CreatePixelShader(shCompiled.data(), shCompiled.size(), &mPixelShaders[i].shader)))
        {
            throw std::exception("LcNoesisRenderDeviceD3D10::CreateShaders(): Cannot create pixel shader");
        }

        mPixelShaders[i].vsShader = VertexForShader[i];
    }

    ID3D10Blob* shBlob;
    const char* quadVS = "float4 main(in uint id: SV_VertexID): SV_Position {float4 o = 0.0f; if(id == 0) {o = float4(-1.0f, 1.0f, 1.0f, 1.0f);} else if(id == 1) {o = float4(3.0f, 1.0f, 1.0f, 1.0f);} else {o = float4(-1.0f, -3.0f, 1.0f, 1.0f);} return o;}";
    if (FAILED(D3D10CompileShader((LPCSTR)quadVS, strlen(quadVS), NULL, NULL, NULL, "main", "vs_4_0", 0, &shBlob, 0)))
    {
        throw std::exception("LcNoesisRenderDeviceD3D10::CreateShaders(): Cannot compile Quad_VS shader");
    }

    if (FAILED(mDevice->CreateVertexShader(shBlob->GetBufferPointer(), shBlob->GetBufferSize(), &mQuadVS)))
    {
        shBlob->Release();
        throw std::exception("LcNoesisRenderDeviceD3D10::CreateShaders(): Cannot create Quad_VS shader");
    }

    shBlob->Release();
    char resolvePS[1024];
    const char* resolve = "Texture2DMS<float4, %d> t; float4 main(in float4 pos: SV_Position): SV_Target {float4 o = 0.0f; [unroll]for (int s = 0; s < %d; s++) {o += t.Load(pos.xy, s);} return o / %d;}";

    sprintf_s(resolvePS, resolve, 2);
    if (FAILED(D3D10CompileShader(resolvePS, strlen(resolvePS), NULL, NULL, NULL, "main", "ps_4_0", 0, &shBlob, 0)))
    {
        throw std::exception("LcNoesisRenderDeviceD3D10::CreateShaders(): Cannot compile Resolve2_PS shader");
    }

    if (FAILED(mDevice->CreatePixelShader(shBlob->GetBufferPointer(), shBlob->GetBufferSize(), &mResolvePS[0])))
    {
        shBlob->Release();
        throw std::exception("LcNoesisRenderDeviceD3D10::CreateShaders(): Cannot create Resolve2_PS shader");
    }

    shBlob->Release();
    sprintf_s(resolvePS, resolve, 4);
    if (FAILED(D3D10CompileShader(resolvePS, strlen(resolvePS), NULL, NULL, NULL, "main", "ps_4_0", 0, &shBlob, 0)))
    {
        throw std::exception("LcNoesisRenderDeviceD3D10::CreateShaders(): Cannot compile Resolve4_PS shader");
    }

    if (FAILED(mDevice->CreatePixelShader(shBlob->GetBufferPointer(), shBlob->GetBufferSize(), &mResolvePS[1])))
    {
        shBlob->Release();
        throw std::exception("LcNoesisRenderDeviceD3D10::CreateShaders(): Cannot create Resolve4_PS shader");
    }

    shBlob->Release();
    sprintf_s(resolvePS, resolve, 8);
    if (FAILED(D3D10CompileShader(resolvePS, strlen(resolvePS), NULL, NULL, NULL, "main", "ps_4_0", 0, &shBlob, 0)))
    {
        throw std::exception("LcNoesisRenderDeviceD3D10::CreateShaders(): Cannot compile Resolve8_PS shader");
    }

    if (FAILED(mDevice->CreatePixelShader(shBlob->GetBufferPointer(), shBlob->GetBufferSize(), &mResolvePS[2])))
    {
        shBlob->Release();
        throw std::exception("LcNoesisRenderDeviceD3D10::CreateShaders(): Cannot create Resolve8_PS shader");
    }

    shBlob->Release();
    sprintf_s(resolvePS, resolve, 16);
    if (FAILED(D3D10CompileShader(resolvePS, strlen(resolvePS), NULL, NULL, NULL, "main", "ps_4_0", 0, &shBlob, 0)))
    {
        throw std::exception("LcNoesisRenderDeviceD3D10::CreateShaders(): Cannot compile Resolve16_PS shader");
    }

    if (FAILED(mDevice->CreatePixelShader(shBlob->GetBufferPointer(), shBlob->GetBufferSize(), &mResolvePS[3])))
    {
        shBlob->Release();
        throw std::exception("LcNoesisRenderDeviceD3D10::CreateShaders(): Cannot create Resolve16_PS shader");
    }

    shBlob->Release();
}

void LcNoesisRenderDeviceD3D10::DestroyShaders()
{
    for (uint32_t i = 0; i < NS_COUNTOF(mLayouts); i++)
    {
        DX_DESTROY(mLayouts[i]);
    }

    for (uint32_t i = 0; i < NS_COUNTOF(mVertexShaders); i++)
    {
        DX_DESTROY(mVertexShaders[i].shader);
    }

    for (uint32_t i = 0; i < NS_COUNTOF(mPixelShaders); i++)
    {
        DX_DESTROY(mPixelShaders[i].shader);
    }

    for (uint32_t i = 0; i < NS_COUNTOF(mResolvePS); i++)
    {
        DX_DESTROY(mResolvePS[i]);
    }

    for (ID3D10PixelShader* shader : mCustomShaders)
    {
        DX_DESTROY(shader);
    }

    DX_DESTROY(mQuadVS);
}

Ptr<RenderTarget> LcNoesisRenderDeviceD3D10::CreateRenderTarget(const char* label, uint32_t width,
    uint32_t height, MSAA::Enum msaa, ID3D10Texture2D* colorAA, ID3D10Texture2D* stencil)
{
    Ptr<RenderTargetD3D10> surface = *new RenderTargetD3D10(width, height, msaa);

    bool sRGB = mCaps.linearRendering;

    // Texture
    D3D10_TEXTURE2D_DESC desc;
    desc.Width = width;
    desc.Height = height;
    desc.MipLevels = 1;
    desc.ArraySize = 1;
    desc.Format = sRGB ? DXGI_FORMAT_R8G8B8A8_UNORM_SRGB : DXGI_FORMAT_R8G8B8A8_UNORM;
    desc.SampleDesc.Count = 1;
    desc.SampleDesc.Quality = 0;
    desc.Usage = D3D10_USAGE_DEFAULT;
    desc.BindFlags = D3D10_BIND_SHADER_RESOURCE | D3D10_BIND_RENDER_TARGET;
    desc.CPUAccessFlags = 0;
    desc.MiscFlags = 0;

    ID3D10Texture2D* colorTex;
    if (FAILED(mDevice->CreateTexture2D(&desc, 0, &colorTex)))
    {
        throw std::exception("LcNoesisRenderDeviceD3D10::CreateRenderTarget(): Cannot create texture");
    }

    ID3D10ShaderResourceView* viewTex;
    if (FAILED(mDevice->CreateShaderResourceView(colorTex, 0, &viewTex)))
    {
        throw std::exception("LcNoesisRenderDeviceD3D10::CreateRenderTarget(): Cannot create resource view");
    }
    DX_RELEASE(colorTex);

    surface->texture = *new TextureD3D10(viewTex, width, height, 1, false, true);

    if (FAILED(mDevice->CreateRenderTargetView(colorTex, 0, &surface->textureRTV)))
    {
        throw std::exception("LcNoesisRenderDeviceD3D10::CreateRenderTarget(): Cannot create render target view");
    }

    // Color
    if (colorAA != 0)
    {
        NS_ASSERT(msaa != MSAA::x1);
        surface->color = colorAA;
    }
    else
    {
        NS_ASSERT(msaa == MSAA::x1);
        surface->color = colorTex;
        surface->color->AddRef();
    }

    if (FAILED(mDevice->CreateRenderTargetView(surface->color, 0, &surface->colorRTV)))
    {
        throw std::exception("LcNoesisRenderDeviceD3D10::CreateRenderTarget(): Cannot create render target view");
    }

    if (FAILED(mDevice->CreateShaderResourceView(surface->color, 0, &surface->colorSRV)))
    {
        throw std::exception("LcNoesisRenderDeviceD3D10::CreateRenderTarget(): Cannot create shader resource view");
    }

    // Stencil
    surface->stencil = stencil;
    surface->stencilDSV = 0;

    if (stencil)
    {
        if (FAILED(mDevice->CreateDepthStencilView(surface->stencil, 0, &surface->stencilDSV)))
        {
            throw std::exception("LcNoesisRenderDeviceD3D10::CreateRenderTarget(): Cannot create depth stencil view");
        }
    }

    return surface;
}

void LcNoesisRenderDeviceD3D10::CheckMultisample()
{
    NS_ASSERT(mDevice != 0);

    unsigned int counts[MSAA::Count] = {1, 2, 4, 8, 16};

    for (uint32_t i = 0, last = 0; i < NS_COUNTOF(counts); i++)
    {
        DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM;
        unsigned int count = counts[i];
        unsigned int quality = 0;

        HRESULT hr = mDevice->CheckMultisampleQualityLevels(format, count, &quality);

        if (SUCCEEDED(hr) && quality > 0)
        {
            mSampleDescs[i].Count = count;
            mSampleDescs[i].Quality = 0;
            last = i;
        }
        else
        {
            mSampleDescs[i] = mSampleDescs[last];
        }
    }
}

void LcNoesisRenderDeviceD3D10::FillCaps(bool sRGB)
{
    CheckMultisample();

    mCaps.centerPixelOffset = 0.0f;
    mCaps.linearRendering = sRGB;
    mCaps.subpixelRendering = true;
}

void LcNoesisRenderDeviceD3D10::InvalidateStateCache()
{
    ID3D10Buffer* vsConstants[] = { mVertexCB[0].obj, mVertexCB[1].obj };
    mDevice->VSSetConstantBuffers(0, 2, vsConstants);

    ID3D10Buffer* psConstants[] = { mPixelCB[0].obj, mPixelCB[1].obj };
    mDevice->PSSetConstantBuffers(0, 2, psConstants);

    mDevice->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    mDevice->SetPredication(nullptr, 0);

    mIndexBuffer = nullptr;
    mLayout = nullptr;
    mVertexShader = nullptr;
    mPixelShader = nullptr;
    mRasterizerState = nullptr;
    mBlendState = nullptr;
    mDepthStencilState = nullptr;
    mStencilRef = (unsigned int)-1;
    memset(mTextures, 0, sizeof(mTextures));
    memset(mSamplers, 0, sizeof(mSamplers));
}

void LcNoesisRenderDeviceD3D10::SetIndexBuffer(ID3D10Buffer* buffer)
{
    if (mIndexBuffer != buffer)
    {
        mDevice->IASetIndexBuffer(buffer, DXGI_FORMAT_R16_UINT, 0);
        mIndexBuffer = buffer;
    }
}

void LcNoesisRenderDeviceD3D10::SetVertexBuffer(ID3D10Buffer* buffer, uint32_t stride, uint32_t offset)
{
    mDevice->IASetVertexBuffers(0, 1, &buffer, &stride, &offset);
}

void LcNoesisRenderDeviceD3D10::SetInputLayout(ID3D10InputLayout* layout)
{
    if (layout != mLayout)
    {
        mDevice->IASetInputLayout(layout);
        mLayout = layout;
    }
}

void LcNoesisRenderDeviceD3D10::SetVertexShader(ID3D10VertexShader* shader)
{
    if (shader != mVertexShader)
    {
        mDevice->VSSetShader(shader);
        mVertexShader = shader;
    }
}

void LcNoesisRenderDeviceD3D10::SetPixelShader(ID3D10PixelShader* shader)
{
    if (shader != mPixelShader)
    {
        mDevice->PSSetShader(shader);
        mPixelShader = shader;
    }
}

void LcNoesisRenderDeviceD3D10::SetRasterizerState(ID3D10RasterizerState* state)
{
    if (state != mRasterizerState)
    {
        mDevice->RSSetState(state);
        mRasterizerState = state;
    }
}

void LcNoesisRenderDeviceD3D10::SetBlendState(ID3D10BlendState* state)
{
    if (state != mBlendState)
    {
        mDevice->OMSetBlendState(state, 0, 0xffffffff);
        mBlendState = state;
    }
}

void LcNoesisRenderDeviceD3D10::SetDepthStencilState(ID3D10DepthStencilState* state, unsigned int stencilRef)
{
    if (state != mDepthStencilState || stencilRef != mStencilRef)
    {
        mDevice->OMSetDepthStencilState(state, stencilRef);
        mDepthStencilState = state;
        mStencilRef = stencilRef;
    }
}

void LcNoesisRenderDeviceD3D10::SetSampler(unsigned int slot, ID3D10SamplerState* sampler)
{
    NS_ASSERT(slot < NS_COUNTOF(mSamplers));
    if (sampler != mSamplers[slot])
    {
        mDevice->PSSetSamplers(slot, 1, &sampler);
        mSamplers[slot] = sampler;
    }
}

void LcNoesisRenderDeviceD3D10::SetTexture(unsigned int slot, ID3D10ShaderResourceView* texture)
{
    NS_ASSERT(slot < NS_COUNTOF(mTextures));
    if (texture != mTextures[slot])
    {
        mDevice->PSSetShaderResources(slot, 1, &texture);
        mTextures[slot] = texture;
    }
}

void LcNoesisRenderDeviceD3D10::ClearTextures()
{
    memset(mTextures, 0, sizeof(mTextures));
    mDevice->PSSetShaderResources(0, NS_COUNTOF(mTextures), mTextures);
}

void LcNoesisRenderDeviceD3D10::SetBuffers(const Batch& batch, uint32_t stride)
{
    // Indices
    SetIndexBuffer(mIndices.obj);

    // Vertices
    uint32_t offset = mVertices.drawPos + batch.vertexOffset;
    SetVertexBuffer(mVertices.obj, stride, offset);

    // Vertex Shader Constant Buffers
    static_assert(NS_COUNTOF(mVertexCB) == NS_COUNTOF(Batch::vertexUniforms), "");
    static_assert(NS_COUNTOF(mVertexCBHash) == NS_COUNTOF(Batch::vertexUniforms), "");

    for (uint32_t i = 0; i < NS_COUNTOF(mVertexCB); i++)
    {
        if (batch.vertexUniforms[i].numDwords > 0)
        {
            if (mVertexCBHash[i] != batch.vertexUniforms[i].hash)
            {
                uint32_t size = batch.vertexUniforms[i].numDwords * sizeof(uint32_t);
                void* ptr = MapBuffer(mVertexCB[i], size);
                memcpy(ptr, batch.vertexUniforms[i].values, size);
                UnmapBuffer(mVertexCB[i]);

                mVertexCBHash[i] = batch.vertexUniforms[i].hash;
            }
        }
    }

    // Pixel Shader Constant Buffers
    static_assert(NS_COUNTOF(mPixelCB) == NS_COUNTOF(Batch::pixelUniforms), "");
    static_assert(NS_COUNTOF(mPixelCBHash) == NS_COUNTOF(Batch::vertexUniforms), "");

    for (uint32_t i = 0; i < NS_COUNTOF(mPixelCB); i++)
    {
        if (batch.pixelUniforms[i].numDwords > 0)
        {
            if (mPixelCBHash[i] != batch.pixelUniforms[i].hash)
            {
                uint32_t size = batch.pixelUniforms[i].numDwords * sizeof(uint32_t);
                void* ptr = MapBuffer(mPixelCB[i], size);
                memcpy(ptr, batch.pixelUniforms[i].values, size);
                UnmapBuffer(mPixelCB[i]);

                mPixelCBHash[i] = batch.pixelUniforms[i].hash;
            }
        }
    }
}

void LcNoesisRenderDeviceD3D10::SetRenderState(const Batch& batch)
{
    auto f = batch.renderState.f;

    NS_ASSERT(f.wireframe < NS_COUNTOF(mRasterizerStates));
    ID3D10RasterizerState* rasterizer = mRasterizerStates[f.wireframe];
    SetRasterizerState(rasterizer);

    NS_ASSERT(f.blendMode < NS_COUNTOF(mBlendStates));
    ID3D10BlendState* blend = f.colorEnable ? mBlendStates[f.blendMode] : mBlendStateNoColor;
    SetBlendState(blend);

    NS_ASSERT(f.stencilMode < NS_COUNTOF(mDepthStencilStates));
    ID3D10DepthStencilState* stencil = mDepthStencilStates[f.stencilMode];
    SetDepthStencilState(stencil, batch.stencilRef);
}

void LcNoesisRenderDeviceD3D10::SetTextures(const Batch& batch)
{
    if (batch.pattern != 0)
    {
        TextureD3D10* t = (TextureD3D10*)batch.pattern;
        SetTexture(TextureSlot::Pattern, t->view);
        SetSampler(TextureSlot::Pattern, mSamplerStates[batch.patternSampler.v]);
    }

    if (batch.ramps != 0)
    {
        TextureD3D10* t = (TextureD3D10*)batch.ramps;
        SetTexture(TextureSlot::Ramps, t->view);
        SetSampler(TextureSlot::Ramps, mSamplerStates[batch.rampsSampler.v]);
    }

    if (batch.image != 0)
    {
        TextureD3D10* t = (TextureD3D10*)batch.image;
        SetTexture(TextureSlot::Image, t->view);
        SetSampler(TextureSlot::Image, mSamplerStates[batch.imageSampler.v]);
    }

    if (batch.glyphs != 0)
    {
        TextureD3D10* t = (TextureD3D10*)batch.glyphs;
        SetTexture(TextureSlot::Glyphs, t->view);
        SetSampler(TextureSlot::Glyphs, mSamplerStates[batch.glyphsSampler.v]);
    }

    if (batch.shadow != 0)
    {
        TextureD3D10* t = (TextureD3D10*)batch.shadow;
        SetTexture(TextureSlot::Shadow, t->view);
        SetSampler(TextureSlot::Shadow, mSamplerStates[batch.shadowSampler.v]);
    }
}
