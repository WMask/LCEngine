/**
* RenderSystemDX10.cpp
* 28.01.2023
* (c) Denis Romakhov
*/

#include "pch.h"
#include "RenderSystem/RenderSystemDX10/RenderSystemDX10.h"
#include "RenderSystem/RenderSystemDX10/ColoredSpriteRenderDX10.h"
#include "RenderSystem/RenderSystemDX10/AnimatedSpriteRenderDX10.h"
#include "RenderSystem/RenderSystemDX10/TexturedVisual2DRenderDX10.h"
#include "RenderSystem/RenderSystemDX10/TiledVisual2DRenderDX10.h"
#include "RenderSystem/RenderSystemDX10/BasicParticlesRenderDX10.h"
#include "RenderSystem/RenderSystemDX10/TextRenderDX10.h"
#include "RenderSystem/RenderSystemDX10/VisualsDX10.h"
#include "Application/ApplicationInterface.h"
#include "World/SpriteInterface.h"
#include "World/World.h"
#include "World/Camera.h"
#include "GUI/GuiManager.h"
#include "Core/LCException.h"


class LcVisual2DLifetimeStrategyDX10 : public LcLifetimeStrategy<IVisual, IWorld::TVisualSet>
{
public:
	LcVisual2DLifetimeStrategyDX10() {}
	//
	virtual ~LcVisual2DLifetimeStrategyDX10() {}
	//
	virtual std::shared_ptr<IVisual> Create(const void* userData) override
	{
		auto layerPtr = static_cast<const float*>(userData);
		std::shared_ptr<IVisual> newVisual;

		switch (curTypeId)
		{
		case LcCreatables::Sprite: newVisual = std::make_shared<LcSpriteDX10>(); break;
		case LcCreatables::Widget: newVisual = std::make_shared<LcWidgetDX10>(); break;
		}

		// add layer Z for initial valid sorting in multiset
		newVisual->SetPos(LcVector3(0.0f, 0.0f, *layerPtr));

		return newVisual;
	}
	//
	virtual void Destroy(IVisual& item, IWorld::TVisualSet& items) override {}
};


LcRenderSystemDX10::LcRenderSystemDX10()
	: tiledRender(nullptr)
	, textureRender(nullptr)
	, renderSystemSize(0, 0)
	, worldScale(1.0f, 1.0f, 1.0f)
	, worldScaleFonts(false)
	, prevSetupRequested(false)
{
}

LcRenderSystemDX10::~LcRenderSystemDX10()
{
	Shutdown();
}

