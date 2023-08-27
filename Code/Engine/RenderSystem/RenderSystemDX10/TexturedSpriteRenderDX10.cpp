/**
* TexturedSpriteRenderDX10.cpp
* 25.08.2023
* (c) Denis Romakhov
*/

#include "pch.h"
#include "RenderSystem/RenderSystemDX10/TexturedSpriteRenderDX10.h"
#include "RenderSystem/RenderSystemDX10/RenderSystemDX10.h"


static const char* texturedSpriteShaderName = "TexturedSprite2d.shader";
struct DX10TEXTUREDSPRITEDATA
{
	LcVector3 pos;		// position
	LcVector2 uv;		// uv coords
	unsigned int index;	// vertex index
};

LcTexturedSpriteRenderDX10::LcTexturedSpriteRenderDX10(IRenderDeviceDX10& inRenderDevice) : renderDevice(inRenderDevice)
{
	vs = nullptr;
	ps = nullptr;
	vertexBuffer = nullptr;
	vertexLayout = nullptr;
	auto d3dDevice = renderDevice.GetD3D10Device();
	if (!d3dDevice) throw std::exception("LcTexturedSpriteRenderDX10(): Invalid arguments");

	auto shaderCode = renderDevice.GetShaderCode(texturedSpriteShaderName);

	ComPtr<ID3D10Blob> vertexBlob;
	if (FAILED(D3D10CompileShader(shaderCode.c_str(), shaderCode.length(), NULL, NULL, NULL, "VShader", "vs_4_0", 0, vertexBlob.GetAddressOf(), NULL)))
	{
		throw std::exception("LcTexturedSpriteRenderDX10(): Cannot compile vertex shader");
	}

	if (FAILED(d3dDevice->CreateVertexShader((DWORD*)vertexBlob->GetBufferPointer(), vertexBlob->GetBufferSize(), &vs)))
	{
		throw std::exception("LcTexturedSpriteRenderDX10(): Cannot create vertex shader");
	}

	ComPtr<ID3D10Blob> pixelBlob;
	if (FAILED(D3D10CompileShader(shaderCode.c_str(), shaderCode.length(), NULL, NULL, NULL, "PShader", "ps_4_0", 0, pixelBlob.GetAddressOf(), NULL)))
	{
		throw std::exception("LcTexturedSpriteRenderDX10(): Cannot compile pixel shader");
	}

	if (FAILED(d3dDevice->CreatePixelShader((DWORD*)pixelBlob->GetBufferPointer(), pixelBlob->GetBufferSize(), &ps)))
	{
		throw std::exception("LcTexturedSpriteRenderDX10(): Cannot create pixel shader");
	}

	D3D10_INPUT_ELEMENT_DESC layout[] =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,  D3D10_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,    0, 12, D3D10_INPUT_PER_VERTEX_DATA, 0},
		{"INDEX",    0, DXGI_FORMAT_R32_SINT,        0, 20, D3D10_INPUT_PER_VERTEX_DATA, 0}
	};

	if (FAILED(d3dDevice->CreateInputLayout(layout, 3, vertexBlob->GetBufferPointer(), vertexBlob->GetBufferSize(), &vertexLayout)))
	{
		throw std::exception("LcTexturedSpriteRenderDX10(): Cannot create input layout");
	}

	// create vertex buffer
	D3D10_BUFFER_DESC bufferDesc;
	bufferDesc.Usage = D3D10_USAGE_DYNAMIC;
	bufferDesc.ByteWidth = sizeof(DX10TEXTUREDSPRITEDATA) * 4;
	bufferDesc.BindFlags = D3D10_BIND_VERTEX_BUFFER;
	bufferDesc.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE;
	bufferDesc.MiscFlags = 0;
	if (FAILED(d3dDevice->CreateBuffer(&bufferDesc, NULL, &vertexBuffer)))
	{
		throw std::exception("LcTexturedSpriteRenderDX10(): Cannot create vertex buffer");
	}

	// fill vertex buffer
	DX10TEXTUREDSPRITEDATA* vertices;
	vertexBuffer->Map(D3D10_MAP_WRITE_DISCARD, 0, (void**)&vertices);
	vertices[0] = DX10TEXTUREDSPRITEDATA{ LcVector3( 0.5, 0.5, 0), LcVector2(1.0, 0.0), 1 };
	vertices[1] = DX10TEXTUREDSPRITEDATA{ LcVector3( 0.5,-0.5, 0), LcVector2(1.0, 1.0), 2 };
	vertices[2] = DX10TEXTUREDSPRITEDATA{ LcVector3(-0.5, 0.5, 0), LcVector2(0.0, 0.0), 0 };
	vertices[3] = DX10TEXTUREDSPRITEDATA{ LcVector3(-0.5,-0.5, 0), LcVector2(0.0, 1.0), 3 };
	vertexBuffer->Unmap();
}

