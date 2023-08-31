/**
* RenderSystemDX10.cpp
* 28.01.2023
* (c) Denis Romakhov
*/

#include "pch.h"
#include "RenderSystem/RenderSystemDX10/RenderSystemDX10.h"
#include "RenderSystem/RenderSystemDX10/ColoredSpriteRenderDX10.h"
#include "RenderSystem/RenderSystemDX10/TexturedSpriteRenderDX10.h"
#include "RenderSystem/RenderSystemDX10/AnimatedSpriteRenderDX10.h"
#include "Application/Application.h"
#include "World/WorldInterface.h"
#include "World/Sprites.h"
#include "World/Camera.h"
#include "GUI/GuiManager.h"


class LcSpriteFactoryDX10 : public TWorldFactory<ISprite, LcSpriteData>
{
public:
	LcSpriteFactoryDX10(LcRenderSystemDX10& inRender) : render(inRender) {}
	//
	virtual std::shared_ptr<ISprite> Build(const LcSpriteData& data) override
	{
		return std::make_shared<LcSpriteDX10>(data, render);
	}
	//
	LcRenderSystemDX10& render;
};

class LcWidgetFactoryDX10 : public TWorldFactory<IWidget, LcWidgetData>
{
public:
	LcWidgetFactoryDX10(IWidgetRender& inRender) : render(inRender) {}
	//
	virtual std::shared_ptr<IWidget> Build(const LcWidgetData& data) override
	{
		return std::make_shared<LcWidgetDX10>(data, render);
	}
	//
	IWidgetRender& render;
};

void LcSpriteDX10::AddComponent(TVComponentPtr comp)
{
	LcSprite::AddComponent(comp);

	if (auto texComp = GetTextureComponent())
	{
		LcSize texSize;
		bool loaded = render.GetTextureLoader()->LoadTexture(
			texComp->texture.c_str(), render.GetD3D10Device(), texture.GetAddressOf(), shaderView.GetAddressOf(), &texSize);
		if (loaded)
			texComp->texSize = ToF(texSize);
		else
			throw std::exception("LcSpriteDX10::AddComponent(): Cannot load texture");
	}
}

void LcWidgetDX10::AddComponent(TVComponentPtr comp)
{
	LcWidget::AddComponent(comp);

	if (auto textComp = GetTextComponent())
	{
		font = render.AddFont(textComp->fontName, textComp->fontSize, textComp->fontWeight);
		if (!font)
			throw std::exception("LcWidgetDX10::AddComponent(): Cannot create font");
	}
}

LcRenderSystemDX10::LcRenderSystemDX10()
{
}

LcRenderSystemDX10::~LcRenderSystemDX10()
{
	Shutdown();
}