void LcRenderSystemDX10::Create(void* windowHandle, LcWinMode winMode, bool inVSync, bool inAllowFullscreen, const LcAppContext& context)
{
	LC_TRY

	HWND hWnd = (HWND)windowHandle;
	RECT clientRect;
	GetClientRect(hWnd, &clientRect);

	int width = clientRect.right - clientRect.left;
	int height = clientRect.bottom - clientRect.top;

	DXGI_MODE_DESC displayModeDesc{};
	if (!LcFindDisplayMode(width, height, &displayModeDesc))
	{
		throw std::exception("LcRenderSystemDX10::Create(): Cannot find display mode");
	}

	DXGI_SWAP_CHAIN_DESC swapChainDesc{};
	swapChainDesc.BufferCount = 2;
	swapChainDesc.BufferDesc = displayModeDesc;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.OutputWindow = hWnd;
	swapChainDesc.Windowed = (winMode == LcWinMode::Windowed) ? TRUE : FALSE;
	swapChainDesc.SwapEffect = inVSync ? DXGI_SWAP_EFFECT_SEQUENTIAL : DXGI_SWAP_EFFECT_DISCARD;
	swapChainDesc.Flags = inAllowFullscreen ? DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH : 0;

	// create the D3D device
	if (FAILED(D3D10CreateDeviceAndSwapChain1(NULL,
		D3D10_DRIVER_TYPE_HARDWARE, NULL, D3D10_CREATE_DEVICE_BGRA_SUPPORT,
		D3D10_FEATURE_LEVEL_10_1, D3D10_1_SDK_VERSION, &swapChainDesc, swapChain.GetAddressOf(), d3dDevice.GetAddressOf())))
	{
		throw std::exception("LcRenderSystemDX10::Create(): Cannot create D3D device");
	}

	// get back buffer
	ComPtr<ID3D10Texture2D> backBuffer;
	if (FAILED(swapChain->GetBuffer(0, __uuidof(ID3D10Texture2D), (LPVOID*)backBuffer.GetAddressOf())))
	{
		throw std::exception("LcRenderSystemDX10::Create(): Cannot create back buffer");
	}

	// create render target
	if (FAILED(d3dDevice->CreateRenderTargetView(backBuffer.Get(), NULL, renderTargetView.GetAddressOf())))
	{
		throw std::exception("LcRenderSystemDX10::Create(): Cannot create render target");
	}

	backBuffer.Reset();

	// set render targets
	d3dDevice->OMSetRenderTargets(1, renderTargetView.GetAddressOf(), nullptr);

	// set the viewport
	D3D10_VIEWPORT viewPort;
	viewPort.Width = width;
	viewPort.Height = height;
	viewPort.MinDepth = 0.0f;
	viewPort.MaxDepth = 1.0f;
	viewPort.TopLeftX = 0;
	viewPort.TopLeftY = 0;

	d3dDevice->RSSetViewports(1, &viewPort);

	// init constant buffers
	cameraPos = LcVector3(width / 2.0f, height / 2.0f, 0.0f);
	cameraTarget = LcVector3(cameraPos.x, cameraPos.y, 1.0f);
	constBuffers.Init(d3dDevice.Get(), cameraPos, cameraTarget, width, height);

	// create blend state
	D3D10_BLEND_DESC blendStateDesc;
	ZeroMemory(&blendStateDesc, sizeof(D3D10_BLEND_DESC));
	blendStateDesc.BlendEnable[0] = TRUE;
	blendStateDesc.SrcBlend = D3D10_BLEND_SRC_ALPHA;
	blendStateDesc.DestBlend = D3D10_BLEND_INV_SRC_ALPHA;
	blendStateDesc.BlendOp = D3D10_BLEND_OP_ADD;
	blendStateDesc.SrcBlendAlpha = D3D10_BLEND_ZERO;
	blendStateDesc.DestBlendAlpha = D3D10_BLEND_ZERO;
	blendStateDesc.BlendOpAlpha = D3D10_BLEND_OP_ADD;
	blendStateDesc.RenderTargetWriteMask[0] = D3D10_COLOR_WRITE_ENABLE_ALL;

	d3dDevice->CreateBlendState(&blendStateDesc, &blendState);
	float blendFactor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
	d3dDevice->OMSetBlendState(blendState.Get(), blendFactor, 0xffffffff);

	// set up rasterizer
	D3D10_RASTERIZER_DESC rasterizerDesc;
	rasterizerDesc.CullMode = D3D10_CULL_NONE;
	rasterizerDesc.FillMode = D3D10_FILL_SOLID;
	rasterizerDesc.FrontCounterClockwise = true;
	rasterizerDesc.DepthBias = false;
	rasterizerDesc.DepthBiasClamp = 0;
	rasterizerDesc.SlopeScaledDepthBias = 0;
	rasterizerDesc.DepthClipEnable = true;
	rasterizerDesc.ScissorEnable = false;
	rasterizerDesc.MultisampleEnable = false;
	rasterizerDesc.AntialiasedLineEnable = true;

	if (FAILED(d3dDevice->CreateRasterizerState(&rasterizerDesc, &rasterizerState)))
	{
		throw std::exception("LcRenderSystemDX10(): Cannot create rasterizer state");
	}

	d3dDevice->RSSetState(rasterizerState.Get());

	// add sprite renders
	visual2DRenders.push_back(std::make_shared<LcColoredSpriteRenderDX10>(context));
	visual2DRenders.push_back(std::make_shared<LcTexturedVisual2DRenderDX10>(context));
	textureRender = static_cast<IVisual2DRender*>(visual2DRenders.back().get());
	visual2DRenders.push_back(std::make_shared<LcAnimatedSpriteRenderDX10>(context));
	visual2DRenders.push_back(std::make_shared<LcTiledVisual2DRenderDX10>(context));
	visual2DRenders.push_back(std::make_shared<LcBasicParticlesRenderDX10>(context));
	tiledRender = static_cast<LcTiledVisual2DRenderDX10*>(visual2DRenders.back().get());
	visual2DRenders.front()->Setup(nullptr, context);

	// add widget render
	float dpi = (float)GetDpiForWindow(hWnd);
	textRender.reset(new LcTextRenderDX10(dpi));
	textRender->Init(context);

	// init managers
	texLoader.reset(new LcTextureLoaderDX10());

	// setup world
	context.world->GetCamera().Set(cameraPos, cameraTarget);
	worldScaleFonts = context.world->GetWorldScale().GetScaleFonts();

	// add factory
	LcWorld& worldRef = static_cast<LcWorld&>(*context.world);
	worldRef.SetLifetimeStrategy(std::make_shared<LcVisual2DLifetimeStrategyDX10>());

	// init render system
	LcRenderSystemBase::Create(this, winMode, inVSync, inAllowFullscreen, context);

	LcMakeWindowAssociation(hWnd);

	renderSystemSize = LcSize(width, height);

	LC_CATCH{ LC_THROW("LcRenderSystemDX10::Create()") }
}

