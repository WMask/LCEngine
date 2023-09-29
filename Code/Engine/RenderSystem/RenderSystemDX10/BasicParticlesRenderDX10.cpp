/**
* BasicParticlesRenderDX10.cpp
* 29.09.2023
* (c) Denis Romakhov
*/

#include "pch.h"
#include "RenderSystem/RenderSystemDX10/BasicParticlesRenderDX10.h"
#include "RenderSystem/RenderSystemDX10/RenderSystemDX10.h"
#include "RenderSystem/RenderSystemDX10/VisualsDX10.h"
#include "Core/LcUtils.h"


static const char* basicParticlesShaderName = "BasicParticles2d.shader";
struct DX10PARTICLESDATA
{
	LcVector4 pos;	// position
	LcVector4 uv;	// UV texture coordinates + fadeIn, fadeOut rate
	int id;			// movement type
};

LcBasicParticlesRenderDX10::LcBasicParticlesRenderDX10(const LcAppContext& context)
{
	vs = nullptr;
	ps = nullptr;
	vertexLayout = nullptr;

	auto render = static_cast<LcRenderSystemDX10*>(context.render);
	auto d3dDevice = render ? render->GetD3D10Device() : nullptr;
	if (!d3dDevice) throw std::exception("LcBasicParticlesRenderDX10(): Invalid arguments");

	auto shaderCode = render->GetShaderCode(basicParticlesShaderName);

	ComPtr<ID3D10Blob> vertexBlob;
	if (FAILED(D3D10CompileShader(shaderCode.c_str(), shaderCode.length(), NULL, NULL, NULL, "VShader", "vs_4_0", 0, vertexBlob.GetAddressOf(), NULL)))
	{
		throw std::exception("LcBasicParticlesRenderDX10(): Cannot compile vertex shader");
	}

	if (FAILED(d3dDevice->CreateVertexShader((DWORD*)vertexBlob->GetBufferPointer(), vertexBlob->GetBufferSize(), &vs)))
	{
		throw std::exception("LcBasicParticlesRenderDX10(): Cannot create vertex shader");
	}

	ComPtr<ID3D10Blob> pixelBlob;
	if (FAILED(D3D10CompileShader(shaderCode.c_str(), shaderCode.length(), NULL, NULL, NULL, "PShader", "ps_4_0", 0, pixelBlob.GetAddressOf(), NULL)))
	{
		throw std::exception("LcBasicParticlesRenderDX10(): Cannot compile pixel shader");
	}

	if (FAILED(d3dDevice->CreatePixelShader((DWORD*)pixelBlob->GetBufferPointer(), pixelBlob->GetBufferSize(), &ps)))
	{
		throw std::exception("LcBasicParticlesRenderDX10(): Cannot create pixel shader");
	}

	D3D10_INPUT_ELEMENT_DESC layout[] =
	{
		{"POSITION",     0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0,  D3D10_INPUT_PER_VERTEX_DATA, 0},
		{"POSITION",     1, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 16, D3D10_INPUT_PER_VERTEX_DATA, 0},
		{"BLENDINDICES", 0, DXGI_FORMAT_R32_UINT,           0, 32, D3D10_INPUT_PER_VERTEX_DATA, 0}
	};

	if (FAILED(d3dDevice->CreateInputLayout(layout, 3, vertexBlob->GetBufferPointer(), vertexBlob->GetBufferSize(), &vertexLayout)))
	{
		throw std::exception("LcBasicParticlesRenderDX10(): Cannot create input layout");
	}
}

LcBasicParticlesRenderDX10::~LcBasicParticlesRenderDX10()
{
	vertexBuffers.clear();

	if (vertexLayout) { vertexLayout->Release(); vertexLayout = nullptr; }
	if (vs) { vs->Release(); vs = nullptr; }
	if (ps) { ps->Release(); ps = nullptr; }
}

void LcBasicParticlesRenderDX10::RemoveTiles(const IVisual* visual)
{
	vertexBuffers.erase(visual);
}

