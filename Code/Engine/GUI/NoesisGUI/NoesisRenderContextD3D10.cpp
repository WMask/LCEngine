/**
* NoesisRenderContextD3D10.cpp
* 04.02.2023
* (c) Denis Romakhov
*/

#include "pch.h"
#include "GUI/NoesisGUI/NoesisRenderContextD3D10.h"
#include "GUI/NoesisGUI/NoesisRenderDeviceD3D10.h"
#include "Core/LCUtils.h"

#include <NsCore/ReflectionImplement.h>
#include <NsCore/Category.h>
#include <NsCore/Log.h>
#include <NsCore/UTF8.h>
#include <NsRender/RenderDevice.h>
#include <NsRender/RenderTarget.h>
#include <NsRender/Texture.h>
#include <NsRender/Image.h>
#include <NsDrawing/Color.h>


using namespace Noesis;
using namespace NoesisApp;


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
        o = 0; \
    }

#define DX_NAME(...) NS_UNUSED(__VA_ARGS__)

#define DXGI_SWAP_EFFECT_FLIP_DISCARD_ DXGI_SWAP_EFFECT(4)


LcNoesisRenderContextD3D10::LcNoesisRenderContextD3D10()
{
}

LcNoesisRenderContextD3D10::~LcNoesisRenderContextD3D10()
{
}

const char* LcNoesisRenderContextD3D10::Description() const
{
    return "D3D10";
}

uint32_t LcNoesisRenderContextD3D10::Score() const
{
    return 200;
}

bool LcNoesisRenderContextD3D10::Validate() const
{
    return true;
}

void LcNoesisRenderContextD3D10::Init(void* window, uint32_t& samples, bool vsync, bool sRGB)
{
    if (FAILED(D3D10CreateDevice(0, D3D10_DRIVER_TYPE_HARDWARE, 0, D3D10_CREATE_DEVICE_SINGLETHREADED, D3D10_SDK_VERSION, &mDevice)))
    {
        throw std::exception("LcNoesisRenderContextD3D10::Init(): Cannot create device");
    }

    CreateSwapChain(window, samples, sRGB);
    CreateQueries();

    mVSync = vsync;
    mRenderer = *new LcNoesisRenderDeviceD3D10(mDevice, sRGB);
}

void LcNoesisRenderContextD3D10::Shutdown()
{
    if (mRenderer) mRenderer.Reset();

    for (uint32_t i = 0; i < NS_COUNTOF(mFrames); i++)
    {
        DX_DESTROY(mFrames[i].begin);
        DX_DESTROY(mFrames[i].end);
        DX_DESTROY(mFrames[i].disjoint);
    }

    DX_DESTROY(mBackBuffer);
    DX_DESTROY(mBackBufferAA);
    DX_DESTROY(mRenderTarget);
    DX_DESTROY(mDepthStencil);
    DX_DESTROY(mSwapChain);
}

RenderDevice* LcNoesisRenderContextD3D10::GetDevice() const
{
    return mRenderer;
}

void LcNoesisRenderContextD3D10::BeginRender()
{
}

void LcNoesisRenderContextD3D10::EndRender()
{
}

void LcNoesisRenderContextD3D10::Resize()
{
    if (mDevice) mDevice->OMSetRenderTargets(0, nullptr, nullptr);

    DX_DESTROY(mDepthStencil);
    DX_DESTROY(mBackBuffer);
    DX_DESTROY(mBackBufferAA);
    DX_DESTROY(mRenderTarget);

    NS_ASSERT(mSwapChain != 0);
    if (FAILED(mSwapChain->ResizeBuffers(0, 0, 0, DXGI_FORMAT_UNKNOWN, 0)))
    {
        throw std::exception("LcNoesisRenderContextD3D10::Resize(): Cannot resize buffers");
    }

    CreateBuffers();
}

float LcNoesisRenderContextD3D10::GetGPUTimeMs() const
{
    return mGPUTime;
}

void LcNoesisRenderContextD3D10::SetClearColor(float r, float g, float b, float a)
{
    bool linear = mRenderer->GetCaps().linearRendering;
    mClearColor[0] = linear ? SRGBToLinear(r) : r;
    mClearColor[1] = linear ? SRGBToLinear(g) : g;
    mClearColor[2] = linear ? SRGBToLinear(b) : b;
    mClearColor[3] = a;
}

