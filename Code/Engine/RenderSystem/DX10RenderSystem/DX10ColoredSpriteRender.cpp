/**
* DX10ColoredSpriteRender.cpp
* 28.01.2023
* (c) Denis Romakhov
*/

#include "pch.h"
#include "RenderSystem/DX10RenderSystem/DX10ColoredSpriteRender.h"
#include "Core/LCUtils.h"


static const char* coloredSpriteShaderName = "ColoredSprite2d.shader";
struct DX10COLOREDSPRITEDATA
{
	Eigen::Vector3f pos; // position
	unsigned int index;  // vertex index
};

DX10ColoredSpriteRender::DX10ColoredSpriteRender(IDX10RenderDevice& inRenderDevice) : renderDevice(inRenderDevice)
{
	vs = nullptr;
	ps = nullptr;
	vertexBuffer = nullptr;
	vertexLayout = nullptr;
	auto d3dDevice = renderDevice.GetD3D10Device();
	if (!d3dDevice) throw std::exception("DX10ColoredSpriteRender(): Invalid arguments");

	auto shaderCode = renderDevice.GetShaderCode(coloredSpriteShaderName);

	ID3D10Blob *vertexBlob;
	if (FAILED(D3D10CompileShader(shaderCode.c_str(), shaderCode.length(), NULL, NULL, NULL, "VShader", "vs_4_0", 0, &vertexBlob, NULL)))
	{
		throw std::exception("DX10ColoredSpriteRender(): Cannot compile vertex shader");
	}

	if (FAILED(d3dDevice->CreateVertexShader((DWORD*)vertexBlob->GetBufferPointer(), vertexBlob->GetBufferSize(), &vs)))
	{
		vertexBlob->Release();
		throw std::exception("DX10ColoredSpriteRender(): Cannot create vertex shader");
	}

	ID3D10Blob *pixelBlob;
	if (FAILED(D3D10CompileShader(shaderCode.c_str(), shaderCode.length(), NULL, NULL, NULL, "PShader", "ps_4_0", 0, &pixelBlob, NULL)))
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
}

DX10ColoredSpriteRender::~DX10ColoredSpriteRender()
{
	if (vertexBuffer) { vertexBuffer->Release(); vertexBuffer = nullptr; }
	if (vertexLayout) { vertexLayout->Release(); vertexLayout = nullptr; }
	if (vs) { vs->Release(); vs = nullptr; }
	if (ps) { ps->Release(); ps = nullptr; }
}

void DX10ColoredSpriteRender::Setup()
{
	auto d3dDevice = renderDevice.GetD3D10Device();
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
	auto d3dDevice = renderDevice.GetD3D10Device();
	auto transMatrix = renderDevice.GetTransformBuffer();
	if (!d3dDevice || !transMatrix) throw std::exception("DX10ColoredSpriteRender::Render(): Invalid render device");

	LcVector2 offset = renderDevice.GetOffset();
	LcVector3 pos(sprite.pos.x() + offset.x(), sprite.pos.y() + offset.y(), sprite.pos.z());

	transData.trans = TransformMatrix(pos, sprite.size, sprite.rotZ).transpose();
	transData.colors[0] = sprite.colors.rightTop;
	transData.colors[1] = sprite.colors.rightBottom;
	transData.colors[2] = sprite.colors.leftTop;
	transData.colors[3] = sprite.colors.leftBottom;

	d3dDevice->UpdateSubresource(transMatrix, 0, NULL, &transData, 0, 0);
	d3dDevice->Draw(4, 0);
}
