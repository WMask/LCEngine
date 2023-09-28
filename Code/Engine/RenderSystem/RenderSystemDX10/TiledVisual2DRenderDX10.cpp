/**
* TiledVisual2DRenderDX10.cpp
* 06.09.2023
* (c) Denis Romakhov
*/

#include "pch.h"
#include "RenderSystem/RenderSystemDX10/TiledVisual2DRenderDX10.h"
#include "RenderSystem/RenderSystemDX10/RenderSystemDX10.h"
#include "RenderSystem/RenderSystemDX10/VisualsDX10.h"


static const char* tiledSpriteShaderName = "TiledSprite2d.shader";
struct DX10TILEDSPRITEDATA
{
	LcVector3 pos;		// position
	LcVector2 uv;		// UV texture coordinates
};

LcTiledVisual2DRenderDX10::LcTiledVisual2DRenderDX10(const LcAppContext& context)
{
	vs = nullptr;
	ps = nullptr;
	vertexLayout = nullptr;

	auto render = static_cast<LcRenderSystemDX10*>(context.render);
	auto d3dDevice = render ? render->GetD3D10Device() : nullptr;
	if (!d3dDevice) throw std::exception("LcTiledVisual2DRenderDX10(): Invalid arguments");

	auto shaderCode = render->GetShaderCode(tiledSpriteShaderName);

	ComPtr<ID3D10Blob> vertexBlob;
	if (FAILED(D3D10CompileShader(shaderCode.c_str(), shaderCode.length(), NULL, NULL, NULL, "VShader", "vs_4_0", 0, vertexBlob.GetAddressOf(), NULL)))
	{
		throw std::exception("LcTiledVisual2DRenderDX10(): Cannot compile vertex shader");
	}

	if (FAILED(d3dDevice->CreateVertexShader((DWORD*)vertexBlob->GetBufferPointer(), vertexBlob->GetBufferSize(), &vs)))
	{
		throw std::exception("LcTiledVisual2DRenderDX10(): Cannot create vertex shader");
	}

	ComPtr<ID3D10Blob> pixelBlob;
	if (FAILED(D3D10CompileShader(shaderCode.c_str(), shaderCode.length(), NULL, NULL, NULL, "PShader", "ps_4_0", 0, pixelBlob.GetAddressOf(), NULL)))
	{
		throw std::exception("LcTiledVisual2DRenderDX10(): Cannot compile pixel shader");
	}

	if (FAILED(d3dDevice->CreatePixelShader((DWORD*)pixelBlob->GetBufferPointer(), pixelBlob->GetBufferSize(), &ps)))
	{
		throw std::exception("LcTiledVisual2DRenderDX10(): Cannot create pixel shader");
	}

	D3D10_INPUT_ELEMENT_DESC layout[] =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,  D3D10_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,    0, 12, D3D10_INPUT_PER_VERTEX_DATA, 0}
	};

	if (FAILED(d3dDevice->CreateInputLayout(layout, 2, vertexBlob->GetBufferPointer(), vertexBlob->GetBufferSize(), &vertexLayout)))
	{
		throw std::exception("LcTiledVisual2DRenderDX10(): Cannot create input layout");
	}
}

LcTiledVisual2DRenderDX10::~LcTiledVisual2DRenderDX10()
{
	vertexBuffers.clear();

	if (vertexLayout) { vertexLayout->Release(); vertexLayout = nullptr; }
	if (vs) { vs->Release(); vs = nullptr; }
	if (ps) { ps->Release(); ps = nullptr; }
}

void LcTiledVisual2DRenderDX10::RemoveTiles(const IVisual* visual)
{
	vertexBuffers.erase(visual);
}

std::vector<DX10TILEDSPRITEDATA> GenerateTiles(const LcTiledSpriteComponent& tiledComp)
{
	std::vector<DX10TILEDSPRITEDATA> tiles;
	tiles.reserve(tiledComp.GetTilesData().size() * 6);

	for (const auto& tile : tiledComp.GetTilesData())
	{
		tiles.emplace_back(DX10TILEDSPRITEDATA{ tile.pos[0], tile.uv[0] });
		tiles.emplace_back(DX10TILEDSPRITEDATA{ tile.pos[1], tile.uv[1] });
		tiles.emplace_back(DX10TILEDSPRITEDATA{ tile.pos[2], tile.uv[2] });
		tiles.emplace_back(DX10TILEDSPRITEDATA{ tile.pos[0], tile.uv[0] });
		tiles.emplace_back(DX10TILEDSPRITEDATA{ tile.pos[3], tile.uv[3] });
		tiles.emplace_back(DX10TILEDSPRITEDATA{ tile.pos[1], tile.uv[1] });
	}

	return tiles;
}

