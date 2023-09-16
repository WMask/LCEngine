/**
* World.cpp
* 29.01.2023
* (c) Denis Romakhov
*/

#pragma once

#include "pch.h"
#include "World/World.h"
#include "World/Sprites.h"
#include "GUI/Widgets.h"


class LcSpriteLifetimeStrategy : public LcLifetimeStrategy<ISprite>
{
public:
	LcSpriteLifetimeStrategy() {}
	//
	virtual ~LcSpriteLifetimeStrategy() {}
	//
	virtual std::shared_ptr<ISprite> Create() override { return std::make_shared<LcSprite>(); }
	//
	virtual void Destroy(ISprite& item) override {}
};

class LcWidgetLifetimeStrategy : public LcLifetimeStrategy<IWidget>
{
public:
	LcWidgetLifetimeStrategy() {}
	//
	virtual ~LcWidgetLifetimeStrategy() {}
	//
	virtual std::shared_ptr<IWidget> Create() override { return std::make_shared<LcWidget>(); }
	//
	virtual void Destroy(IWidget& item) override {}
};


LcWorld::LcWorld(const LcAppContext& inContext)
	: context(inContext)
	, spriteHelper(std::make_unique<LcSpriteHelper>(inContext))
	, widgetHelper(std::make_unique<LcWidgetHelper>(inContext))
	, lastVisual(nullptr)
{
	sprites.SetLifetimeStrategy(std::make_shared<LcSpriteLifetimeStrategy>());
	widgets.SetLifetimeStrategy(std::make_shared<LcWidgetLifetimeStrategy>());
}

ISprite* LcWorld::AddSprite(float x, float y, LcLayersRange z, float width, float height, float rotZ, bool visible)
{
	auto newSprite = sprites.Add();
	if (newSprite)
	{
		newSprite->SetPos(LcVector3(x, y, z));
		newSprite->SetSize(LcSizef(width, height));
		newSprite->SetRotZ(rotZ);
		newSprite->SetVisible(visible);
		newSprite->Init(context);
	}
	else
	{
		throw std::exception("LcWorld::AddSprite(): Cannot create sprite");
	}

	lastVisual = newSprite;
	return newSprite;
}

ISprite* LcWorld::AddSprite2D(float x, float y, float width, float height, float inRotZ, bool inVisible)
{
	return AddSprite(x, y, LcLayers::Z0, width, height, inRotZ, inVisible);
}

void LcWorld::RemoveSprite(ISprite* sprite)
{
	if (lastVisual == sprite) lastVisual = nullptr;

	sprites.Remove(sprite);
}

IWidget* LcWorld::AddWidget(float x, float y, float z, float width, float height, bool visible)
{
	auto newWidget = widgets.Add();
	if (newWidget)
	{
		newWidget->SetPos(LcVector3(x, y, z));
		newWidget->SetSize(LcSizef(width, height));
		newWidget->SetVisible(visible);
		newWidget->Init(context);
	}
	else
	{
		throw std::exception("LcWorld::AddWidget(): Cannot create widget");
	}

	lastVisual = newWidget;
	return newWidget;
}

IWidget* LcWorld::AddWidget(float x, float y, float width, float height, bool inVisible)
{
	return AddWidget(x, y, LcLayers::Z0, width, height, inVisible);
}

void LcWorld::RemoveWidget(IWidget* widget)
{
	if (lastVisual == widget) lastVisual = nullptr;

	widgets.Remove(widget);
}

LcWorldScale::LcWorldScale() : scaleList{ {LcSize(1920, 1080), LcDefaults::OneVec2} }, scale(1.0f, 1.0f), scaleFonts(true)
{
}

void LcWorldScale::UpdateWorldScale(LcSize newScreenSize)
{
	auto prevScale = scale;

	for (auto entry : scaleList)
	{
		// check equal
		if (entry.resolution == newScreenSize)
		{
			scale = entry.scale;

			if (prevScale != scale)
			{
				auto& listeners = scaleUpdatedHandler.GetListeners();
				for (auto& listener : listeners) listener(scale);
			}

			return;
		}
	}

	for (auto entry : scaleList)
	{
		// check greater
		if (entry.resolution.y >= newScreenSize.y)
		{
			scale = entry.scale;

			if (prevScale != scale)
			{
				auto& listeners = scaleUpdatedHandler.GetListeners();
				for (auto& listener : listeners) listener(scale);
			}

			return;
		}
	}

	if (scaleList.size() > 0)
	{
		// accept any
		scale = scaleList.begin()->scale;

		if (prevScale != scale)
		{
			auto& listeners = scaleUpdatedHandler.GetListeners();
			for (auto& listener : listeners) listener(scale);
		}
	}
}

TWorldPtr GetWorld(LcAppContext& context)
{
	return std::make_shared<LcWorld>(context);
}
