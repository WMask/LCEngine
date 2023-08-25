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

void LcWorld::RemoveSprite(ISprite* sprite)
{
	auto it = std::find_if(sprites.begin(), sprites.end(), [sprite](std::shared_ptr<ISprite>& data) { return data.get() == sprite; });
	if (it != sprites.end()) sprites.erase(it);
}

IWidget* LcWorld::AddWidget(const LcWidgetData& inWidget)
{
	auto newWidget = spriteFactory ? widgetFactory->Build(inWidget) : nullptr;
	if (newWidget)
		widgets.push_back(newWidget);
	else
		throw std::exception("LcWorld::AddWidget(): Cannot create widget");

	return newWidget.get();
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
