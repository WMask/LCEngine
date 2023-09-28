/**
* Sprites.cpp
* 26.08.2023
* (c) Denis Romakhov
*/

#pragma once

#include "pch.h"
#include "World/Sprites.h"
#include "Core/LCException.h"
#include "Core/LCUtils.h"

#include <filesystem>

// put nlohmann's json lib in Code/Json folder
#include "nlohmann/json.hpp"

using json = nlohmann::json;


void LcSpriteHelper::AddCustomUVComponent(LcVector2 inLeftTop, LcVector2 inRightTop, LcVector2 inRightBottom, LcVector2 inLeftBottom) const
{
	if (auto visual = context.world->GetLastAddedVisual())
	{
		visual->AddComponent(std::make_shared<LcSpriteCustomUVComponent>(inLeftTop, inRightTop, inRightBottom, inLeftBottom), context);
	}
}

void LcSpriteHelper::AddAnimationComponent(LcSizef inFrameSize, unsigned short inNumFrames, float inFramesPerSecond) const
{
	if (auto visual = context.world->GetLastAddedVisual())
	{
		visual->AddComponent(std::make_shared<LcSpriteAnimationComponent>(inFrameSize, inNumFrames, inFramesPerSecond), context);
	}
}

void LcSpriteHelper::AddTiledComponent(const std::string& tiledJsonPath, const LcLayersList& inLayerNames) const
{
	if (auto visual = context.world->GetLastAddedVisual())
	{
		visual->AddComponent(std::make_shared<LcTiledSpriteComponent>(tiledJsonPath, inLayerNames), context);
	}
}

void LcSpriteHelper::AddTiledComponent(const std::string& tiledJsonPath, LcTiledObjectHandler inObjectHandler, const LcLayersList& inLayerNames) const
{
	if (auto visual = context.world->GetLastAddedVisual())
	{
		visual->AddComponent(std::make_shared<LcTiledSpriteComponent>(tiledJsonPath, inObjectHandler, inLayerNames), context);
	}
}

void LcSpriteHelper::AddParticlesComponent(unsigned short inNumParticles, unsigned short inNumFrames, LcSizef inFrameSize,
	float inParticleLifetime, float inParticleSpeed, float inParticleMovementRadius) const
{
	if (auto visual = context.world->GetLastAddedVisual())
	{
		visual->AddComponent(std::make_shared<LcBasicParticlesComponent>(inNumParticles, inNumFrames,
			inFrameSize, inParticleLifetime, inParticleSpeed, inParticleMovementRadius), context);
	}
}

void LcSpriteAnimationComponent::Update(float deltaSeconds, const LcAppContext& context)
{
	IVisualComponent::Update(deltaSeconds, context);

	double curGameTime = (double)GetTickCount64();
	double frameDelta = curGameTime - lastFrameSeconds;
	float frameLength = 1.0f / framesPerSecond;
	if ((frameDelta / 1000.0) > frameLength)
	{
		lastFrameSeconds = curGameTime;
		curFrame++;
		if (curFrame >= numFrames) curFrame = 0;
	}
}

LcVector4 LcSpriteAnimationComponent::GetAnimData() const
{
	if (auto sprite = (LcSprite*)owner)
	{
		if (auto texComp = sprite->GetTextureComponent())
		{
			auto framesPerRow = unsigned short(texComp->GetTextureSize().x / frameSize.x);
			auto column = curFrame % framesPerRow;
			auto row = curFrame / framesPerRow;
			float frameWidth = frameSize.x / texComp->GetTextureSize().x;
			float frameHeight = frameSize.y / texComp->GetTextureSize().y;

			return LcVector4(
				frameWidth,
				frameHeight,
				frameWidth * column,
				frameHeight * row);
		}
	}

	return LcDefaults::ZeroVec4;
}

