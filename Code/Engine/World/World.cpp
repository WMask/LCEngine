/**
* World.cpp
* 29.01.2023
* (c) Denis Romakhov
*/

#pragma once

#include "pch.h"
#include "World/World.h"
#include "World/Sprites.h"


/**
* Default Sprite factory implementation */
class LcSpriteFactory : public TWorldFactory<ISprite, LcSpriteData>
{
public:
	LcSpriteFactory() {}
	//
	virtual std::shared_ptr<ISprite> Build(const LcSpriteData& data) override
	{
		return std::make_shared<LcSprite>(data);
	}
};


LcWorld::LcWorld()
{
	spriteFactory = TSpriteFactoryPtr(new LcSpriteFactory());
}

LcWorld::~LcWorld()
{
}

LcWorld::LcWorld(const LcWorld&)
{
}

LcWorld& LcWorld::operator=(const LcWorld&)
{
	return *this;
}

ISprite* LcWorld::AddSprite(const LcSpriteData& inSprite)
{
	auto newSprite = spriteFactory ? spriteFactory->Build(inSprite) : nullptr;
	if (newSprite)
		sprites.push_back(newSprite);
	else
		throw std::exception("LcWorld::AddSprite(): Cannot create sprite");

	return newSprite.get();
}

ISprite* LcWorld::AddSprite(float x, float y, float z, float width, float height, float inRotZ, bool inVisible)
{
	return AddSprite(LcSpriteData(LcVector3(x, y, z), LcSizef(width, height), inRotZ, inVisible));
}

ISprite* LcWorld::AddSprite(float x, float y, float width, float height, float inRotZ, bool inVisible)
{
	return AddSprite(LcSpriteData(LcVector3(x, y, 0.0f), LcSizef(width, height), inRotZ, inVisible));
}

void LcWorld::RemoveSprite(ISprite* sprite)
{
	auto it = std::find_if(sprites.begin(), sprites.end(), [sprite](std::shared_ptr<ISprite>& data) { return data.get() == sprite; });
	if (it != sprites.end()) sprites.erase(it);
}

void LcWorld::AddWidget(std::shared_ptr<IWidget> widget)
{
	if (!widget) throw std::exception("LcWorld::AddWidget(): Invalid widget");

	widgets.push_back(widget);
}

void LcWorld::RemoveWidget(IWidget* widget)
{
	auto it = std::find_if(widgets.begin(), widgets.end(), [widget](std::shared_ptr<IWidget>& data) { return data.get() == widget; });
	if (it != widgets.end()) widgets.erase(it);
}

TWorldPtr GetWorld()
{
	return std::make_shared<LcWorld>();
}
