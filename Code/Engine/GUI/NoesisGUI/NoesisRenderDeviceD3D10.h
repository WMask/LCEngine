/**
* NoesisRenderDeviceD3D10.h
* 04.02.2023
* (c) Denis Romakhov
*/

#pragma once

#include <NsRender/RenderDevice.h>
#include <NsCore/Vector.h>

#include <d3d10.h>
#include <map>


namespace Noesis { template<class T> class Ptr; }

struct MSAA
{
    enum Enum
    {
        x1,
        x2,
        x4,
        x8,
        x16,

        Count
    };
};


/**
* Noesis render device for DirectX 10 */
class LcNoesisRenderDeviceD3D10 final: public Noesis::RenderDevice
{
public:
    LcNoesisRenderDeviceD3D10(ID3D10Device* device, bool sRGB = false);
    ~LcNoesisRenderDeviceD3D10();
    void LoadShaders(const char* folderPath);

    // Creates a Noesis texture from a D3D10 texture. Reference count is incremented by one
    static Noesis::Ptr<Noesis::Texture> WrapTexture(ID3D10Texture2D* texture, uint32_t width,
        uint32_t height, uint32_t levels, bool isInverted, bool hasAlpha);

    void* CreatePixelShader(const char* label, const void* hlsl, uint32_t size);
    void ClearPixelShaders();


private: // NoesisApp::RenderDevice interface implementation
    const Noesis::DeviceCaps& GetCaps() const override;
    Noesis::Ptr<Noesis::RenderTarget> CreateRenderTarget(const char* label, uint32_t width,
        uint32_t height, uint32_t sampleCount, bool needsStencil) override;
    Noesis::Ptr<Noesis::RenderTarget> CloneRenderTarget(const char* label,
        Noesis::RenderTarget* surface) override;
    Noesis::Ptr<Noesis::Texture> CreateTexture(const char* label, uint32_t width, uint32_t height,
        uint32_t numLevels, Noesis::TextureFormat::Enum format, const void** data) override;
    void UpdateTexture(Noesis::Texture* texture, uint32_t level, uint32_t x, uint32_t y,
        uint32_t width, uint32_t height, const void* data) override;
    void BeginOffscreenRender() override;
    void EndOffscreenRender() override;
    void BeginOnscreenRender() override;
    void EndOnscreenRender() override;
    void SetRenderTarget(Noesis::RenderTarget* surface) override;
    void ResolveRenderTarget(Noesis::RenderTarget* surface, const Noesis::Tile* tiles,
        uint32_t numTiles) override;
    void* MapVertices(uint32_t bytes) override;
    void UnmapVertices() override;
    void* MapIndices(uint32_t bytes) override;
    void UnmapIndices() override;
    void DrawBatch(const Noesis::Batch& batch) override;


private:
    struct DynamicBuffer;

    void CreateBuffer(DynamicBuffer& buffer, uint32_t size, D3D10_BIND_FLAG flags, const char* label);
    void CreateBuffers();
    void DestroyBuffers();
    void* MapBuffer(DynamicBuffer& buffer, uint32_t size);
    void UnmapBuffer(DynamicBuffer& buffer) const;

    ID3D10InputLayout* CreateLayout(uint32_t format, const void* code, uint32_t size);
    void CreateStateObjects();
    void DestroyStateObjects();
    void CreateShaders();
    void DestroyShaders();
    Noesis::Ptr<Noesis::RenderTarget> CreateRenderTarget(const char* label, uint32_t width,
        uint32_t height, MSAA::Enum msaa, ID3D10Texture2D* colorAA, ID3D10Texture2D* stencil);

    void CheckMultisample();
    void FillCaps(bool sRGB);
    void InvalidateStateCache();

    void SetIndexBuffer(ID3D10Buffer* buffer);
    void SetVertexBuffer(ID3D10Buffer* buffer, uint32_t stride, uint32_t offset);
    void SetInputLayout(ID3D10InputLayout* layout);
    void SetVertexShader(ID3D10VertexShader* shader);
    void SetPixelShader(ID3D10PixelShader* shader);
    void SetRasterizerState(ID3D10RasterizerState* state);
    void SetBlendState(ID3D10BlendState* state);
    void SetDepthStencilState(ID3D10DepthStencilState* state, unsigned int stencilRef);
    void SetSampler(unsigned int slot, ID3D10SamplerState* sampler);
    void SetTexture(unsigned int slot, ID3D10ShaderResourceView* texture);
    void ClearTextures();

    void SetBuffers(const Noesis::Batch& batch, uint32_t stride);
    void SetRenderState(const Noesis::Batch& batch);
    void SetTextures(const Noesis::Batch& batch);


private:
    ID3D10Device* mDevice = nullptr;
    ID3D10InputLayout* mLayouts[Noesis::Shader::Vertex::Format::Count];

    std::map<std::string, std::string> mShaderSource;

    DXGI_SAMPLE_DESC mSampleDescs[MSAA::Count];
    Noesis::DeviceCaps mCaps;

    struct VertexShader
    {
        ID3D10VertexShader* shader;
        ID3D10InputLayout* layout;
        uint32_t stride;
    };

    struct PixelShader
    {
        ID3D10PixelShader* shader;
        int8_t vsShader;
    };

    struct TextureSlot
    {
        enum Enum
        {
            Pattern,
            Ramps,
            Image,
            Glyphs,
            Shadow,

            Count
        };
    };

    struct DynamicBuffer
    {
        uint32_t size;
        uint32_t pos;
        uint32_t drawPos;
        D3D10_BIND_FLAG flags;

        ID3D10Buffer* obj;
    };

    DynamicBuffer mVertices;
    DynamicBuffer mIndices;

    DynamicBuffer mVertexCB[2];
    uint32_t mVertexCBHash[2];

    DynamicBuffer mPixelCB[2];
    uint32_t mPixelCBHash[2];

    VertexShader mVertexShaders[Noesis::Shader::Vertex::Count];
    PixelShader mPixelShaders[Noesis::Shader::Count];

    Noesis::Vector<ID3D10PixelShader*> mCustomShaders;
    ID3D10PixelShader* mResolvePS[MSAA::Count - 1];
    ID3D10VertexShader* mQuadVS;

    ID3D10RasterizerState* mRasterizerStates[2];
    ID3D10RasterizerState* mRasterizerStateScissor;
    ID3D10BlendState* mBlendStates[6];
    ID3D10BlendState* mBlendStateNoColor;
    ID3D10DepthStencilState* mDepthStencilStates[5];
    ID3D10SamplerState* mSamplerStates[64];
    ID3D10Buffer* mIndexBuffer;
    ID3D10InputLayout* mLayout;
    ID3D10VertexShader* mVertexShader;
    ID3D10PixelShader* mPixelShader;
    ID3D10RasterizerState* mRasterizerState;
    ID3D10BlendState* mBlendState;
    ID3D10DepthStencilState* mDepthStencilState;
    ID3D10ShaderResourceView* mTextures[TextureSlot::Count];
    ID3D10SamplerState* mSamplers[TextureSlot::Count];
    unsigned int mStencilRef;

};
