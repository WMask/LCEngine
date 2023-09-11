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
#include "Application/ApplicationInterface.h"
#include "World/WorldInterface.h"
#include "World/Sprites.h"
#include "World/Camera.h"
#include "GUI/GuiManager.h"
#include "Core/LCException.h"


class LcSpriteFactoryDX10 : public TWorldFactory<ISprite, LcSpriteData>
{
public:
	LcSpriteFactoryDX10(LcRenderSystemDX10& inRender) : render(inRender), tiledRender(nullptr)
	{
		TVFeaturesList features = { EVCType::Tiled, EVCType::Texture };
		auto& renders = render.GetVisual2DRenderList();
		for (auto& render : renders)
		{
			if (render->Supports(features))
			{
				tiledRender = static_cast<LcTiledVisual2DRenderDX10*>(render.get());
				break;
			}
		}
	}
	//
	virtual std::shared_ptr<ISprite> Build(const LcSpriteData& data) override
	{
		auto newSprite = std::make_shared<LcSpriteDX10>(data, render);
		newSprite->tiledRender = tiledRender;
		return newSprite;
	}
	//
	LcRenderSystemDX10& render;
	//
	LcTiledVisual2DRenderDX10* tiledRender;
};

class LcWidgetFactoryDX10 : public TWorldFactory<IWidget, LcWidgetData>
{
public:
	LcWidgetFactoryDX10(LcRenderSystemDX10& inRender) : render(inRender) {}
	//
	virtual std::shared_ptr<IWidget> Build(const LcWidgetData& data) override
	{
		return std::make_shared<LcWidgetDX10>(data, render);
	}
	//
	LcRenderSystemDX10& render;
};

LcRenderSystemDX10::LcRenderSystemDX10()
	: widgetRender(nullptr)
	, renderSystemSize(0, 0)
	, prevSetupRequested(false)
{
}

LcRenderSystemDX10::~LcRenderSystemDX10()
{
	Shutdown();
}

