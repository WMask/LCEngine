/**
* RenderSystemDX10.h
* 28.01.2023
* (c) Denis Romakhov
*/

#pragma once

#include <deque>
#include <d3d10.h>
#include <wrl.h>

using namespace Microsoft::WRL;

#include "Module.h"
#include "RenderSystem/RenderSystem.h"
#include "RenderSystem/SpriteRender.h"
#include "World/Module.h"


#pragma warning(disable : 4251)

/**
* Render device */
class RENDERSYSTEMDX10_API IRenderDeviceDX10
{
public:
	/**
	* Return D3D10 device */
	virtual ID3D10Device* GetD3D10Device() const = 0;
	/**
	* Return matrix buffer */
	virtual ID3D10Buffer* GetTransformBuffer() const = 0;
	/**
	* Return colors buffer */
	virtual ID3D10Buffer* GetColorsBuffer() const = 0;
	/**
	* Return frame animation buffer */
	virtual ID3D10Buffer* GetFrameAnimBuffer() const = 0;
	/**
	* Get shader code */
	virtual std::string GetShaderCode(const std::string& shaderName) const = 0;

};

/**
* DirectX 10 render system */
class RENDERSYSTEMDX10_API LcRenderSystemDX10
	: public LcRenderSystemBase
	, public IRenderDeviceDX10
{
public:
	LcRenderSystemDX10();
	//
	class LcTextureLoaderDX10* GetTextureLoader() { return texLoader.get(); }


public: // IRenderSystem interface implementation
	/**
	* Virtual destructor */
	virtual ~LcRenderSystemDX10() override;
	/**
	* Create render system */
	virtual void Create(TWeakWorld worldPtr, void* windowHandle, bool windowed) override;
	/**
	* Shutdown render system */
	virtual void Shutdown() override;
	/**
	* Update world */
	virtual void Update(float deltaSeconds) override;
	/**
	* Update camera */
	virtual void UpdateCamera(float deltaSeconds, LcVector3 newPos, LcVector3 newTarget) override;
	/**
	* Render world */
	virtual void Render() override;
	/**
	* Render sprite */
	virtual void RenderSprite(const ISprite* sprite) override;
	/**
	* Render widget */
	virtual void RenderWidget(const IWidget* widget) override;
	/**
	* Return render system state */
	virtual bool CanRender() const override { return d3dDevice; }
	/**
	* Return render system type */
	virtual LcRenderSystemType GetType() const override { return LcRenderSystemType::DX10; }


public: // IDX10RenderDevice interface implementation
	/**
	* Return D3D10 device */
	virtual ID3D10Device* GetD3D10Device() const override { return d3dDevice; }
	/**
	* Return matrix buffer */
	virtual ID3D10Buffer* GetTransformBuffer() const override { return transMatrixBuffer; }
	/**
	* Return colors buffer */
	virtual ID3D10Buffer* GetColorsBuffer() const override { return colorsBuffer; }
	/**
	* Return colors buffer */
	virtual ID3D10Buffer* GetFrameAnimBuffer() const override { return frameAnimBuffer; }
	/**
	* Get shader code */
	virtual std::string GetShaderCode(const std::string& shaderName) const override;


protected:
	ID3D10Device* d3dDevice;
	//
	IDXGISwapChain* swapChain;
	//
	ID3D10RenderTargetView* renderTargetView;
	//
	ID3D10Buffer* projMatrixBuffer;
	//
	ID3D10Buffer* transMatrixBuffer;
	//
	ID3D10Buffer* viewMatrixBuffer;
	//
	ID3D10Buffer* colorsBuffer;
	//
	ID3D10Buffer* frameAnimBuffer;
	//
	ID3D10BlendState* blendState;
	//
	ID3D10RasterizerState* rasterizerState;
	//
	std::unique_ptr<class LcTextureLoaderDX10> texLoader;
	//
	std::deque<std::shared_ptr<ISpriteRender>> spriteRenders;
	//
	TVFeaturesList prevSpriteFeatures;

};


/**
* DirectX10 Sprite implementation */
class LcSpriteDX10 : public LcSprite
{
public:
	LcSpriteDX10(LcSpriteData inSprite, LcRenderSystemDX10& inRender) : LcSprite(inSprite), render(inRender) {}
	//
	~LcSpriteDX10();
	//
	LcRenderSystemDX10& render;
	//
	ID3D10Texture2D* texture;
	//
	ID3D10ShaderResourceView* shaderView;


public: // IVisual interface implementation
	virtual void AddComponent(TVComponentPtr comp) override;

};