void LcTiledVisual2DRenderDX10::Setup(const IVisual* visual, const LcAppContext& context)
{
	auto render = static_cast<LcRenderSystemDX10*>(context.render);
	auto d3dDevice = render ? render->GetD3D10Device() : nullptr;
	if (!d3dDevice) throw std::exception("LcTiledVisual2DRenderDX10::Setup(): Invalid render device");

	auto tiledComp = static_cast<LcTiledSpriteComponent*>(visual ? visual->GetComponent(EVCType::Tiled).get() : nullptr);
	if (!tiledComp) throw std::exception("LcTiledVisual2DRenderDX10::Setup(): No Tiled component found");

	auto vbIt = vertexBuffers.find(visual);
	if (vbIt == vertexBuffers.end())
	{
		// create vertex buffer
		auto tilesData = GenerateTiles(*tiledComp);
		auto& vertexBuffer = vertexBuffers[visual];
		vertexBuffer.vertexCount = (int)tilesData.size();

		D3D10_BUFFER_DESC bufferDesc;
		bufferDesc.Usage = D3D10_USAGE_DYNAMIC;
		bufferDesc.ByteWidth = sizeof(DX10TILEDSPRITEDATA) * vertexBuffer.vertexCount;
		bufferDesc.BindFlags = D3D10_BIND_VERTEX_BUFFER;
		bufferDesc.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE;
		bufferDesc.MiscFlags = 0;
		if (FAILED(d3dDevice->CreateBuffer(&bufferDesc, NULL, vertexBuffer.buffer.GetAddressOf())))
		{
			throw std::exception("LcTiledVisual2DRenderDX10::Setup(): Cannot create vertex buffer");
		}

		DX10TILEDSPRITEDATA* vertices;
		vertexBuffer.buffer->Map(D3D10_MAP_WRITE_DISCARD, 0, (void**)&vertices);
		memcpy(vertices, &tilesData[0], sizeof(DX10TILEDSPRITEDATA) * vertexBuffer.vertexCount);
		vertexBuffer.buffer->Unmap();

		vbIt = vertexBuffers.find(visual);
	}

	d3dDevice->VSSetShader(vs);
	d3dDevice->PSSetShader(ps);
	d3dDevice->IASetInputLayout(vertexLayout);

	UINT stride = sizeof(DX10TILEDSPRITEDATA);
	UINT offset = 0;
	d3dDevice->IASetVertexBuffers(0, 1, vbIt->second.buffer.GetAddressOf(), &stride, &offset);
	d3dDevice->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// force setup because next sprite anyway will have another vertex buffer
	render->ForceRenderSetup();
}

void LcTiledVisual2DRenderDX10::Render(const IVisual* visual, const LcAppContext& context)
{
	auto render = static_cast<LcRenderSystemDX10*>(context.render);
	auto d3dDevice = render ? render->GetD3D10Device() : nullptr;
	auto transBuffer = render ? render->GetBuffers().transMatrixBuffer.Get() : nullptr;
	auto sprite = (visual->GetTypeId() == LcCreatables::Sprite) ? static_cast<const ISprite*>(visual) : nullptr;
	if (!d3dDevice || !transBuffer || !sprite)
	{
		throw std::exception("LcTiledVisual2DRenderDX10::Render(): Invalid render params");
	}

	auto vbIt = vertexBuffers.find(sprite);
	if (vbIt == vertexBuffers.end()) throw std::exception("LcTiledVisual2DRenderDX10::Render(): No vertex buffer found");

	// update components
	if (sprite->HasComponent(EVCType::Texture))
	{
		auto spriteDX10 = static_cast<const LcSpriteDX10*>(sprite);
		d3dDevice->PSSetShaderResources(0, 1, (ID3D10ShaderResourceView**)&spriteDX10->textureSV);
	}

	// update transform
	LcVector2 worldScale2D(context.world->GetWorldScale().GetScale());
	LcVector3 worldScale(worldScale2D.x, worldScale2D.y, 1.0f);
	LcVector3 spritePos = sprite->GetPos() * worldScale;
	LcVector2 spriteSize = sprite->GetSize() * worldScale2D;
	if (auto tiledComp = sprite->GetTiledComponent()) spriteSize = tiledComp->GetTilesScale() * To2(worldScale);

	LcMatrix4 trans = TransformMatrix(spritePos, spriteSize, 0.0f, false);
	d3dDevice->UpdateSubresource(transBuffer, 0, NULL, &trans, 0, 0);

	// render sprite
	d3dDevice->Draw(vbIt->second.vertexCount, 0);
}

bool LcTiledVisual2DRenderDX10::Supports(const TVFeaturesList& features) const
{
	bool needTexture = false, needAnimation = false, needTiles = false;
	for (auto& feature : features)
	{
		needTexture |= (feature == EVCType::Texture);
		needAnimation |= (feature == EVCType::FrameAnimation);
		needTiles |= (feature == EVCType::Tiled);
	}
	return !needAnimation && needTexture && needTiles;
}
