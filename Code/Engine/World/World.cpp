/**
* World.cpp
* 29.01.2023
* (c) Denis Romakhov
*/

#pragma once

#include "pch.h"
#include "World/World.h"


LCWorld& LCWorld::GetInstance()
{
	static LCWorld instance;
	return instance;
}

LCWorld::LCWorld()
{
}

LCWorld::LCWorld(const LCWorld&)
{
}

LCWorld& LCWorld::operator=(const LCWorld&)
{
	return *this;
}

LCWorld::~LCWorld()
{
}

SPRITE_DATA* LCWorld::AddSprite(const SPRITE_DATA& sprite)
{
	sprites.push_back(sprite);
	return &sprites.back();
}

void LCWorld::RemoveSprite(SPRITE_DATA* sprite)
{
	auto it = std::find_if(sprites.begin(), sprites.end(), [sprite](SPRITE_DATA& data) { return &data == sprite; });
	if (it != sprites.end()) sprites.erase(it);
}