void LcNoesisRenderContextD3D10::SetDefaultRenderTarget(uint32_t, uint32_t, bool doClearColor)
{
    if (doClearColor)
    {
        mDevice->ClearRenderTargetView(mRenderTarget, mClearColor);
    }

    mDevice->ClearDepthStencilView(mDepthStencil, D3D10_CLEAR_STENCIL, 0.0f, 0);
    mDevice->OMSetRenderTargets(1, &mRenderTarget, mDepthStencil);
    mDevice->RSSetViewports(1, &mViewport);
}

Ptr<NoesisApp::Image> LcNoesisRenderContextD3D10::CaptureRenderTarget(RenderTarget* surface) const
{
    struct D3D10Texture: public Texture
    {
        ID3D10ShaderResourceView* view;
    };

    ID3D10Texture2D* source;
    ((D3D10Texture*)surface->GetTexture())->view->GetResource((ID3D10Resource**)&source);

    D3D10_TEXTURE2D_DESC desc;
    source->GetDesc(&desc);

    desc.CPUAccessFlags = D3D10_CPU_ACCESS_READ;
    desc.Usage = D3D10_USAGE_STAGING;
    desc.BindFlags = 0;

    ID3D10Texture2D* dest;
    if (FAILED(mDevice->CreateTexture2D(&desc, 0, &dest)))
    {
        throw std::exception("LcNoesisRenderContextD3D10::CaptureRenderTarget(): Cannot create texture");
    }

    mDevice->CopyResource(dest, source);

    D3D10_MAPPED_TEXTURE2D mapped;
    if (FAILED(dest->Map(0, D3D10_MAP_READ, 0, &mapped)))
    {
        throw std::exception("LcNoesisRenderContextD3D10::CaptureRenderTarget(): Cannot map texture");
    }

    Ptr<Image> image = *new Image(desc.Width, desc.Height);
    const uint8_t* src = (const uint8_t*)(mapped.pData);
    uint8_t* dst = (uint8_t*)(image->Data());

    for (uint32_t i = 0; i < desc.Height; i++)
    {
        for (uint32_t j = 0; j < desc.Width; j++)
        {
            // RGBA -> BGRA
            dst[4 * j + 0] = src[4 * j + 2];
            dst[4 * j + 1] = src[4 * j + 1];
            dst[4 * j + 2] = src[4 * j + 0];
            dst[4 * j + 3] = src[4 * j + 3];
        }

        src += mapped.RowPitch;
        dst += image->Stride();
    }

    dest->Unmap(0);

    DX_RELEASE(source);
    DX_DESTROY(dest);

    return image;
}

void LcNoesisRenderContextD3D10::Swap()
{
    if (mBackBufferAA != 0)
    {
        mDevice->ResolveSubresource(mBackBuffer, 0, mBackBufferAA, 0, mRenderTargetFormat);
    }

    if (FAILED(mSwapChain->Present(mVSync ? 1 : 0, 0)))
    {
        throw std::exception("LcNoesisRenderContextD3D10::Swap(): Cannot present");
    }
}

void* LcNoesisRenderContextD3D10::CreatePixelShader(const ShaderSource& source)
{
    if (!source.hlsl.Empty())
    {
        LcNoesisRenderDeviceD3D10* device = (LcNoesisRenderDeviceD3D10*)mRenderer.GetPtr();
        return device->CreatePixelShader(source.label, source.hlsl.Data(), source.hlsl.Size());
    }

    return nullptr;
}

DXGI_SAMPLE_DESC LcNoesisRenderContextD3D10::GetSampleDesc(uint32_t samples) const
{
    NS_ASSERT(mDevice != 0);

    DXGI_SAMPLE_DESC descs[16];
    samples = Max(1U, Min(samples, 16U));

    descs[0].Count = 1;
    descs[0].Quality = 0;

    for (uint32_t i = 1, last = 0; i < samples; i++)
    {
        unsigned int count = i + 1;
        unsigned int quality = 0;
        DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM;
        HRESULT hr = mDevice->CheckMultisampleQualityLevels(format, count, &quality);

        if (SUCCEEDED(hr) && quality > 0)
        {
            descs[i].Count = count;
            descs[i].Quality = 0;
            last = i;
        }
        else
        {
            descs[i] = descs[last];
        }
    }

    return descs[samples - 1];
}

