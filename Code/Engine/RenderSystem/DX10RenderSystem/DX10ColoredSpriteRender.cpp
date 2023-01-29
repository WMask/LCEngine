/**
* DX10ColoredSpriteRender.cpp
* 28.01.2023
* (c) Denis Romakhov
*/

#include "pch.h"
#include "RenderSystem/DX10RenderSystem/DX10ColoredSpriteRender.h"
#include "Core/LCUtils.h"


#define SHADER(x) #x

const char* shaderCode = SHADER(
	cbuffer VS_PROJ_BUFFER : register(b0)
	{
		float4x4 mProj;
		float2 vOffset;
	};

	cbuffer VS_TRANS_BUFFER : register(b1)
	{
		float4x4 mTrans;
		float4 vColors[4];
	};

	struct VOut
	{
		float4 position : SV_POSITION;
		float4 color : COLOR;
	};

	VOut VShader(float4 position : POSITION, uint index : INDEX)
	{
		VOut output;

		output.position = mul(position, mul(mTrans, mProj));
		output.color = vColors[index];

		return output;
	}

	float4 PShader(float4 position : SV_POSITION, float4 color : COLOR) : SV_TARGET
	{
		return color;
	}
);

struct DX10COLOREDSPRITEDATA
{
	Eigen::Vector3f pos; // position
	int index;           // vertex index
};

DX10ColoredSpriteRender::DX10ColoredSpriteRender(IDX10RenderDevice* inRenderDevice)
{
	vs = nullptr;
	ps = nullptr;
	vertexBuffer = nullptr;
	vertexLayout = nullptr;
	rasterizerState = nullptr;
	renderDevice = inRenderDevice;
	auto d3dDevice = renderDevice ? renderDevice->GetD3D10Device() : nullptr;
	if (!d3dDevice) throw std::exception("DX10ColoredSpriteRender(): Invalid arguments");

	ID3D10Blob *vertexBlob;
	if (FAILED(D3D10CompileShader(shaderCode, strlen(shaderCode), NULL, NULL, NULL, "VShader", "vs_4_0", 0, &vertexBlob, NULL)))
	{
		throw std::exception("DX10ColoredSpriteRender(): Cannot compile vertex shader");
	}

	if (FAILED(d3dDevice->CreateVertexShader((DWORD*)vertexBlob->GetBufferPointer(), vertexBlob->GetBufferSize(), &vs)))
	{
		vertexBlob->Release();
		throw std::exception("DX10ColoredSpriteRender(): Cannot create vertex shader");
	}

	ID3D10Blob *pixelBlob;
	if (FAILED(D3D10CompileShader(shaderCode, strlen(shaderCode), NULL, NULL, NULL, "PShader", "ps_4_0", 0, &pixelBlob, NULL)))
	{
		throw std::exception("DX10ColoredSpriteRender(): Cannot compile pixel shader");
	}

	if (FAILED(d3dDevice->CreatePixelShader((DWORD*)pixelBlob->GetBufferPointer(), pixelBlob->GetBufferSize(), &ps)))
	{
		pixelBlob->Release();
		throw std::exception("DX10ColoredSpriteRender(): Cannot create pixel shader");
	}

	pixelBlob->Release();

	D3D10_INPUT_ELEMENT_DESC layout[] =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D10_INPUT_PER_VERTEX_DATA, 0},
		{"INDEX", 0, DXGI_FORMAT_R32_SINT, 0, 12, D3D10_INPUT_PER_VERTEX_DATA, 0}
	};

	if (FAILED(d3dDevice->CreateInputLayout(layout, 2, vertexBlob->GetBufferPointer(), vertexBlob->GetBufferSize(), &vertexLayout)))
	{
		throw std::exception("DX10ColoredSpriteRender(): Cannot create input layout");
	}
	vertexBlob->Release();

	// create vertex buffer
	D3D10_BUFFER_DESC bufferDesc;
	bufferDesc.Usage = D3D10_USAGE_DYNAMIC;
	bufferDesc.ByteWidth = sizeof(DX10COLOREDSPRITEDATA) * 4;
	bufferDesc.BindFlags = D3D10_BIND_VERTEX_BUFFER;
	bufferDesc.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE;
	bufferDesc.MiscFlags = 0;
	if (FAILED(d3dDevice->CreateBuffer(&bufferDesc, NULL, &vertexBuffer)))
	{
		throw std::exception("DX10ColoredSpriteRender(): Cannot create vertex buffer");
	}

	// fill vertex buffer
	DX10COLOREDSPRITEDATA* vertices;
	vertexBuffer->Map(D3D10_MAP_WRITE_DISCARD, 0, (void**)&vertices);
	vertices[0] = DX10COLOREDSPRITEDATA{ Eigen::Vector3f( 0.5,-0.5, 0), 0 };
	vertices[1] = DX10COLOREDSPRITEDATA{ Eigen::Vector3f( 0.5, 0.5, 0), 1 };
	vertices[2] = DX10COLOREDSPRITEDATA{ Eigen::Vector3f(-0.5,-0.5, 0), 2 };
	vertices[3] = DX10COLOREDSPRITEDATA{ Eigen::Vector3f(-0.5, 0.5, 0), 3 };
	vertexBuffer->Unmap();

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
}

DX10ColoredSpriteRender::~DX10ColoredSpriteRender()
{
	renderDevice = nullptr;

	if (rasterizerState)
	{
		rasterizerState->Release();
		rasterizerState = nullptr;
	}

	if (vertexBuffer)
	{
		vertexBuffer->Release();
		vertexBuffer = nullptr;
	}

	if (vertexLayout)
	{
		vertexLayout->Release();
		vertexLayout = nullptr;
	}

	if (vs)
	{
		vs->Release();
		vs = nullptr;
	}

	if (ps)
	{
		ps->Release();
		ps = nullptr;
	}
}

void DX10ColoredSpriteRender::Setup()
{
	auto d3dDevice = renderDevice ? renderDevice->GetD3D10Device() : nullptr;
	if (!d3dDevice) throw std::exception("DX10ColoredSpriteRender::Setup(): Invalid render device");

	d3dDevice->VSSetShader(vs);
	d3dDevice->PSSetShader(ps);
	d3dDevice->IASetInputLayout(vertexLayout);

	UINT stride = sizeof(DX10COLOREDSPRITEDATA);
	UINT offset = 0;
	d3dDevice->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	d3dDevice->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
}

void DX10ColoredSpriteRender::Render(const SPRITE_DATA& sprite)
{
	auto d3dDevice = renderDevice ? renderDevice->GetD3D10Device() : nullptr;
	auto transMatrix = renderDevice ? renderDevice->GetTransformBuffer() : nullptr;
	if (!d3dDevice || !transMatrix) throw std::exception("DX10ColoredSpriteRender::Render(): Invalid render device");

	Eigen::Vector2f offset = renderDevice->GetOffset();
	Eigen::Vector3f pos(sprite.pos.x() + offset.x(), sprite.pos.y() + offset.y(), sprite.pos.z());

	transData.trans = TransformMatrix(pos, sprite.size, sprite.rotZ).transpose();
	transData.colors[0] = sprite.colors.rightTop;
	transData.colors[1] = sprite.colors.rightBottom;
	transData.colors[2] = sprite.colors.leftTop;
	transData.colors[3] = sprite.colors.leftBottom;

	d3dDevice->UpdateSubresource(transMatrix, 0, NULL, &transData, 0, 0);
	d3dDevice->Draw(4, 0);
}
