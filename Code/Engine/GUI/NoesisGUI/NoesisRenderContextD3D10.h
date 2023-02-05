/**
* NoesisRenderContextD3D10.h
* 04.02.2023
* (c) Denis Romakhov
*/

#pragma once

#include <NsCore/Noesis.h>
#include <NsCore/Ptr.h>
#include <NsCore/Vector.h>
#include <NsRender/RenderContext.h>
#include <NsRender/RenderDevice.h>

#include <d3d10.h>


/**
* Custom render context */
class LcRenderContext : public NoesisApp::RenderContext
{
public:
    /**
    * Set shaders path */
    virtual void SetShadersPath(const char* shadersPath) = 0;

};


/**
* Noesis render context for DirectX 10 */
class LcNoesisRenderContextD3D10 final: public LcRenderContext
{
public:
    LcNoesisRenderContextD3D10();
    ~LcNoesisRenderContextD3D10();


public: // LcRenderContext interface implementation
    /**
    * Set shaders path */
    virtual void SetShadersPath(const char* shadersPath) override { mShadersPath = shadersPath; }


public: // NoesisApp::RenderContext interface implementation
    const char* Description() const override;
    uint32_t Score() const override;
    bool Validate() const override;
    void Init(void* window, uint32_t& samples, bool vsync, bool sRGB) override;
    void Shutdown() override;
    Noesis::RenderDevice* GetDevice() const override;
    void BeginRender() override;
    void EndRender() override;
    void Resize() override;
    float GetGPUTimeMs() const override;
    void SetClearColor(float r, float g, float b, float a) override;
    void SetDefaultRenderTarget(uint32_t width, uint32_t height, bool doClearColor) override;
    Noesis::Ptr<NoesisApp::Image> CaptureRenderTarget(Noesis::RenderTarget* surface) const override;
    void Swap() override;
    void* CreatePixelShader(const NoesisApp::ShaderSource& source) override;


private:
    DXGI_SAMPLE_DESC GetSampleDesc(uint32_t samples) const;
    void CreateSwapChain(void* window, uint32_t& samples, bool sRGB);
    void CreateBuffers();
    void CreateQueries();


private:
    Noesis::Ptr<Noesis::RenderDevice> mRenderer;
    IDXGISwapChain* mSwapChain = nullptr;
    HWND mHwnd;

    ID3D10Device* mDevice = nullptr;
    ID3D10DepthStencilView* mDepthStencil = nullptr;
    ID3D10Texture2D* mBackBuffer = nullptr;
    ID3D10Texture2D* mBackBufferAA = nullptr;
    ID3D10RenderTargetView* mRenderTarget = nullptr;
    DXGI_FORMAT mRenderTargetFormat;

    bool mVSync;
    float mClearColor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
    D3D10_VIEWPORT mViewport;
    DXGI_SAMPLE_DESC mSampleDesc;

    struct Frame
    {
        ID3D10Query* begin;
        ID3D10Query* end;
        ID3D10Query* disjoint;
    };

    Frame mFrames[6] = {};
    uint32_t mReadFrame = 0;
    uint32_t mWriteFrame = 0;
    float mGPUTime = 0.0f;
    std::string mShadersPath;

    NS_DECLARE_REFLECTION(LcNoesisRenderContextD3D10, RenderContext)

};

/**
* Get DirectX 10 context */
LcNoesisRenderContextD3D10* GetContext();