void LcRenderSystemDX10::Create(TWeakWorld inWorld, void* windowHandle, LcWinMode winMode, bool inVSync)
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

	// create the D3D device
	if (FAILED(D3D10CreateDeviceAndSwapChain1(NULL,
		D3D10_DRIVER_TYPE_HARDWARE, NULL, D3D10_CREATE_DEVICE_BGRA_SUPPORT,
		D3D10_FEATURE_LEVEL_10_0, D3D10_1_SDK_VERSION, &swapChainDesc, swapChain.GetAddressOf(), d3dDevice.GetAddressOf())))
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

	// define constant buffers
	struct VS_MATRIX_BUFFER
	{
		LcMatrix4 mat;
	};
	VS_MATRIX_BUFFER matData;

	struct VS_COLORS_BUFFER
	{
		LcColor4 colors[4];
	};
	VS_COLORS_BUFFER colorsData;

	struct VS_CUSTOM_UV_BUFFER
	{
		LcVector4 uv[4];
	};
	VS_CUSTOM_UV_BUFFER uvData;

	struct VS_FRAME_ANIM2D_BUFFER
	{
		LcVector4 animData;
	};
	VS_FRAME_ANIM2D_BUFFER anim2dData;

	D3D10_BUFFER_DESC cbDesc{};
	cbDesc.ByteWidth = sizeof(VS_MATRIX_BUFFER);
	cbDesc.Usage = D3D10_USAGE_DEFAULT;
	cbDesc.BindFlags = D3D10_BIND_CONSTANT_BUFFER;

	D3D10_SUBRESOURCE_DATA subResData{};
	subResData.pSysMem = &matData;

	// create constant buffers
	matData.mat = OrthoMatrix(LcSize(width, height), 1.0f, -1.0f);
	if (FAILED(d3dDevice->CreateBuffer(&cbDesc, &subResData, projMatrixBuffer.GetAddressOf())))
	{
		throw std::exception("LcRenderSystemDX10(): Cannot create constant buffer");
	}

	matData.mat = IdentityMatrix();
	if (FAILED(d3dDevice->CreateBuffer(&cbDesc, &subResData, transMatrixBuffer.GetAddressOf())))
	{
		throw std::exception("LcRenderSystemDX10(): Cannot create constant buffer");
	}

	cameraPos = LcVector3(width / 2.0f, height / 2.0f, 0.0f);
	cameraTarget = LcVector3(cameraPos.x, cameraPos.y, 1.0f);
	matData.mat = LookAtMatrix(cameraPos, cameraTarget);
	if (FAILED(d3dDevice->CreateBuffer(&cbDesc, &subResData, viewMatrixBuffer.GetAddressOf())))
	{
		throw std::exception("LcRenderSystemDX10(): Cannot create constant buffer");
	}

	subResData.pSysMem = &colorsData;
	colorsData.colors[0] = LcDefaults::White4;
	colorsData.colors[1] = LcDefaults::White4;
	colorsData.colors[2] = LcDefaults::White4;
	colorsData.colors[3] = LcDefaults::White4;
	if (FAILED(d3dDevice->CreateBuffer(&cbDesc, &subResData, colorsBuffer.GetAddressOf())))
	{
		throw std::exception("LcRenderSystemDX10(): Cannot create constant buffer");
	}

	subResData.pSysMem = &uvData;
	uvData.uv[1] = To4(LcVector2(1.0, 0.0));
	uvData.uv[2] = To4(LcVector2(1.0, 1.0));
	uvData.uv[0] = To4(LcVector2(0.0, 0.0));
	uvData.uv[3] = To4(LcVector2(0.0, 1.0));
	if (FAILED(d3dDevice->CreateBuffer(&cbDesc, &subResData, customUvBuffer.GetAddressOf())))
	{
		throw std::exception("LcRenderSystemDX10(): Cannot create constant buffer");
	}

	cbDesc.ByteWidth = sizeof(VS_FRAME_ANIM2D_BUFFER);
	subResData.pSysMem = &anim2dData;
	anim2dData.animData = LcVector4(1.0, 1.0, 0.0, 0.0);
	if (FAILED(d3dDevice->CreateBuffer(&cbDesc, &subResData, frameAnimBuffer.GetAddressOf())))
	{
		throw std::exception("LcRenderSystemDX10(): Cannot create constant buffer");
	}

	// set buffers
	d3dDevice->VSSetConstantBuffers(0, 1, projMatrixBuffer.GetAddressOf());
	d3dDevice->VSSetConstantBuffers(1, 1, viewMatrixBuffer.GetAddressOf());
	d3dDevice->VSSetConstantBuffers(2, 1, transMatrixBuffer.GetAddressOf());
	d3dDevice->VSSetConstantBuffers(3, 1, colorsBuffer.GetAddressOf());
	d3dDevice->VSSetConstantBuffers(4, 1, customUvBuffer.GetAddressOf());
	d3dDevice->VSSetConstantBuffers(5, 1, frameAnimBuffer.GetAddressOf());

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
	visual2DRenders.push_back(std::make_shared<LcTexturedVisual2DRenderDX10>(*this));
	visual2DRenders.push_back(std::make_shared<LcAnimatedSpriteRenderDX10>(*this));
	visual2DRenders.push_back(std::make_shared<LcTiledVisual2DRenderDX10>(*this));
	visual2DRenders.push_back(std::make_shared<LcColoredSpriteRenderDX10>(*this));
	visual2DRenders.back()->Setup(nullptr);

	// add widget render
	visual2DRenders.push_back(std::make_shared<LcWidgetRenderDX10>(*this, hWnd));
	widgetRender = (LcWidgetRenderDX10*)visual2DRenders.back().get();
	widgetRender->Setup(nullptr);

	// init managers
	texLoader.reset(new LcTextureLoaderDX10(inWorld));

	// add sprite factory
	if (auto world = inWorld.lock())
	{
		world->GetCamera().Set(cameraPos, cameraTarget);
		world->SetSpriteFactory(std::make_shared<LcSpriteFactoryDX10>(*this));
		world->SetWidgetFactory(std::make_shared<LcWidgetFactoryDX10>(*this));
	}

	// init render system
	LcRenderSystemBase::Create(inWorld, this, winMode, inVSync);

	LcMakeWindowAssociation(hWnd);

	renderSystemSize = LcSize(width, height);

	LC_CATCH{ LC_THROW("LcRenderSystemDX10::Create()") }
}

void LcRenderSystemDX10::Subscribe(LcDelegate<void(LcVector2)>& worldScaleUpdated)
{
	worldScaleUpdated.AddListener([this](LcVector2 newScale) {
		if (auto world = worldPtr.lock())
		{
			if (world->GetWorldScale().scaleFonts)
			{
				auto& widgets = world->GetWidgets();
				for (auto widget : widgets) widget->RecreateFont();
			}
		}
	});
}

void LcRenderSystemDX10::Shutdown()
{
	LcRenderSystemBase::Shutdown();

	texLoader.reset();
	visual2DRenders.clear();
	widgetRender = nullptr;

	rasterizerState.Reset();
	blendState.Reset();
	frameAnimBuffer.Reset();
	customUvBuffer.Reset();
	colorsBuffer.Reset();
	transMatrixBuffer.Reset();
	projMatrixBuffer.Reset();
	viewMatrixBuffer.Reset();
	renderTargetView.Reset();
	swapChain.Reset();
	d3dDevice.Reset();
}