void LcTiledSpriteComponent::Init(const LcAppContext& context)
{
	LC_TRY

	if (!owner) throw std::exception("LcTiledSpriteComponent::Init(): Cannot get owner");

	auto tilesFileText = ReadTextFile(tiledJsonPath.c_str());
	if (tilesFileText.empty()) throw std::exception("LcTiledSpriteComponent::Init(): Cannot read tiled file");

	auto tilesObject = json::parse(tilesFileText);
	auto tilesets = tilesObject["tilesets"];

	// get tileset
	std::string tilesetFileName;
	for (auto tileset : tilesets)
	{
		auto source = tileset["source"].get<std::string>();
		if (!source.empty())
		{
			tilesetFileName = source;
			break;
		}
	}

	std::filesystem::path tilesetPath(tiledJsonPath);
	tilesetPath.replace_filename(tilesetFileName);
	auto tilsetFileText = ReadTextFile(tilesetPath.u8string().c_str());
	auto tilsetObject = json::parse(tilsetFileText);

	// add texture
	auto texPath = tilsetObject["image"].get<std::string>();
	if (!texPath.empty())
	{
		context.world->GetSpriteHelper().AddTextureComponent(texPath);
	}

	// check parameters
	auto rows = tilesObject["height"].get<int>();
	auto columns = tilesObject["width"].get<int>();
	auto tilewidth = tilesObject["tilewidth"].get<float>();
	auto tileheight = tilesObject["tileheight"].get<float>();
	auto imagewidth = tilsetObject["imagewidth"].get<float>();
	auto imageheight = tilsetObject["imageheight"].get<float>();
	if (rows <= 0 ||
		columns <= 0 ||
		tilewidth <= 0.0f ||
		tileheight <= 0.0f ||
		imagewidth <= 0.0f ||
		imageheight <= 0.0f ||
		imagewidth < tilewidth ||
		imageheight < tileheight)
	{
		throw std::exception("LcTiledSpriteComponent::Init(): Invalid tileset");
	}

	float uvx = tilewidth / imagewidth;
	float uvy = tileheight / imageheight;
	int uvRows = int(imageheight / tileheight);
	int uvColumns = int(imagewidth / tilewidth);
	float offsetX = tilewidth * columns / -2.0f;
	float offsetY = tileheight * rows / -2.0f;
	float z = owner->GetPos().z;
	scale.x = owner->GetSize().x / (tilewidth * columns);
	scale.y = owner->GetSize().y / (tileheight * rows);

	for (auto layer : tilesObject["layers"])
	{
		auto curLayerName = layer["name"].get<std::string>();
		auto curLayerType = layer["type"].get<std::string>();
		bool layerFound = std::find(layerNames.begin(), layerNames.end(), curLayerName) != layerNames.end();
		bool validLayer = layerFound || layerNames.empty();
		if (!validLayer) continue;

		// add tiles
		auto layerTiles = layer["data"];
		if (layerTiles.is_array() && (curLayerType == LcTiles::Type::TileLayer))
		{
			int tileId = 0;
			for (size_t id = 0; id < layerTiles.size(); id++)
			{
				int uvTileId = layerTiles[id].get<int>();
				if (uvTileId != 0)
				{
					uvTileId--; // 0 - means invalid, starts from 1, remap to 0

					int row = tileId / columns;
					int column = tileId % columns;
					float x = tilewidth * column + offsetX;
					float y = tileheight * row + offsetY;

					int uvRow = uvTileId / uvColumns;
					int uvColumn = uvTileId % uvColumns;
					float ox = uvx * uvColumn;
					float oy = uvy * uvRow;

					LC_TILES_DATA tile{};
					tile.pos[0] = LcVector3(x, y, z);
					tile.pos[1] = LcVector3(x + tilewidth, y + tileheight, z);
					tile.pos[2] = LcVector3(x + tilewidth, y, z);
					tile.pos[3] = LcVector3(x, y + tileheight, z);
					tile.uv[0] = LcVector2(ox, oy);
					tile.uv[1] = LcVector2(ox + uvx, oy + uvy);
					tile.uv[2] = LcVector2(ox + uvx, oy);
					tile.uv[3] = LcVector2(ox, oy + uvy);
					tiles.push_back(tile);
				}

				tileId++;
			}

			continue;
		}

		// process objects
		auto layerObjects = layer["objects"];
		if (layerObjects.is_array() && (curLayerType == LcTiles::Type::ObjectGroup) && objectHandler)
		{
			for (auto object : layerObjects)
			{
				auto x = object["x"].get<float>();
				auto y = object["y"].get<float>();
				auto width = object["width"].get<float>();
				auto height = object["height"].get<float>();
				auto pos = LcVector2(x + width / 2.0f, y + height / 2.0f) * scale;
				auto size = LcSizef(width, height) * scale;

				auto objectName = object["name"].get<std::string>();
				auto objectType = object["type"].get<std::string>();
				auto properties = object["properties"];

				LcTiledProps props;
				if (properties.is_array())
				{
					for (auto entry : properties)
					{
						std::pair<std::string, LcAny> newProp;
						newProp.first = entry["name"].get<std::string>();

						auto type = entry["type"].get<std::string>();
						if (type == "int")
							newProp.second.iValue = entry["value"].get<int>();
						else if (type == "float")
							newProp.second.fValue = entry["value"].get<float>();
						else if (type == "bool")
							newProp.second.bValue = entry["value"].get<bool>();
						else if (type == "string")
							newProp.second.sValue = entry["value"].get<std::string>();

						props.push_back(newProp);
					}
				}

				objectHandler(curLayerName, objectName, objectType, props, pos, size);
			}

			continue;
		}
	}

	LC_CATCH { LC_THROW("LcTiledSpriteComponent::Init()") }
}

void LcSprite::AddComponent(TVComponentPtr comp, const LcAppContext& context)
{
	IVisualBase::AddComponent(comp, context);

	features.insert(comp->GetType());
}
