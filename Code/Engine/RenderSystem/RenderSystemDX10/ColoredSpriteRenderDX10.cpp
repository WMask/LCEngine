/**
* ColoredSpriteRenderDX10.cpp
* 28.01.2023
* (c) Denis Romakhov
*/

#include "pch.h"
#include "RenderSystem/RenderSystemDX10/ColoredSpriteRenderDX10.h"


static const char* coloredSpriteShaderName = "ColoredSprite2d.shader";
struct DX10COLOREDSPRITEDATA
{
	LcVector3 pos;		// position
	unsigned int index;	// vertex index
};

LcColoredSpriteRenderDX10::LcColoredSpriteRenderDX10(IRenderDeviceDX10& inRenderDevice) : renderDevice(inRenderDevice)
{
	vs = nullptr;
	ps = nullptr;
	vertexBuffer = nullptr;
	vertexLayout = nullptr;
	colorsBuffer = nullptr;
	auto d3dDevice = renderDevice.GetD3D10Device();
	if (!d3dDevice) throw std::exception("LcColoredSpriteRenderDX10(): Invalid arguments");

	auto shaderCode = renderDevice.GetShaderCode(coloredSpriteShaderName);

	ComPtr<ID3D10Blob> vertexBlob;
	if (FAILED(D3D10CompileShader(shaderCode.c_str(), shaderCode.length(), NULL, NULL, NULL, "VShader", "vs_4_0", 0, &vertexBlob, NULL)))
	{
		throw std::exception("LcColoredSpriteRenderDX10(): Cannot compile vertex shader");
	}

	if (FAILED(d3dDevice->CreateVertexShader((DWORD*)vertexBlob->GetBufferPointer(), vertexBlob->GetBufferSize(), &vs)))
	{
		throw std::exception("LcColoredSpriteRenderDX10(): Cannot create vertex shader");
	}

	ComPtr<ID3D10Blob> pixelBlob;
	if (FAILED(D3D10CompileShader(shaderCode.c_str(), shaderCode.length(), NULL, NULL, NULL, "PShader", "ps_4_0", 0, &pixelBlob, NULL)))
	{
		throw std::exception("LcColoredSpriteRenderDX10(): Cannot compile pixel shader");
	}

	if (FAILED(d3dDevice->CreatePixelShader((DWORD*)pixelBlob->GetBufferPointer(), pixelBlob->GetBufferSize(), &ps)))
	{
		throw std::exception("LcColoredSpriteRenderDX10(): Cannot create pixel shader");
	}

	D3D10_INPUT_ELEMENT_DESC layout[] =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D10_INPUT_PER_VERTEX_DATA, 0},
		{"INDEX", 0, DXGI_FORMAT_R32_SINT, 0, 12, D3D10_INPUT_PER_VERTEX_DATA, 0}
	};

	if (FAILED(d3dDevice->CreateInputLayout(layout, 2, vertexBlob->GetBufferPointer(), vertexBlob->GetBufferSize(), &vertexLayout)))
	{
		throw std::exception("LcColoredSpriteRenderDX10(): Cannot create input layout");
	}

	// create vertex buffer
	D3D10_BUFFER_DESC bufferDesc;
	bufferDesc.Usage = D3D10_USAGE_DYNAMIC;
	bufferDesc.ByteWidth = sizeof(DX10COLOREDSPRITEDATA) * 4;
	bufferDesc.BindFlags = D3D10_BIND_VERTEX_BUFFER;
	bufferDesc.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE;
	bufferDesc.MiscFlags = 0;
	if (FAILED(d3dDevice->CreateBuffer(&bufferDesc, NULL, &vertexBuffer)))
	{
		throw std::exception("LcColoredSpriteRenderDX10(): Cannot create vertex buffer");
	}

	// define constant buffer
	struct VS_COLORS_BUFFER
	{
		LcColor4 colors[4];
	};
	VS_COLORS_BUFFER colorsData;
	colorsData.colors[0] = LcDefaults::White;
	colorsData.colors[1] = LcDefaults::White;
	colorsData.colors[2] = LcDefaults::White;
	colorsData.colors[3] = LcDefaults::White;

	D3D10_BUFFER_DESC cbDesc;
	cbDesc.ByteWidth = sizeof(VS_COLORS_BUFFER);
	cbDesc.Usage = D3D10_USAGE_DEFAULT;
	cbDesc.BindFlags = D3D10_BIND_CONSTANT_BUFFER;
	cbDesc.CPUAccessFlags = 0;
	cbDesc.MiscFlags = 0;

	D3D10_SUBRESOURCE_DATA subResData;
	subResData.pSysMem = &colorsData;
	subResData.SysMemPitch = 0;
	subResData.SysMemSlicePitch = 0;

	if (FAILED(d3dDevice->CreateBuffer(&cbDesc, &subResData, &colorsBuffer)))
	{
		throw std::exception("LcColoredSpriteRenderDX10(): Cannot create constant buffer");
	}

	// slots in ColoredSprite2d.shader: 0 - proj matrix, 1 - trans matrix, 2 - colors
	d3dDevice->VSSetConstantBuffers(2, 1, &colorsBuffer);

	// fill vertex buffer
	DX10COLOREDSPRITEDATA* vertices;
	vertexBuffer->Map(D3D10_MAP_WRITE_DISCARD, 0, (void**)&vertices);
	vertices[0] = DX10COLOREDSPRITEDATA{ LcVector3( 0.5, 0.5, 0), 1 };
	vertices[1] = DX10COLOREDSPRITEDATA{ LcVector3( 0.5,-0.5, 0), 2 };
	vertices[2] = DX10COLOREDSPRITEDATA{ LcVector3(-0.5, 0.5, 0), 0 };
	vertices[3] = DX10COLOREDSPRITEDATA{ LcVector3(-0.5,-0.5, 0), 3 };
	vertexBuffer->Unmap();
}