void LcRenderSystemDX10::Shutdown()
{
	LcRenderSystemBase::Shutdown();

	texLoader.reset();
	textRender.reset();
	visual2DRenders.clear();

	constBuffers.Destroy();
	rasterizerState.Reset();
	blendState.Reset();
	renderTargetView.Reset();
	swapChain.Reset();
	d3dDevice.Reset();
}

void LcRenderSystemDX10::Clear(IWorld* world, bool removeRooted)
{
	if (removeRooted)
	{
		texLoader->RemoveTextures();
		if (tiledRender) tiledRender->RemoveTiles();
		if (textRender) textRender->RemoveFonts();
	}
	else
	{
		texLoader->ClearCache(world);
		if (tiledRender) tiledRender->ClearCache(world);
		if (textRender) textRender->ClearCache(world);
	}
}

void LcRenderSystemDX10::Subscribe(const LcAppContext& context)
{
	auto contextPtr = &context;

	context.world->GetWorldScale().onScaleChanged.AddListener([this, contextPtr](LcVector2 newScale)
	{
		LC_TRY

		worldScale = LcVector3(newScale.x, newScale.y, 1.0f);

		if (contextPtr->world->GetWorldScale().GetScaleFonts())
		{
			auto& visuals = contextPtr->world->GetVisuals();
			for (auto& visual : visuals)
			{
				if (visual->GetTypeId() == LcCreatables::Widget)
				{
					auto widget = static_cast<IWidget*>(visual.get());
					widget->RecreateFont(*contextPtr);
				}
			}
		}

		LC_CATCH{ LC_THROW("LcRenderSystemDX10::worldScaleUpdated()") }
	});

	context.world->onTintChanged.AddListener([this](LcColor3 globalTint)
	{
		if (constBuffers.settingsBuffer)
		{
			VS_SETTINGS_BUFFER settings;
			settings.worldTint = To4(globalTint);
			d3dDevice->UpdateSubresource(constBuffers.settingsBuffer.Get(), 0, NULL, &settings, 0, 0);
		}
	});
}

void LcRenderSystemDX10::Update(float deltaSeconds, const LcAppContext& context)
{
	LcRenderSystemBase::Update(deltaSeconds, context);
}

void LcRenderSystemDX10::UpdateCamera(float deltaSeconds, LcVector3 newPos, LcVector3 newTarget)
{
	auto viewMatrix = LookAtMatrix(newPos, newTarget);
	d3dDevice->UpdateSubresource(constBuffers.viewMatrixBuffer.Get(), 0, NULL, &viewMatrix, 0, 0);
}

void LcRenderSystemDX10::Render(const LcAppContext& context)
{
	LC_TRY

	if (!d3dDevice || !swapChain)
	{
		throw std::exception("LcRenderSystemDX10::Render(): Invalid render device");
	}

	LcColor4 color(0.0f, 0.0f, 1.0f, 0.0f);
	d3dDevice->ClearRenderTargetView(renderTargetView.Get(), (FLOAT*)&color);

	LcRenderSystemBase::Render(context);

	swapChain->Present(vSync ? DXGI_SWAP_EFFECT_SEQUENTIAL : DXGI_SWAP_EFFECT_DISCARD, 0);

	LC_CATCH{ LC_THROW("LcRenderSystemDX10::Render()") }
}

void LcRenderSystemDX10::RequestResize(int width, int height)
{
	LC_TRY

	DXGI_MODE_DESC displayModeDesc{};
	if (!LcFindDisplayMode(width, height, &displayModeDesc))
	{
		throw std::exception("LcRenderSystemDX10::RequestResize(): Cannot find display mode");
	}

	swapChain->ResizeTarget(&displayModeDesc);

	LC_CATCH{ LC_THROW("LcRenderSystemDX10::RequestResize()") }
}