LcTexturedSpriteRenderDX10::~LcTexturedSpriteRenderDX10()
{
	if (vertexBuffer) { vertexBuffer->Release(); vertexBuffer = nullptr; }
	if (vertexLayout) { vertexLayout->Release(); vertexLayout = nullptr; }
	if (vs) { vs->Release(); vs = nullptr; }
	if (ps) { ps->Release(); ps = nullptr; }
}

void LcTexturedSpriteRenderDX10::Setup()
{
	auto d3dDevice = renderDevice.GetD3D10Device();
	if (!d3dDevice) throw std::exception("LcTexturedSpriteRenderDX10::Setup(): Invalid render device");

	d3dDevice->VSSetShader(vs);
	d3dDevice->PSSetShader(ps);
	d3dDevice->IASetInputLayout(vertexLayout);

	UINT stride = sizeof(DX10TEXTUREDSPRITEDATA);
	UINT offset = 0;
	d3dDevice->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	d3dDevice->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
}

void LcTexturedSpriteRenderDX10::Render(const ISprite* sprite)
{
	auto d3dDevice = renderDevice.GetD3D10Device();
	auto transBuffer = renderDevice.GetTransformBuffer();
	auto colorsBuffer = renderDevice.GetColorsBuffer();
	if (!d3dDevice || !transBuffer || !colorsBuffer || !sprite) throw std::exception("LcTexturedSpriteRenderDX10::Render(): Invalid render params");

	// update components
	auto colors = sprite->GetColorsComponent();
	auto tint = sprite->GetTintComponent();
	if (colors || tint)
	{
		auto colorsData = colors ? colors->GetData() : tint->GetData();
		d3dDevice->UpdateSubresource(colorsBuffer, 0, NULL, colorsData, 0, 0);
	}
	else
	{
		static LcColor4 defaultColors[] = { LcDefaults::White4, LcDefaults::White4, LcDefaults::White4, LcDefaults::White4 };
		d3dDevice->UpdateSubresource(colorsBuffer, 0, NULL, defaultColors, 0, 0);
	}

	if (sprite->HasComponent(EVCType::Texture))
	{
		const LcSpriteDX10* spriteDX10 = (LcSpriteDX10*)sprite;
		d3dDevice->PSSetShaderResources(0, 1, &spriteDX10->shaderView);
	}

	// update transform
	LcVector2 offset = renderDevice.GetOffset();
	LcVector3 pos(sprite->GetPos().x + offset.x, sprite->GetPos().y + offset.y, sprite->GetPos().z);
	LcMatrix4 trans = TransformMatrix(pos, sprite->GetSize(), sprite->GetRotZ());

	d3dDevice->UpdateSubresource(transBuffer, 0, NULL, &trans, 0, 0);

	// render sprite
	d3dDevice->Draw(4, 0);
}

bool LcTexturedSpriteRenderDX10::Supports(const TVFeaturesList& features) const
{
	bool needTexture = false, needAnimation = false;
	for (auto& feature : features)
	{
		needTexture |= (feature == EVCType::Texture);
		needAnimation |= (feature == EVCType::FrameAnimation);
	}
	return !needAnimation && needTexture;
}
