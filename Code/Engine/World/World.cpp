/**
* World.cpp
* 29.01.2023
* (c) Denis Romakhov
*/

#pragma once

#include "pch.h"
#include "World/World.h"


LcWorld& LcWorld::GetInstance()
{
	static LcWorld instance;
	return instance;
}

LcWorld::LcWorld()
{
}

LcWorld::LcWorld(const LcWorld&)
{
}

LcWorld& LcWorld::operator=(const LcWorld&)
{
	return *this;
}

LcWorld::~LcWorld()
{
}

LcSpriteData* LcWorld::AddSprite(const LcSpriteData& sprite)
{
	sprites.push_back(sprite);
	return &sprites.back();
}

void LcWorld::RemoveSprite(LcSpriteData* sprite)
{
	auto it = std::find_if(sprites.begin(), sprites.end(), [sprite](LcSpriteData& data) { return &data == sprite; });
	if (it != sprites.end()) sprites.erase(it);
}