std::vector<DX10PARTICLESDATA> GenerateParticles(
	const IVisualTextureComponent& textureComp,
	const IBasicParticlesComponent& particlesComp,
	const IVisual& visual)
{
	std::vector<DX10PARTICLESDATA> particles;
	particles.reserve(particlesComp.GetNumParticles() * 6);

	const auto& settings = particlesComp.GetSettings();
	const float offset = 1.0f;
	const float offsetX = visual.GetSize().x / -2.0f;
	const float offsetY = visual.GetSize().y / -2.0f;
	const float sizeX = settings.frameSize.x;
	const float sizeY = settings.frameSize.y;
	const float halfSizeX = sizeX / 2.0f;
	const float halfSizeY = sizeY / 2.0f;
	const float minX = (sizeX / 2.0f + offset);
	const float minY = (sizeY / 2.0f + offset);
	const float maxX = (visual.GetSize().x - sizeX / 2.0f - 2.0f * offset);
	const float maxY = (visual.GetSize().y - sizeY / 2.0f - 2.0f * offset);
	const float rangeX = maxX - minX;
	const float rangeY = maxY - minY;

	int uvColumns = int(textureComp.GetTextureSize().x / sizeX);
	int uvRows = int(textureComp.GetTextureSize().y / sizeY);
	float uvx = sizeX / textureComp.GetTextureSize().x;
	float uvy = sizeY / textureComp.GetTextureSize().y;

	for (int i = 0; i < particlesComp.GetNumParticles(); i++)
	{
		float x = offsetX + minX + RandHelper() * rangeX;
		float y = offsetY + minY + RandHelper() * rangeY;
		float z = (-0.01f * particlesComp.GetNumParticles()) + 0.01f * i;

		int id = RandHelper(0, 3);
		int uvTileId = RandHelper(0, uvColumns * uvRows - 1);
		float time = RandHelper() * settings.lifetime;

		LcVector4 pos[4] = {
			{x - halfSizeX, y - halfSizeY, z, time},
			{x + halfSizeX, y + halfSizeY, z, time},
			{x + halfSizeX, y - halfSizeY, z, time},
			{x - halfSizeX, y + halfSizeY, z, time},
		};

		int uvRow = uvTileId / uvColumns;
		int uvColumn = uvTileId % uvColumns;
		float ox = uvx * uvColumn;
		float oy = uvy * uvRow;

		LcVector4 uv[4] = {
			{ox, oy, settings.fadeInRate, settings.fadeOutRate},
			{ox + uvx, oy + uvy, settings.fadeInRate, settings.fadeOutRate},
			{ox + uvx, oy, settings.fadeInRate, settings.fadeOutRate},
			{ox, oy + uvy, settings.fadeInRate, settings.fadeOutRate},
		};

		particles.push_back(DX10PARTICLESDATA{ pos[0], uv[0], id });
		particles.push_back(DX10PARTICLESDATA{ pos[1], uv[1], id });
		particles.push_back(DX10PARTICLESDATA{ pos[2], uv[2], id });
		particles.push_back(DX10PARTICLESDATA{ pos[0], uv[0], id });
		particles.push_back(DX10PARTICLESDATA{ pos[3], uv[3], id });
		particles.push_back(DX10PARTICLESDATA{ pos[1], uv[1], id });
	}

	return particles;
}

