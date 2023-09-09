/**
* Sprites.cpp
* 26.08.2023
* (c) Denis Romakhov
*/

#pragma once

#include "pch.h"
#include "World/Sprites.h"
#include "Core/Physics.h"
#include "Core/LcUtils.h"

#include <filesystem>

// put nlohmann's json lib in Code/Json folder
#include "nlohmann/json.hpp"

using json = nlohmann::json;


void LcSpriteAnimationComponent::Update(float deltaSeconds)
{
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
			auto framesPerRow = unsigned short(texComp->texSize.x / frameSize.x);
			auto column = curFrame % framesPerRow;
			auto row = curFrame / framesPerRow;
			float frameWidth = frameSize.x / texComp->texSize.x;
			float frameHeight = frameSize.y / texComp->texSize.y;

			return LcVector4(
				frameWidth,
				frameHeight,
				frameWidth * column,
				frameHeight * row);
		}
	}

	return LcDefaults::ZeroVec4;
}

void LcTiledSpriteComponent::Init(class IWorld& world)
{
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
		owner->AddComponent(std::make_shared<LcVisualTextureComponent>(texPath));
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

	// add tiles
	for (auto layer : tilesObject["layers"])
	{
		auto curLayerName = layer["name"].get<std::string>();
		auto curLayerType = layer["type"].get<std::string>();
		bool layerFound = std::find(layerNames.begin(), layerNames.end(), curLayerName) != layerNames.end();
		bool validLayer = layerFound || layerNames.empty();
		if (!validLayer) continue;

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
					tile.pos[0] = LcVector3(x, y, 0.0f);
					tile.pos[1] = LcVector3(x + tilewidth, y + tileheight, 0.0f);
					tile.pos[2] = LcVector3(x + tilewidth, y, 0.0f);
					tile.pos[3] = LcVector3(x, y + tileheight, 0.0f);
					tile.uv[0] = LcVector2(ox, oy);
					tile.uv[1] = LcVector2(ox + uvx, oy + uvy);
					tile.uv[2] = LcVector2(ox + uvx, oy);
					tile.uv[3] = LcVector2(ox, oy + uvy);
					tiles.push_back(tile);
				}

				tileId++;
			}
		}

		auto layerObjects = layer["objects"];
		if (layerObjects.is_array() && (curLayerType == LcTiles::Type::ObjectGroup))
		{
			for (auto object : layerObjects)
			{
				if (physWorld &&
					curLayerName == LcTiles::Layers::Collision &&
					curLayerType == LcTiles::Type::ObjectGroup)
				{
					auto x = object["x"].get<float>();
					auto y = object["y"].get<float>();
					auto width = object["width"].get<float>();
					auto height = object["height"].get<float>();

					physWorld->AddStaticBox(LcVector2(x + width / 2.0f, y + height / 2.0f), LcSizef(width, height));
				}
			}
		}
	}

	scale.x = owner->GetSize().x / (tilewidth * columns);
	scale.y = owner->GetSize().y / (tileheight * rows);
}

void LcSprite::Update(float deltaSeconds)
{
	if (auto anim = GetAnimationComponent())
	{
		anim->Update(deltaSeconds);
	}
}

void LcSprite::AddComponent(TVComponentPtr comp)
{
	IVisualBase::AddComponent(comp);

	features.insert(comp->GetType());
}
