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
#include "RenderSystem/RenderSystemDX10/ConstantBuffersDX10.h"
#include "RenderSystem/RenderSystem.h"
#include "GUI/WidgetInterface.h"

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
	* Get sprite renders */
	virtual TVisual2DRenderList& GetVisual2DRenderList() = 0;
	/**
	* Return constant buffers */
	virtual const LcConstantBuffersDX10& GetBuffers() const = 0;
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
	class LcTextRenderDX10* GetTextRender() { return textRender.get(); }
	//
	class LcTiledVisual2DRenderDX10* GetTiledRender() { return tiledRender; }
	//
	class IVisual2DRender* GetTextureRender() { return textureRender; }


public:// IRenderSystem interface implementation
	//
	virtual ~LcRenderSystemDX10() override;
	//
	virtual void Create(void* windowHandle, LcWinMode mode, bool inVSync, bool inAllowFullscreen, const LcAppContext& context) override;
	//
	virtual void Shutdown() override;
	//
	virtual void Clear(IWorld* world, bool removeRooted = false) override;
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
	virtual LcRSStats GetStats() const override;
	//
	virtual LcRenderSystemType GetType() const override { return LcRenderSystemType::DX10; }


protected:// LcRenderSystemBase interface implementation
	//
	virtual void Render(const IVisual* visual, const LcAppContext& context) override;


public:// IDX10RenderDevice interface implementation
	//
	virtual ID3D10Device1* GetD3D10Device() const override { return d3dDevice.Get(); }
	//
	virtual IDXGISwapChain* GetD3D10SwapChain() const override { return swapChain.Get(); }
	//
	virtual TVisual2DRenderList& GetVisual2DRenderList() override { return visual2DRenders; }
	//
	virtual const LcConstantBuffersDX10& GetBuffers() const override { return constBuffers; }
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
	std::unique_ptr<class LcTextureLoaderDX10> texLoader;
	//
	std::unique_ptr<class LcTextRenderDX10> textRender;
	//
	TVisual2DRenderList visual2DRenders;
	//
	LcConstantBuffersDX10 constBuffers;
	//
	class LcTiledVisual2DRenderDX10* tiledRender;
	//
	class IVisual2DRender* textureRender;
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
