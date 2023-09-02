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
#include "RenderSystem/RenderSystemDX10/WidgetRenderDX10.h"
#include "RenderSystem/RenderSystem.h"
#include "RenderSystem/SpriteRender.h"
#include "GUI/Widgets.h"

#pragma warning(disable : 4251)


typedef std::deque<std::shared_ptr<IVisual2DRender>> TVisual2DRenderList;

/**
* Render device */
class RENDERSYSTEMDX10_API IRenderDeviceDX10
{
public:
	/**
	* Return D3D10 device */
	virtual ID3D10Device1* GetD3D10Device() const = 0;
	/**
	* Return D3D10 swap chain */
	virtual IDXGISwapChain* GetD3D10SwapChain() const = 0;
	/**
	* Return matrix buffer */
	virtual ID3D10Buffer* GetTransformBuffer() const = 0;
	/**
	* Return colors buffer */
	virtual ID3D10Buffer* GetColorsBuffer() const = 0;
	/**
	* Return custom UV buffer */
	virtual ID3D10Buffer* GetCustomUvBuffer() const = 0;
	/**
	* Return frame animation buffer */
	virtual ID3D10Buffer* GetFrameAnimBuffer() const = 0;
	/**
	* Get sprite renders */
	virtual TVisual2DRenderList& GetVisual2DRenderList() = 0;
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
	//
	class LcWidgetRenderDX10* GetWidgetRender() { return widgetRender; }


public:// IRenderSystem interface implementation
	//
	virtual ~LcRenderSystemDX10() override;
	//
	virtual void Create(TWeakWorld worldPtr, void* windowHandle, bool windowed) override;
	//
	virtual void Shutdown() override;
	//
	virtual void Update(float deltaSeconds) override;
	//
	virtual void UpdateCamera(float deltaSeconds, LcVector3 newPos, LcVector3 newTarget) override;
	//
	virtual void Render() override;
	//
	virtual bool CanRender() const override { return d3dDevice; }
	//
	virtual LcRenderSystemType GetType() const override { return LcRenderSystemType::DX10; }


protected:// LcRenderSystemBase interface implementation
	//
	virtual void RenderSprite(const ISprite* sprite) override;
	//
	virtual void RenderWidget(const IWidget* widget) override;
	//
	virtual void PreRenderWidgets(EWRMode mode) override;
	//
	virtual void PostRenderWidgets(EWRMode mode) override;


public:// IDX10RenderDevice interface implementation
	/**
	* Return D3D10 device */
	virtual ID3D10Device1* GetD3D10Device() const override { return d3dDevice.Get(); }
	/**
	* Return D3D10 swap chain */
	virtual IDXGISwapChain* GetD3D10SwapChain() const override { return swapChain.Get(); }
	/**
	* Return matrix buffer */
	virtual ID3D10Buffer* GetTransformBuffer() const override { return transMatrixBuffer.Get(); }
	/**
	* Return colors buffer */
	virtual ID3D10Buffer* GetColorsBuffer() const override { return colorsBuffer.Get(); }
	/**
	* Return custom UV buffer */
	virtual ID3D10Buffer* GetCustomUvBuffer() const override { return customUvBuffer.Get(); }
	/**
	* Return colors buffer */
	virtual ID3D10Buffer* GetFrameAnimBuffer() const override { return frameAnimBuffer.Get(); }
	/**
	* Get sprite renders */
	virtual TVisual2DRenderList& GetVisual2DRenderList() override { return visual2DRenders; }
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
	ComPtr<ID3D10Buffer> customUvBuffer;
	//
	ComPtr<ID3D10Buffer> frameAnimBuffer;
	//
	ComPtr<ID3D10BlendState> blendState;
	//
	ComPtr<ID3D10RasterizerState> rasterizerState;
	//
	std::unique_ptr<class LcTextureLoaderDX10> texLoader;
	//
	TVisual2DRenderList visual2DRenders;
	//
	LcWidgetRenderDX10* widgetRender;
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
	LcRenderSystemDX10& render;
	//
	ComPtr<ID3D10Texture2D> texture;
	//
	ComPtr<ID3D10ShaderResourceView1> shaderView;


public:// IVisual interface implementation
	virtual void AddComponent(TVComponentPtr comp) override;

};


/**
* DirectX10 Widget implementation */
class LcWidgetDX10 : public LcWidget
{
public:
	LcWidgetDX10(LcWidgetData inSprite, LcRenderSystemDX10& inRender) : LcWidget(inSprite), render(inRender), font(nullptr) {}
	//
	LcRenderSystemDX10& render;
	//
	ComPtr<ID3D10Texture2D> texture;
	//
	ComPtr<ID3D10ShaderResourceView1> shaderView;
	//
	const ITextFont* font;


public:// IVisual interface implementation
	virtual void AddComponent(TVComponentPtr comp) override;

};
