/**
* RenderSystemDX10.h
* 28.01.2023
* (c) Denis Romakhov
*/

#pragma once

#include <deque>
#include <d3d10_1.h>
#include <wrl.h>

using namespace Microsoft::WRL;

#include "Module.h"
#include "RenderSystem/RenderSystemDX10/UtilsDX10.h"
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
	virtual ID3D10Device1* GetD3D10Device() const = 0;
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
	* Return render system state */
	virtual bool CanRender() const override { return d3dDevice; }
	/**
	* Get widget render */
	virtual IWidgetRender* GetWidgetRender() override { return widgetRender.get(); }
	/**
	* Return render system type */
	virtual LcRenderSystemType GetType() const override { return LcRenderSystemType::DX10; }


public: // IDX10RenderDevice interface implementation
	/**
	* Return D3D10 device */
	virtual ID3D10Device1* GetD3D10Device() const override { return d3dDevice.Get(); }
	/**
	* Return matrix buffer */
	virtual ID3D10Buffer* GetTransformBuffer() const override { return transMatrixBuffer.Get(); }
	/**
	* Return colors buffer */
	virtual ID3D10Buffer* GetColorsBuffer() const override { return colorsBuffer.Get(); }
	/**
	* Return colors buffer */
	virtual ID3D10Buffer* GetFrameAnimBuffer() const override { return frameAnimBuffer.Get(); }
	/**
	* Get shader code */
	virtual std::string GetShaderCode(const std::string& shaderName) const override;


protected:
	ComPtr<ID3D10Device1> d3dDevice;
	//
	ComPtr<IDXGISwapChain> swapChain;
	//
	ComPtr<ID3D10RenderTargetView> renderTargetView;
	//
	ComPtr<ID3D10Buffer> projMatrixBuffer;
	//
	ComPtr<ID3D10Buffer> transMatrixBuffer;
	//
	ComPtr<ID3D10Buffer> viewMatrixBuffer;
	//
	ComPtr<ID3D10Buffer> colorsBuffer;
	//
	ComPtr<ID3D10Buffer> frameAnimBuffer;
	//
	ComPtr<ID3D10BlendState> blendState;
	//
	ComPtr<ID3D10RasterizerState> rasterizerState;
	//
	std::unique_ptr<class LcTextureLoaderDX10> texLoader;
	//
	std::unique_ptr<class LcWidgetRenderDX10> widgetRender;
	//
	std::deque<std::shared_ptr<ISpriteRender>> spriteRenders;
	//
	TSFeaturesList prevSpriteFeatures;

};


/**
* DirectX10 Sprite implementation */
class LcSpriteDX10 : public LcSprite
{
public:
	LcSpriteDX10(LcSpriteData inSprite, LcRenderSystemDX10& inRender) : LcSprite(inSprite), render(inRender) {}
	//
	LcRenderSystemDX10& render;
	//
	ComPtr<ID3D10Texture2D> texture;
	//
	ComPtr<ID3D10ShaderResourceView1> shaderView;


public: // IVisual interface implementation
	virtual void AddComponent(TSComponentPtr comp) override;

};