void LcRenderSystemDX10::Create(TWeakWorld worldPtr, void* windowHandle, bool windowed)
{
	HWND hWnd = (HWND)windowHandle;
	RECT clientRect;
	GetClientRect(hWnd, &clientRect);

	int width = clientRect.right - clientRect.left, height = clientRect.bottom - clientRect.top;

	DXGI_SWAP_CHAIN_DESC swapChainDesc{};
	swapChainDesc.BufferCount = 2;
	swapChainDesc.BufferDesc.Width = width;
	swapChainDesc.BufferDesc.Height = height;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
	swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.OutputWindow = hWnd;
	swapChainDesc.Windowed = true;

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

	struct VS_FRAME_ANIM2D_BUFFER
	{
		LcVector4 animData;
	};
	VS_FRAME_ANIM2D_BUFFER anim2dData;

	D3D10_BUFFER_DESC cbDesc;
	cbDesc.ByteWidth = sizeof(VS_MATRIX_BUFFER);
	cbDesc.Usage = D3D10_USAGE_DEFAULT;
	cbDesc.BindFlags = D3D10_BIND_CONSTANT_BUFFER;
	cbDesc.CPUAccessFlags = 0;
	cbDesc.MiscFlags = 0;

	D3D10_SUBRESOURCE_DATA subResData;
	subResData.pSysMem = &matData;
	subResData.SysMemPitch = 0;
	subResData.SysMemSlicePitch = 0;

	// create constant buffers
	matData.mat = OrthoMatrix(LcSize(width, height), 1.0f, -1.0f);
	if (FAILED(d3dDevice->CreateBuffer(&cbDesc, &subResData, &projMatrixBuffer)))
	{
		throw std::exception("LcRenderSystemDX10(): Cannot create constant buffer");
	}

	matData.mat = IdentityMatrix();
	if (FAILED(d3dDevice->CreateBuffer(&cbDesc, &subResData, &transMatrixBuffer)))
	{
		throw std::exception("LcRenderSystemDX10(): Cannot create constant buffer");
	}

	cameraPos = LcVector3(width / 2.0f, height / 2.0f, 0.0f);
	cameraTarget = LcVector3(cameraPos.x, cameraPos.y, 1.0f);
	matData.mat = LookAtMatrix(cameraPos, cameraTarget);
	if (FAILED(d3dDevice->CreateBuffer(&cbDesc, &subResData, &viewMatrixBuffer)))
	{
		throw std::exception("LcRenderSystemDX10(): Cannot create constant buffer");
	}

	subResData.pSysMem = &colorsData;
	colorsData.colors[0] = LcDefaults::White4;
	colorsData.colors[1] = LcDefaults::White4;
	colorsData.colors[2] = LcDefaults::White4;
	colorsData.colors[3] = LcDefaults::White4;
	if (FAILED(d3dDevice->CreateBuffer(&cbDesc, &subResData, &colorsBuffer)))
	{
		throw std::exception("LcRenderSystemDX10(): Cannot create constant buffer");
	}

	subResData.pSysMem = &anim2dData;
	anim2dData.animData = LcVector4(1.0, 1.0, 0.0, 0.0);
	if (FAILED(d3dDevice->CreateBuffer(&cbDesc, &subResData, &frameAnimBuffer)))
	{
		throw std::exception("LcRenderSystemDX10(): Cannot create constant buffer");
	}

	// set buffers
	d3dDevice->VSSetConstantBuffers(0, 1, projMatrixBuffer.GetAddressOf());
	d3dDevice->VSSetConstantBuffers(1, 1, viewMatrixBuffer.GetAddressOf());
	d3dDevice->VSSetConstantBuffers(2, 1, transMatrixBuffer.GetAddressOf());
	d3dDevice->VSSetConstantBuffers(3, 1, colorsBuffer.GetAddressOf());
	d3dDevice->VSSetConstantBuffers(4, 1, frameAnimBuffer.GetAddressOf());

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
	spriteRenders.push_back(std::make_shared<LcTexturedSpriteRenderDX10>(*this));
	spriteRenders.push_back(std::make_shared<LcAnimatedSpriteRenderDX10>(*this));
	spriteRenders.push_back(std::make_shared<LcColoredSpriteRenderDX10>(*this));
	spriteRenders.back()->Setup();

	// init managers
	texLoader.reset(new LcTextureLoaderDX10(worldPtr));
	auto newWidgetRender = new LcWidgetRenderDX10(swapChain.Get(), hWnd);
	newWidgetRender->Setup();
	widgetRender.reset(newWidgetRender);

	// add sprite factory
	if (auto world = worldPtr.lock())
	{
		world->GetCamera().Set(cameraPos, cameraTarget);
		world->SetSpriteFactory(std::make_shared<LcSpriteFactoryDX10>(*this));
		world->SetWidgetFactory(std::make_shared<LcWidgetFactoryDX10>(*widgetRender.get()));
	}

	// init render system
	LcRenderSystemBase::Create(worldPtr, this, windowed);
}

void LcRenderSystemDX10::Shutdown()
{
	LcRenderSystemBase::Shutdown();

	widgetRender.reset();
	texLoader.reset();

	rasterizerState.Reset();
	blendState.Reset();
	frameAnimBuffer.Reset();
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
	if (!d3dDevice || !swapChain)
	{
		throw std::exception("LcRenderSystemDX10::Render(): Invalid render device");
	}

	LcColor4 color(0.0f, 0.0f, 1.0f, 0.0f);
	d3dDevice->ClearRenderTargetView(renderTargetView.Get(), (FLOAT*)&color);

	LcRenderSystemBase::Render();

	swapChain->Present(0, 0);
}

void LcRenderSystemDX10::RenderSprite(const ISprite* sprite)
{
	if (!sprite) throw std::exception("LcRenderSystemDX10::RenderSprite(): Invalid sprite");

	for (auto& render : spriteRenders)
	{
		if (render->Supports(sprite->GetFeaturesList()))
		{
			if (prevSpriteFeatures != sprite->GetFeaturesList())
			{
				prevSpriteFeatures = sprite->GetFeaturesList();
				render->Setup();
			}

			render->Render(sprite);
			break;
		}
	}
}

void LcRenderSystemDX10::RenderWidget(const IWidget* widget)
{
	if (!widget) throw std::exception("LcRenderSystemDX10::RenderSprite(): Invalid sprite");

	if (auto render = (LcWidgetRenderDX10*)widgetRender.get())
	{
		render->Render(widget);
	}
}

void LcRenderSystemDX10::PreRenderWidgets()
{
	if (auto render = (LcWidgetRenderDX10*)widgetRender.get()) render->BeginRender();
}

void LcRenderSystemDX10::PostRenderWidgets()
{
	if (auto render = (LcWidgetRenderDX10*)widgetRender.get()) render->EndRender();
}

std::string LcRenderSystemDX10::GetShaderCode(const std::string& shaderName) const
{
	return shaders.at(shaderName);
}

TRenderSystemPtr GetRenderSystem()
{
	return std::make_unique<LcRenderSystemDX10>();
}