void LcNoesisRenderContextD3D10::CreateSwapChain(void* window, uint32_t& samples, bool sRGB)
{
    IDXGIDevice* dxgiDevice;
    if (FAILED(mDevice->QueryInterface(__uuidof(IDXGIDevice), (void**)&dxgiDevice)))
    {
        throw std::exception("LcNoesisRenderContextD3D10::CreateSwapChain(): Cannot get device");
    }

    IDXGIAdapter* dxgiAdapter;
    if (FAILED(dxgiDevice->GetAdapter(&dxgiAdapter)))
    {
        throw std::exception("LcNoesisRenderContextD3D10::CreateSwapChain(): Cannot get adapter");
    }

    IDXGIFactory* factory;
    if (FAILED(dxgiAdapter->GetParent(__uuidof(IDXGIFactory), (void**)&factory)))
    {
        throw std::exception("LcNoesisRenderContextD3D10::CreateSwapChain(): Cannot get factory");
    }

    mHwnd = (HWND)window;
    mRenderTargetFormat = sRGB ? DXGI_FORMAT_R8G8B8A8_UNORM_SRGB : DXGI_FORMAT_R8G8B8A8_UNORM;
    mSampleDesc = GetSampleDesc(samples);
    samples = mSampleDesc.Count;

    DXGI_SWAP_CHAIN_DESC desc;
    desc.BufferDesc.Width = 0;
    desc.BufferDesc.Height = 0;
    desc.BufferDesc.RefreshRate.Numerator = 0;
    desc.BufferDesc.RefreshRate.Denominator = 0;
    desc.BufferDesc.Format = mRenderTargetFormat;
    desc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
    desc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
    desc.SampleDesc.Count = 1;
    desc.SampleDesc.Quality = 0;
    desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    desc.BufferCount = 2;
    desc.OutputWindow = mHwnd;
    desc.Windowed = true;
    desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD_;
    desc.Flags = 0;

    if (FAILED(factory->CreateSwapChain(mDevice, &desc, &mSwapChain)))
    {
        desc.BufferCount = 1;
        desc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
        if (FAILED(factory->CreateSwapChain(mDevice, &desc, &mSwapChain)))
        {
            throw std::exception("LcNoesisRenderContextD3D10::CreateSwapChain(): Cannot create swap chain");
        }
    }

    DX_RELEASE(factory);
    DX_RELEASE(dxgiDevice);
    DX_RELEASE(dxgiAdapter);
}