void LcRenderSystemDX10::Update(float deltaSeconds)
{
	LcRenderSystemBase::Update(deltaSeconds);
}

void LcRenderSystemDX10::UpdateCamera(float deltaSeconds, LcVector3 newPos, LcVector3 newTarget)
{
	auto viewMatrix = LookAtMatrix(newPos, newTarget);
	d3dDevice->UpdateSubresource(viewMatrixBuffer.Get(), 0, NULL, &viewMatrix, 0, 0);
}

void LcRenderSystemDX10::Render()
{
	LC_TRY

	if (!d3dDevice || !swapChain)
	{
		throw std::exception("LcRenderSystemDX10::Render(): Invalid render device");
	}

	LcColor4 color(0.0f, 0.0f, 1.0f, 0.0f);
	d3dDevice->ClearRenderTargetView(renderTargetView.Get(), (FLOAT*)&color);

	LcRenderSystemBase::Render();

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

void LcRenderSystemDX10::Resize(int width, int height)
{
	LC_TRY

	LcSize newViewportSize(width, height);
	bool needResize = (renderSystemSize != newViewportSize);

	if (swapChain && needResize)
	{
		// reset render system
		d3dDevice->OMSetRenderTargets(0, NULL, NULL);
		renderTargetView.Reset();
		widgetRender->Shutdown();

		// resize swap chain
		if (FAILED(swapChain->ResizeBuffers(2, width, height, DXGI_FORMAT_R8G8B8A8_UNORM, 0)))
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
		widgetRender->Setup(nullptr);

		// update world settings
		if (auto world = worldPtr.lock())
		{
			cameraPos = LcVector3(width / 2.0f, height / 2.0f, 0.0f);
			cameraTarget = LcVector3(cameraPos.x, cameraPos.y, 1.0f);

			world->GetWorldScale().UpdateWorldScale(newViewportSize);
			world->GetCamera().Set(cameraPos, cameraTarget);
			UpdateCamera(0.1f, cameraPos, cameraTarget);
		}

		// update projection matrix
		LcMatrix4 proj = OrthoMatrix(LcSize(width, height), 1.0f, -1.0f);
		d3dDevice->UpdateSubresource(projMatrixBuffer.Get(), 0, NULL, &proj, 0, 0);

		renderSystemSize = newViewportSize;
	}

	LC_CATCH{ LC_THROW("LcRenderSystemDX10::Resize()") }
}

void LcRenderSystemDX10::SetMode(LcWinMode winMode)
{
	if (swapChain) swapChain->SetFullscreenState((winMode == LcWinMode::Fullscreen), nullptr);
}

void LcRenderSystemDX10::RenderSprite(const ISprite* sprite)
{
	LC_TRY

	if (!sprite) throw std::exception("LcRenderSystemDX10::RenderSprite(): Invalid sprite");

	for (auto& render : visual2DRenders)
	{
		if (render->Supports(sprite->GetFeaturesList()))
		{
			if (prevSpriteFeatures != sprite->GetFeaturesList() || prevSetupRequested)
			{
				prevSetupRequested = false;
				prevSpriteFeatures = sprite->GetFeaturesList();
				render->Setup(sprite);
			}

			render->RenderSprite(sprite);
			break;
		}
	}

	LC_CATCH{ LC_THROW("LcRenderSystemDX10::RenderSprite()") }
}

void LcRenderSystemDX10::RenderWidget(const IWidget* widget)
{
	LC_TRY

	if (!widget) throw std::exception("LcRenderSystemDX10::RenderWidget(): Invalid sprite");

	if (widgetRender)
	{
		widgetRender->RenderWidget(widget);
	}

	LC_CATCH{ LC_THROW("LcRenderSystemDX10::RenderWidget()") }
}

void LcRenderSystemDX10::PreRenderWidgets(EWRMode mode)
{
	LC_TRY

	if (widgetRender)
	{
		widgetRender->SetRenderMode(mode);

		switch (mode)
		{
		case Textures:
			if (auto textureRender = widgetRender->GetTextureRender())
			{
				textureRender->Setup(nullptr);
				ForceRenderSetup();
			}
			break;
		case Text:
			widgetRender->BeginRender();
			break;
		}
	}

	LC_CATCH{ LC_THROW("LcRenderSystemDX10::PreRenderWidgets()") }
}

void LcRenderSystemDX10::PostRenderWidgets(EWRMode mode)
{
	if (widgetRender) widgetRender->EndRender();
}

std::string LcRenderSystemDX10::GetShaderCode(const std::string& shaderName) const
{
	return shaders.at(shaderName);
}

TRenderSystemPtr GetRenderSystem()
{
	return std::make_unique<LcRenderSystemDX10>();
}