LcColoredSpriteRenderDX10::~LcColoredSpriteRenderDX10()
{
	if (vertexBuffer) { vertexBuffer->Release(); vertexBuffer = nullptr; }
	if (vertexLayout) { vertexLayout->Release(); vertexLayout = nullptr; }
	if (colorsBuffer) { colorsBuffer->Release(); ps = nullptr; }
	if (vs) { vs->Release(); vs = nullptr; }
	if (ps) { ps->Release(); ps = nullptr; }
}

void LcColoredSpriteRenderDX10::Setup()
{
	auto d3dDevice = renderDevice.GetD3D10Device();
	if (!d3dDevice) throw std::exception("LcColoredSpriteRenderDX10::Setup(): Invalid render device");

	d3dDevice->VSSetShader(vs);
	d3dDevice->PSSetShader(ps);
	d3dDevice->IASetInputLayout(vertexLayout);

	UINT stride = sizeof(DX10COLOREDSPRITEDATA);
	UINT offset = 0;
	d3dDevice->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	d3dDevice->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
}

void LcColoredSpriteRenderDX10::Render(const ISprite* sprite)
{
	auto d3dDevice = renderDevice.GetD3D10Device();
	auto transMatrix = renderDevice.GetTransformBuffer();
	if (!d3dDevice || !transMatrix || !sprite) throw std::exception("LcColoredSpriteRenderDX10::Render(): Invalid render params");

	LcVector2 offset = renderDevice.GetOffset();
	LcVector3 pos(sprite->GetPos().x + offset.x, sprite->GetPos().y + offset.y, sprite->GetPos().z);
	LcMatrix4 trans = TransformMatrix(pos, sprite->GetSize(), sprite->GetRotZ());

	d3dDevice->UpdateSubresource(transMatrix, 0, NULL, &trans, 0, 0);
	d3dDevice->UpdateSubresource(colorsBuffer, 0, NULL, &sprite->GetColors(), 0, 0);
	d3dDevice->Draw(4, 0);
}