void LcNoesisRenderContextD3D10::CreateBuffers()
{
    NS_ASSERT(mSwapChain != 0);

    DXGI_SWAP_CHAIN_DESC desc;
    if (FAILED(mSwapChain->GetDesc(&desc)))
    {
        throw std::exception("LcNoesisRenderContextD3D10::CreateBuffers(): Cannot get swap chain desc");
    }

    if (mSampleDesc.Count != 1)
    {
        D3D10_TEXTURE2D_DESC colorDesc;
        colorDesc.Width = desc.BufferDesc.Width;
        colorDesc.Height = desc.BufferDesc.Height;
        colorDesc.MipLevels = 1;
        colorDesc.ArraySize = 1;
        colorDesc.Format = mRenderTargetFormat;
        colorDesc.SampleDesc = mSampleDesc;
        colorDesc.Usage = D3D10_USAGE_DEFAULT;
        colorDesc.BindFlags = D3D10_BIND_RENDER_TARGET;
        colorDesc.CPUAccessFlags = 0;
        colorDesc.MiscFlags = 0;

        if (FAILED(mDevice->CreateTexture2D(&colorDesc, 0, &mBackBufferAA)))
        {
            throw std::exception("LcNoesisRenderContextD3D10::CreateBuffers(): Cannot create back buffer");
        }

        if (FAILED(mDevice->CreateRenderTargetView(mBackBufferAA, 0, &mRenderTarget)))
        {
            throw std::exception("LcNoesisRenderContextD3D10::CreateBuffers(): Cannot create render target");
        }

        if (FAILED(mSwapChain->GetBuffer(0, __uuidof(ID3D10Texture2D), (void**)&mBackBuffer)))
        {
            throw std::exception("LcNoesisRenderContextD3D10::CreateBuffers(): Cannot get back buffer");
        }
    }
    else
    {
        ID3D10Texture2D* color;
        if (FAILED(mSwapChain->GetBuffer(0, __uuidof(ID3D10Texture2D), (void**)&color)))
        {
            throw std::exception("LcNoesisRenderContextD3D10::CreateBuffers(): Cannot get swap chain color");
        }

        D3D10_RENDER_TARGET_VIEW_DESC viewDesc;
        viewDesc.ViewDimension = D3D10_RTV_DIMENSION_TEXTURE2D;
        viewDesc.Texture2D.MipSlice = 0;
        viewDesc.Format = mRenderTargetFormat;
        if (FAILED(mDevice->CreateRenderTargetView(color, &viewDesc, &mRenderTarget)))
        {
            throw std::exception("LcNoesisRenderContextD3D10::CreateBuffers(): Cannot create render target");
        }

        DX_RELEASE(color);
    }

    D3D10_TEXTURE2D_DESC stencilDesc;
    stencilDesc.Width = desc.BufferDesc.Width;
    stencilDesc.Height = desc.BufferDesc.Height;
    stencilDesc.MipLevels = 1;
    stencilDesc.ArraySize = 1;
    stencilDesc.SampleDesc = mSampleDesc;
    stencilDesc.Usage = D3D10_USAGE_DEFAULT;
    stencilDesc.BindFlags = D3D10_BIND_DEPTH_STENCIL;
    stencilDesc.CPUAccessFlags = 0;
    stencilDesc.MiscFlags = 0;
    stencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;

    ID3D10Texture2D* depthStencil;
    if (FAILED(mDevice->CreateTexture2D(&stencilDesc, 0, &depthStencil)))
    {
        throw std::exception("LcNoesisRenderContextD3D10::CreateBuffers(): Cannot get depth stencil");
    }

    if (FAILED(mDevice->CreateDepthStencilView(depthStencil, 0, &mDepthStencil)))
    {
        throw std::exception("LcNoesisRenderContextD3D10::CreateBuffers(): Cannot create depth stencil view");
    }

    DX_RELEASE(depthStencil);

    mViewport.TopLeftX = 0;
    mViewport.TopLeftY = 0;
    mViewport.Width = desc.BufferDesc.Width;
    mViewport.Height = desc.BufferDesc.Height;
    mViewport.MinDepth = 0.0f;
    mViewport.MaxDepth = 1.0f;
}

void LcNoesisRenderContextD3D10::CreateQueries()
{
    for (uint32_t i = 0; i < NS_COUNTOF(mFrames); i++)
    {
        D3D10_QUERY_DESC desc;
        desc.MiscFlags = 0;

        desc.Query = D3D10_QUERY_TIMESTAMP;
        if (FAILED(mDevice->CreateQuery(&desc, &mFrames[i].begin)))
        {
            throw std::exception("LcNoesisRenderContextD3D10::CreateQueries(): Cannot create query");
        }

        if (FAILED(mDevice->CreateQuery(&desc, &mFrames[i].end)))
        {
            throw std::exception("LcNoesisRenderContextD3D10::CreateQueries(): Cannot create query");
        }

        desc.Query = D3D10_QUERY_TIMESTAMP_DISJOINT;
        if (FAILED(mDevice->CreateQuery(&desc, &mFrames[i].disjoint)))
        {
            throw std::exception("LcNoesisRenderContextD3D10::CreateQueries(): Cannot create query");
        }
    }
}

NS_BEGIN_COLD_REGION

NS_IMPLEMENT_REFLECTION(LcNoesisRenderContextD3D10)
{
    NsMeta<Category>("RenderContext");
}

NS_END_COLD_REGION

LcNoesisRenderContextD3D10* GetContext()
{
    static LcNoesisRenderContextD3D10 context;
    return &context;
}