void LcBasicParticlesRenderDX10::Setup(const IVisual* visual, const LcAppContext& context)
{
	auto render = static_cast<LcRenderSystemDX10*>(context.render);
	auto d3dDevice = render ? render->GetD3D10Device() : nullptr;
	if (!d3dDevice) throw std::exception("LcBasicParticlesRenderDX10::Setup(): Invalid render device");

	auto particlesComp = static_cast<IBasicParticlesComponent*>(visual ? visual->GetComponent(EVCType::Particles).get() : nullptr);
	auto textureComp = static_cast<IVisualTextureComponent*>(visual ? visual->GetComponent(EVCType::Texture).get() : nullptr);
	if (!particlesComp || !textureComp) throw std::exception("LcBasicParticlesRenderDX10::Setup(): Invalid components");

	auto vbIt = vertexBuffers.find(visual);
	if (vbIt == vertexBuffers.end())
	{
		// create vertex buffer
		auto particlesData = GenerateParticles(*textureComp, *particlesComp, *visual);
		auto& vertexBuffer = vertexBuffers[visual];
		vertexBuffer.vertexCount = (int)particlesData.size();
		vertexBuffer.creationTime = context.gameTime;

		D3D10_BUFFER_DESC bufferDesc;
		bufferDesc.Usage = D3D10_USAGE_DYNAMIC;
		bufferDesc.ByteWidth = sizeof(DX10PARTICLESDATA) * vertexBuffer.vertexCount;
		bufferDesc.BindFlags = D3D10_BIND_VERTEX_BUFFER;
		bufferDesc.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE;
		bufferDesc.MiscFlags = 0;
		if (FAILED(d3dDevice->CreateBuffer(&bufferDesc, NULL, vertexBuffer.buffer.GetAddressOf())))
		{
			throw std::exception("LcBasicParticlesRenderDX10::Setup(): Cannot create vertex buffer");
		}

		DX10PARTICLESDATA* vertices;
		vertexBuffer.buffer->Map(D3D10_MAP_WRITE_DISCARD, 0, (void**)&vertices);
		memcpy(vertices, &particlesData[0], sizeof(DX10PARTICLESDATA) * vertexBuffer.vertexCount);
		vertexBuffer.buffer->Unmap();

		vbIt = vertexBuffers.find(visual);
	}

	d3dDevice->VSSetShader(vs);
	d3dDevice->PSSetShader(ps);
	d3dDevice->IASetInputLayout(vertexLayout);

	UINT stride = sizeof(DX10PARTICLESDATA);
	UINT offset = 0;
	d3dDevice->IASetVertexBuffers(0, 1, vbIt->second.buffer.GetAddressOf(), &stride, &offset);
	d3dDevice->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// force setup because next sprite anyway will have another vertex buffer
	render->ForceRenderSetup();
}

void LcBasicParticlesRenderDX10::Render(const IVisual* visual, const LcAppContext& context)
{
	auto render = static_cast<LcRenderSystemDX10*>(context.render);
	auto d3dDevice = render ? render->GetD3D10Device() : nullptr;
	auto transBuffer = render ? render->GetBuffers().transMatrixBuffer.Get() : nullptr;
	auto animBuffer = render ? render->GetBuffers().frameAnimBuffer.Get() : nullptr;
	auto sprite = (visual->GetTypeId() == LcCreatables::Sprite) ? static_cast<const ISprite*>(visual) : nullptr;
	if (!d3dDevice || !animBuffer || !transBuffer || !sprite)
	{
		throw std::exception("LcBasicParticlesRenderDX10::Render(): Invalid render params");
	}

	auto vbIt = vertexBuffers.find(sprite);
	if (vbIt == vertexBuffers.end()) throw std::exception("LcBasicParticlesRenderDX10::Render(): No vertex buffer found");

	// update components
	if (auto particles = sprite->GetParticlesComponent())
	{
		const auto& settings = particles->GetSettings();
		LcVector4 animData = {
			settings.speed,
			settings.lifetime,
			settings.movementRadius,
			context.gameTime - vbIt->second.creationTime
		};
		d3dDevice->UpdateSubresource(animBuffer, 0, NULL, &animData, 0, 0);
	}

	if (sprite->HasComponent(EVCType::Texture))
	{
		auto spriteDX10 = static_cast<const LcSpriteDX10*>(sprite);
		d3dDevice->PSSetShaderResources(0, 1, (ID3D10ShaderResourceView**)&spriteDX10->textureSV);
	}

	// update transform
	LcVector2 worldScale2D(context.world->GetWorldScale().GetScale());
	LcVector3 worldScale(worldScale2D.x, worldScale2D.y, 1.0f);
	LcVector3 spritePos = sprite->GetPos() * worldScale;

	LcMatrix4 trans = TransformMatrix(spritePos, worldScale2D, 0.0f, false);
	d3dDevice->UpdateSubresource(transBuffer, 0, NULL, &trans, 0, 0);

	// render sprite
	d3dDevice->Draw(vbIt->second.vertexCount, 0);
}

bool LcBasicParticlesRenderDX10::Supports(const TVFeaturesList& features) const
{
	bool needTexture = false, needAnimation = false, needTiles = false, needBasicParticles = false;
	for (auto& feature : features)
	{
		needTexture |= (feature == EVCType::Texture);
		needAnimation |= (feature == EVCType::FrameAnimation);
		needTiles |= (feature == EVCType::Tiled);
		needBasicParticles |= (feature == EVCType::Particles);
	}
	return !needAnimation && !needTiles && needTexture && needBasicParticles;
}