void LcRenderSystemDX10::Resize(int width, int height, const LcAppContext& context)
{
	LC_TRY

	LcSize newViewportSize(width, height);
	bool needResize = (renderSystemSize != newViewportSize);

	if (swapChain && needResize)
	{
		// reset render system
		d3dDevice->OMSetRenderTargets(0, NULL, NULL);
		renderTargetView.Reset();
		textRender->Shutdown();

		// resize swap chain
		UINT flags = allowFullscreen ? DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH : 0u;
		if (FAILED(swapChain->ResizeBuffers(2, width, height, DXGI_FORMAT_R8G8B8A8_UNORM, flags)))
		{
			throw std::exception("LcRenderSystemDX10::Resize(): Cannot resize swap chain");
		}

		// create render target
		ComPtr<ID3D10Texture2D> backBuffer;
		if (FAILED(swapChain->GetBuffer(0, __uuidof(ID3D10Texture2D), (LPVOID*)backBuffer.GetAddressOf())))
		{
			throw std::exception("LcRenderSystemDX10::Create(): Cannot get back buffer");
		}

		if (FAILED(d3dDevice->CreateRenderTargetView(backBuffer.Get(), NULL, renderTargetView.GetAddressOf())))
		{
			throw std::exception("LcRenderSystemDX10::Create(): Cannot create render target");
		}

		backBuffer.Reset();
		d3dDevice->OMSetRenderTargets(1, renderTargetView.GetAddressOf(), NULL);

		// set the viewport
		D3D10_VIEWPORT viewPort;
		viewPort.Width = width;
		viewPort.Height = height;
		viewPort.MinDepth = 0.0f;
		viewPort.MaxDepth = 1.0f;
		viewPort.TopLeftX = 0;
		viewPort.TopLeftY = 0;

		d3dDevice->RSSetViewports(1, &viewPort);

		// recreate widget render
		textRender->Init(context);

		// update world settings
		cameraPos = LcVector3(width / 2.0f, height / 2.0f, 0.0f);
		cameraTarget = LcVector3(cameraPos.x, cameraPos.y, 1.0f);

		context.world->UpdateWorldScale(newViewportSize);
		context.world->GetCamera().Set(cameraPos, cameraTarget);
		UpdateCamera(0.1f, cameraPos, cameraTarget);

		// update projection matrix
		LcMatrix4 proj = OrthoMatrix(LcSize(width, height), 1.0f, -1.0f);
		d3dDevice->UpdateSubresource(constBuffers.projMatrixBuffer.Get(), 0, NULL, &proj, 0, 0);

		renderSystemSize = newViewportSize;
	}

	LC_CATCH{ LC_THROW("LcRenderSystemDX10::Resize()") }
}

void LcRenderSystemDX10::SetMode(LcWinMode winMode)
{
	if (swapChain) swapChain->SetFullscreenState((winMode == LcWinMode::Fullscreen), nullptr);
}

LcRSStats LcRenderSystemDX10::GetStats() const
{
	return LcRSStats{
		texLoader->GetNumTextures(),
		tiledRender ? tiledRender->GetNumTiles() : 0,
		textRender ? textRender->GetNumFonts() : 0
	};
}

void LcRenderSystemDX10::Render(const IVisual* visual, const LcAppContext& context)
{
	LC_TRY

	if (!visual) throw std::exception("LcRenderSystemDX10::Render(): Invalid visual");

	for (auto& render : visual2DRenders)
	{
		if (render->Supports(visual->GetFeaturesList()))
		{
			if (prevSpriteFeatures != visual->GetFeaturesList() || prevSetupRequested)
			{
				prevSetupRequested = false;
				prevSpriteFeatures = visual->GetFeaturesList();
				render->Setup(visual, context);
			}

			render->Render(visual, context);
			break;
		}
	}

	LC_CATCH{ LC_THROW("LcRenderSystemDX10::Render()") }
}

std::string LcRenderSystemDX10::GetShaderCode(const std::string& shaderName) const
{
	return shaders.at(shaderName);
}

TRenderSystemPtr GetRenderSystem()
{
	return std::make_unique<LcRenderSystemDX10>();
}
