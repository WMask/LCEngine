/**
* TexturedVisual2DRenderDX10.cpp
* 25.08.2023
* (c) Denis Romakhov
*/

#include "pch.h"
#include "RenderSystem/RenderSystemDX10/TexturedVisual2DRenderDX10.h"
#include "RenderSystem/RenderSystemDX10/RenderSystemDX10.h"
#include "RenderSystem/RenderSystemDX10/VisualsDX10.h"


static const char* texturedSpriteShaderName = "TexturedSprite2d.shader";
struct DX10TEXTUREDSPRITEDATA
{
	LcVector3 pos;		// position
	unsigned int index;	// vertex index
};

LcTexturedVisual2DRenderDX10::LcTexturedVisual2DRenderDX10(const LcAppContext& context)
{
	vs = nullptr;
	ps = nullptr;
	vertexBuffer = nullptr;
	vertexLayout = nullptr;

	auto render = static_cast<LcRenderSystemDX10*>(context.render);
	auto d3dDevice = render ? render->GetD3D10Device() : nullptr;
	if (!d3dDevice) throw std::exception("LcTexturedVisual2DRenderDX10(): Invalid arguments");

	auto shaderCode = render->GetShaderCode(texturedSpriteShaderName);

	ComPtr<ID3D10Blob> vertexBlob;
	if (FAILED(D3D10CompileShader(shaderCode.c_str(), shaderCode.length(), NULL, NULL, NULL, "VShader", "vs_4_0", 0, vertexBlob.GetAddressOf(), NULL)))
	{
		throw std::exception("LcTexturedVisual2DRenderDX10(): Cannot compile vertex shader");
	}

	if (FAILED(d3dDevice->CreateVertexShader((DWORD*)vertexBlob->GetBufferPointer(), vertexBlob->GetBufferSize(), &vs)))
	{
		throw std::exception("LcTexturedVisual2DRenderDX10(): Cannot create vertex shader");
	}

	ComPtr<ID3D10Blob> pixelBlob;
	if (FAILED(D3D10CompileShader(shaderCode.c_str(), shaderCode.length(), NULL, NULL, NULL, "PShader", "ps_4_0", 0, pixelBlob.GetAddressOf(), NULL)))
	{
		throw std::exception("LcTexturedVisual2DRenderDX10(): Cannot compile pixel shader");
	}

	if (FAILED(d3dDevice->CreatePixelShader((DWORD*)pixelBlob->GetBufferPointer(), pixelBlob->GetBufferSize(), &ps)))
	{
		throw std::exception("LcTexturedVisual2DRenderDX10(): Cannot create pixel shader");
	}

	D3D10_INPUT_ELEMENT_DESC layout[] =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,  D3D10_INPUT_PER_VERTEX_DATA, 0},
		{"INDEX",    0, DXGI_FORMAT_R32_SINT,        0, 12, D3D10_INPUT_PER_VERTEX_DATA, 0}
	};

	if (FAILED(d3dDevice->CreateInputLayout(layout, 2, vertexBlob->GetBufferPointer(), vertexBlob->GetBufferSize(), &vertexLayout)))
	{
		throw std::exception("LcTexturedVisual2DRenderDX10(): Cannot create input layout");
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
		throw std::exception("LcTexturedVisual2DRenderDX10(): Cannot create vertex buffer");
	}

	// fill vertex buffer
	DX10TEXTUREDSPRITEDATA* vertices;
	vertexBuffer->Map(D3D10_MAP_WRITE_DISCARD, 0, (void**)&vertices);
	vertices[0] = DX10TEXTUREDSPRITEDATA{ LcVector3( 0.5, 0.5, 0), 1 };
	vertices[1] = DX10TEXTUREDSPRITEDATA{ LcVector3( 0.5,-0.5, 0), 2 };
	vertices[2] = DX10TEXTUREDSPRITEDATA{ LcVector3(-0.5, 0.5, 0), 0 };
	vertices[3] = DX10TEXTUREDSPRITEDATA{ LcVector3(-0.5,-0.5, 0), 3 };
	vertexBuffer->Unmap();
}

LcTexturedVisual2DRenderDX10::~LcTexturedVisual2DRenderDX10()
{
	if (vertexBuffer) { vertexBuffer->Release(); vertexBuffer = nullptr; }
	if (vertexLayout) { vertexLayout->Release(); vertexLayout = nullptr; }
	if (vs) { vs->Release(); vs = nullptr; }
	if (ps) { ps->Release(); ps = nullptr; }
}

void LcTexturedVisual2DRenderDX10::Setup(const IVisual* visual, const LcAppContext& context)
{
	auto render = static_cast<LcRenderSystemDX10*>(context.render);
	auto d3dDevice = render ? render->GetD3D10Device() : nullptr;
	if (!d3dDevice) throw std::exception("LcTexturedVisual2DRenderDX10::Setup(): Invalid render device");

	d3dDevice->VSSetShader(vs);
	d3dDevice->PSSetShader(ps);
	d3dDevice->IASetInputLayout(vertexLayout);

	UINT stride = sizeof(DX10TEXTUREDSPRITEDATA);
	UINT offset = 0;
	d3dDevice->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	d3dDevice->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
}

