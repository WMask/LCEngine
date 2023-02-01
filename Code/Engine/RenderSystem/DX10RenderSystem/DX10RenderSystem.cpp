/**
* DX10RenderSystem.cpp
* 28.01.2023
* (c) Denis Romakhov
*/

#include "pch.h"
#include "RenderSystem/DX10RenderSystem/DX10RenderSystem.h"
#include "RenderSystem/DX10RenderSystem/DX10ColoredSpriteRender.h"
#include "Application/Application.h"
#include "World/World.h"
#include "Core/LCUtils.h"


DX10RenderSystem::DX10RenderSystem(IApplication& app) : IRenderSystem(app)
{
	d3dDevice = nullptr;
	swapChain = nullptr;
	renderTargetView = nullptr;
	projMatrixBuffer = nullptr;
	transMatrixBuffer = nullptr;
	blendState = nullptr;
	rasterizerState = nullptr;
}

DX10RenderSystem::~DX10RenderSystem()
{
	Shutdown();
}

void DX10RenderSystem::Create(void* Handle, LcSize viewportSize, bool windowed)
{
	int width = viewportSize.x(), height = viewportSize.y();
	initialOffset = LcVector2(width / -2.0f, height / -2.0f);

	DXGI_SWAP_CHAIN_DESC swapChainDesc;
	ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));
	ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));
	swapChainDesc.BufferCount = 2;
	swapChainDesc.BufferDesc.Width = width;
	swapChainDesc.BufferDesc.Height = height;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
	swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	swapChainDesc.SampleDesc.Quality = 0;
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.OutputWindow = (HWND)Handle;
	swapChainDesc.Windowed = true;

	// create the D3D device
	if (FAILED(D3D10CreateDeviceAndSwapChain(NULL, D3D10_DRIVER_TYPE_HARDWARE, NULL, 0,
		D3D10_SDK_VERSION, &swapChainDesc, &swapChain, &d3dDevice)))
	{
		throw std::exception("DX10RenderSystem::Create(): Cannot create D3D device");
	}

	// get back buffer
	ID3D10Texture2D* backBuffer;
	if (FAILED(swapChain->GetBuffer(0, __uuidof(ID3D10Texture2D), (LPVOID*)&backBuffer)))
	{
		throw std::exception("DX10RenderSystem::Create(): Cannot create back buffer");
	}

	// create render target
	if (FAILED(d3dDevice->CreateRenderTargetView(backBuffer, NULL, &renderTargetView)))
	{
		throw std::exception("DX10RenderSystem::Create(): Cannot create render target");
	}

	backBuffer->Release();
	d3dDevice->OMSetRenderTargets(1, &renderTargetView, NULL);

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
	struct VS_PROJ_BUFFER
	{
		Eigen::Matrix4f matrix;
	};
	VS_PROJ_BUFFER projData;
	VS_TRANS_BUFFER transData;

	D3D10_BUFFER_DESC cbDesc;
	cbDesc.ByteWidth = sizeof(VS_PROJ_BUFFER);
	cbDesc.Usage = D3D10_USAGE_DEFAULT;
	cbDesc.BindFlags = D3D10_BIND_CONSTANT_BUFFER;
	cbDesc.CPUAccessFlags = 0;
	cbDesc.MiscFlags = 0;

	D3D10_SUBRESOURCE_DATA InitData;
	InitData.pSysMem = &projData;
	InitData.SysMemPitch = 0;
	InitData.SysMemSlicePitch = 0;

	// create constant buffers
	projData.matrix = OrthoMatrix(viewportSize, -1.0f, 2.0f);
	if (FAILED(d3dDevice->CreateBuffer(&cbDesc, &InitData, &projMatrixBuffer)))
	{
		throw std::exception("DX10RenderSystem(): Cannot create constant buffer");
	}

	cbDesc.ByteWidth = sizeof(VS_TRANS_BUFFER);
	InitData.pSysMem = &transData;
	transData.trans = Eigen::Matrix4f::Identity();
	transData.colors[0] = LcVector4::Ones();
	transData.colors[1] = LcVector4::Ones();
	transData.colors[2] = LcVector4::Ones();
	transData.colors[3] = LcVector4::Ones();
	if (FAILED(d3dDevice->CreateBuffer(&cbDesc, &InitData, &transMatrixBuffer)))
	{
		throw std::exception("DX10RenderSystem(): Cannot create constant buffer");
	}

	// set buffers
	d3dDevice->VSSetConstantBuffers(0, 1, &projMatrixBuffer);
	d3dDevice->VSSetConstantBuffers(1, 1, &transMatrixBuffer);

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
	d3dDevice->OMSetBlendState(blendState, blendFactor, 0xffffffff);

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

	d3dDevice->CreateRasterizerState(&rasterizerDesc, &rasterizerState);
	d3dDevice->RSSetState(rasterizerState);

	// add sprite renders
	spriteRenders.push_back(std::shared_ptr<ISpriteRender>(new DX10ColoredSpriteRender(*this)));
	spriteRenders[0]->Setup();

	// init render system
	IRenderSystem::Create(this, viewportSize, windowed);
}

void DX10RenderSystem::Shutdown()
{
	IRenderSystem::Shutdown();

	if (rasterizerState) { rasterizerState->Release(); rasterizerState = nullptr; }
	if (blendState) { blendState->Release(); blendState = nullptr; }
	if (transMatrixBuffer) { transMatrixBuffer->Release(); transMatrixBuffer = nullptr; }
	if (projMatrixBuffer) { projMatrixBuffer->Release(); projMatrixBuffer = nullptr; }
	if (renderTargetView) { renderTargetView->Release(); renderTargetView = nullptr; }
	if (swapChain) { swapChain->Release(); swapChain = nullptr; }
	if (d3dDevice) { d3dDevice->Release(); d3dDevice = nullptr; }
}

void DX10RenderSystem::Update(float deltaSeconds)
{
}

void DX10RenderSystem::Render()
{
	if (!d3dDevice || !swapChain)
	{
		throw std::exception("DX10RenderSystem::Render(): Invalid render device");
	}

	LcColor4 color(0.0f, 0.0f, 1.0f, 0.0f);
	d3dDevice->ClearRenderTargetView(renderTargetView, color.data());

	IRenderSystem::Render();

	swapChain->Present(0, 0);
}

void DX10RenderSystem::RenderSprite(const SPRITE_DATA& sprite)
{
	for (auto& render : spriteRenders)
	{
		if (render->GetType() == sprite.type)
		{
			render->Render(sprite);
			break;
		}
	}
}

std::string DX10RenderSystem::GetShaderCode(const std::string& shaderName) const
{
	return app.GetShaders().at(shaderName);
}
