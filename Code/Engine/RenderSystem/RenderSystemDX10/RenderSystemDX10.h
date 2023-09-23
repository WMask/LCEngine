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
#include "RenderSystem/RenderSystemDX10/ConstantBuffersDX10.h"
#include "RenderSystem/RenderSystem.h"
#include "RenderSystem/SpriteRender.h"
#include "GUI/Widgets.h"

#pragma warning(disable : 4251)
#pragma warning(disable : 5046)


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
	* Return shaders settings buffer */
	virtual ID3D10Buffer* GetSettingsBuffer() const = 0;
	/**
	* Get sprite renders */
	virtual TVisual2DRenderList& GetVisual2DRenderList() = 0;
	/**
	* Force sprite render setup. Updates shaders and buffers */
	virtual void ForceRenderSetup() = 0;
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
	class LcTiledVisual2DRenderDX10* GetTiledRender() { return tiledRender; }
	//
	class ISpriteRender* GetTextureRender() { return textureRender; }
	//
	class LcWidgetRenderDX10* GetWidgetRender() { return widgetRender; }


public:// IRenderSystem interface implementation
	//
	virtual ~LcRenderSystemDX10() override;
	//
	virtual void Create(void* windowHandle, LcWinMode mode, bool inVSync, const LcAppContext& context) override;
	//
	virtual void Shutdown() override;
	//
	virtual void Subscribe(const LcAppContext& context);
	//
	virtual void Update(float deltaSeconds, const LcAppContext& context) override;
	//
	virtual void UpdateCamera(float deltaSeconds, LcVector3 newPos, LcVector3 newTarget) override;
	//
	virtual void Render(const LcAppContext& context) override;
	//
	virtual void RequestResize(int width, int height) override;
	//
	virtual void Resize(int width, int height, const LcAppContext& context) override;
	//
	virtual void SetMode(LcWinMode mode) override;
	//
	virtual bool CanRender() const override { return d3dDevice; }
	//
	virtual LcRenderSystemType GetType() const override { return LcRenderSystemType::DX10; }


protected:// LcRenderSystemBase interface implementation
	//
	virtual void RenderSprite(const ISprite* sprite, const LcAppContext& context) override;
	//
	virtual void RenderWidget(const IWidget* widget, const LcAppContext& context) override;


public:// IDX10RenderDevice interface implementation
	//
	virtual ID3D10Device1* GetD3D10Device() const override { return d3dDevice.Get(); }
	//
	virtual IDXGISwapChain* GetD3D10SwapChain() const override { return swapChain.Get(); }
	//
	virtual ID3D10Buffer* GetTransformBuffer() const override { return constBuffers.transMatrixBuffer.Get(); }
	//
	virtual ID3D10Buffer* GetColorsBuffer() const override { return constBuffers.colorsBuffer.Get(); }
	//
	virtual ID3D10Buffer* GetCustomUvBuffer() const override { return constBuffers.customUvBuffer.Get(); }
	//
	virtual ID3D10Buffer* GetFrameAnimBuffer() const override { return constBuffers.frameAnimBuffer.Get(); }
	//
	virtual ID3D10Buffer* GetSettingsBuffer() const override { return constBuffers.settingsBuffer.Get(); }
	//
	virtual TVisual2DRenderList& GetVisual2DRenderList() override { return visual2DRenders; }
	//
	virtual void ForceRenderSetup() override { prevSetupRequested = true; }
	//
	virtual std::string GetShaderCode(const std::string& shaderName) const override;


protected:
	ComPtr<ID3D10Device1> d3dDevice;
	//
	ComPtr<IDXGISwapChain> swapChain;
	//
	ComPtr<ID3D10RenderTargetView> renderTargetView;
	//
	ComPtr<ID3D10BlendState> blendState;
	//
	ComPtr<ID3D10RasterizerState> rasterizerState;
	//
	ComPtr<ID3D10DepthStencilView> depthStencilView;
	//
	ComPtr<ID3D10Texture2D> depthStencil;
	//
	std::unique_ptr<class LcTextureLoaderDX10> texLoader;
	//
	TVisual2DRenderList visual2DRenders;
	//
	LcConstantBuffersDX10 constBuffers;
	//
	LcWidgetRenderDX10* widgetRender;
	//
	class LcTiledVisual2DRenderDX10* tiledRender;
	//
	class ISpriteRender* textureRender;
	//
	TVFeaturesList prevSpriteFeatures;
	//
	LcSize renderSystemSize;
	//
	LcVector3 worldScale;
	//
	bool worldScaleFonts;
	//
	bool prevSetupRequested;

};