void LcTexturedVisual2DRenderDX10::RenderSprite(const ISprite* sprite, const LcAppContext& context)
{
	auto render = static_cast<LcRenderSystemDX10*>(context.render);
	auto d3dDevice = render ? render->GetD3D10Device() : nullptr;
	auto transBuffer = render ? render->GetTransformBuffer() : nullptr;
	auto colorsBuffer = render ? render->GetColorsBuffer() : nullptr;
	auto uvsBuffer = render ? render->GetCustomUvBuffer() : nullptr;
	if (!d3dDevice || !transBuffer || !colorsBuffer || !uvsBuffer || !sprite)
		throw std::exception("LcTexturedVisual2DRenderDX10::RenderSprite(): Invalid render params");

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

	if (auto customUV = sprite->GetCustomUVComponent())
	{
		d3dDevice->UpdateSubresource(uvsBuffer, 0, NULL, customUV->GetData(), 0, 0);
	}
	else
	{
		static LcVector4 defaultUVs[] = { To4(LcVector2(0.0, 0.0)), To4(LcVector2(1.0, 0.0)), To4(LcVector2(1.0, 1.0)), To4(LcVector2(0.0, 1.0)) };
		d3dDevice->UpdateSubresource(uvsBuffer, 0, NULL, defaultUVs, 0, 0);
	}

	if (sprite->HasComponent(EVCType::Texture))
	{
		const LcSpriteDX10* spriteDX10 = (LcSpriteDX10*)sprite;
		d3dDevice->PSSetShaderResources(0, 1, (ID3D10ShaderResourceView**)spriteDX10->shaderView.GetAddressOf());
	}

	// update transform
	LcVector2 worldScale2D(context.world.GetWorldScale().scale);
	LcVector3 worldScale(worldScale2D.x, worldScale2D.y, 1.0f);
	LcVector3 spritePos = sprite->GetPos() * worldScale;
	LcVector2 spriteSize = sprite->GetSize() * worldScale2D;
	LcMatrix4 trans = TransformMatrix(spritePos, spriteSize, sprite->GetRotZ());
	d3dDevice->UpdateSubresource(transBuffer, 0, NULL, &trans, 0, 0);

	// render sprite
	d3dDevice->Draw(4, 0);
}

void LcTexturedVisual2DRenderDX10::RenderWidget(const IWidget* widget, const LcAppContext& context)
{
	auto render = static_cast<LcRenderSystemDX10*>(context.render);
	auto d3dDevice = render ? render->GetD3D10Device() : nullptr;
	auto transBuffer = render ? render->GetTransformBuffer() : nullptr;
	auto colorsBuffer = render ? render->GetColorsBuffer() : nullptr;
	auto uvsBuffer = render ? render->GetCustomUvBuffer() : nullptr;
	if (!d3dDevice || !transBuffer || !colorsBuffer || !uvsBuffer || !widget)
		throw std::exception("LcTexturedVisual2DRenderDX10::RenderWidget(): Invalid render params");

	// update components
	static LcColor4 defaultColors[] = { LcDefaults::White4, LcDefaults::White4, LcDefaults::White4, LcDefaults::White4 };
	d3dDevice->UpdateSubresource(colorsBuffer, 0, NULL, defaultColors, 0, 0);

	if (auto customUV = widget->GetButtonComponent())
	{
		d3dDevice->UpdateSubresource(uvsBuffer, 0, NULL, customUV->GetData(), 0, 0);
	}
	else
	if (auto customUV = widget->GetCheckboxComponent())
	{
		d3dDevice->UpdateSubresource(uvsBuffer, 0, NULL, customUV->GetData(), 0, 0);
	}

	auto widgetDX10 = static_cast<const LcWidgetDX10*>(widget);
	if (widget->HasComponent(EVCType::Texture))
	{
		d3dDevice->PSSetShaderResources(0, 1, (ID3D10ShaderResourceView**)widgetDX10->spriteTextureSV.GetAddressOf());
	}
	else
	{
		ID3D10ShaderResourceView* nullSRV[1] = { nullptr };
		d3dDevice->PSSetShaderResources(0, 1, nullSRV);
	}

	// update transform
	LcVector2 worldScale2D(context.world.GetWorldScale().scale);
	LcVector3 worldScale(worldScale2D.x, worldScale2D.y, 1.0f);
	LcVector3 widgetPos = widget->GetPos() * worldScale;
	LcVector2 widgetSize = widget->GetSize() * worldScale2D;
	LcMatrix4 trans = TransformMatrix(widgetPos, widgetSize);
	d3dDevice->UpdateSubresource(transBuffer, 0, NULL, &trans, 0, 0);

	// render sprite
	d3dDevice->Draw(4, 0);

	if (widgetDX10->textTextureSV)
	{
		// set text texture
		d3dDevice->PSSetShaderResources(0, 1, (ID3D10ShaderResourceView**)widgetDX10->textTextureSV.GetAddressOf());

		static LcVector4 defaultUVs[] = { To4(LcVector2(0.0, 0.0)), To4(LcVector2(1.0, 0.0)), To4(LcVector2(1.0, 1.0)), To4(LcVector2(0.0, 1.0)) };
		d3dDevice->UpdateSubresource(uvsBuffer, 0, NULL, defaultUVs, 0, 0);

		// move to the front of sprite
		trans.r[2].m128_f32[3] = widget->GetPos().z + 0.01f;
		d3dDevice->UpdateSubresource(transBuffer, 0, NULL, &trans, 0, 0);

		// render text texture
		d3dDevice->Draw(4, 0);
	}
}

bool LcTexturedVisual2DRenderDX10::Supports(const TVFeaturesList& features) const
{
	bool needTexture = false, needAnimation = false, needTiles = false;
	for (auto& feature : features)
	{
		needTexture |= (feature == EVCType::Texture);
		needAnimation |= (feature == EVCType::FrameAnimation);
		needTiles |= (feature == EVCType::Tiled);
	}
	return !needAnimation && !needTiles && needTexture;
}
