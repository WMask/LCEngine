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


class LcVisualLifetimeStrategy : public LcLifetimeStrategy<IVisual, IWorld::TVisualSet>
{
public:
	LcVisualLifetimeStrategy() {}
	//
	virtual ~LcVisualLifetimeStrategy() {}
	//
	virtual std::shared_ptr<IVisual> Create(const void* userData) override
	{
		auto layerPtr = static_cast<const float*>(userData);
		std::shared_ptr<IVisual> newVisual;

		switch (curTypeId)
		{
		case LcCreatables::Sprite: newVisual = std::make_shared<LcSprite>(); break;
		case LcCreatables::Widget: newVisual = std::make_shared<LcWidget>(); break;
		}

		// add layer Z for initial valid sorting in multiset
		newVisual->SetPos(LcVector3(0.0f, 0.0f, *layerPtr));

		return newVisual;
	}
	//
	virtual void Destroy(IVisual& item, IWorld::TVisualSet& items) override
	{
		auto inWidget = static_cast<IWidget*>(&item);

		for (auto& visual : items)
		{
			if (visual.get() != inWidget &&
				visual->GetTypeId() == LcCreatables::Widget)
			{
				auto curWidget = static_cast<IWidget*>(visual.get());
				curWidget->RemoveChild(inWidget);
			}
		}
	}
};


LcWorld::LcWorld(const LcAppContext& inContext)
	: context(inContext)
	, visualHelper(std::make_unique<LcVisualHelper>(inContext))
	, spriteHelper(std::make_unique<LcSpriteHelper>(inContext))
	, widgetHelper(std::make_unique<LcWidgetHelper>(inContext))
	, globalTint(LcDefaults::White3)
	, lastVisual(nullptr)
{
	items.SetLifetimeStrategy(std::make_shared<LcVisualLifetimeStrategy>());
}

ISprite* LcWorld::AddSprite(float x, float y, LcLayersRange z, float width, float height, float rotZ, bool visible)
{
	auto newSprite = items.Add<LcSprite>(z.get());
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

ISprite* LcWorld::AddSprite(float x, float y, float width, float height, float inRotZ, bool inVisible)
{
	return AddSprite(x, y, LcLayers::Z0, width, height, inRotZ, inVisible);
}

void LcWorld::RemoveSprite(ISprite* sprite)
{
	if (lastVisual == sprite) lastVisual = nullptr;

	items.Remove(sprite);
}

IWidget* LcWorld::AddWidget(float x, float y, LcLayersRange z, float width, float height, bool visible)
{
	auto newWidget = items.Add<LcWidget>(z.get());
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

	items.Remove(widget);
}

IVisual* LcWorld::GetVisualByTag(ObjectTag tag) const
{
	auto it = std::find_if(items.GetItems().begin(), items.GetItems().end(), [tag](const std::shared_ptr<IVisual>& visual) {
		return visual->GetTag() == tag;
	});
	return (it != items.GetItems().end()) ? it->get() : nullptr;
}

void LcWorld::SetGlobalTint(LcColor3 tint)
{
	if (globalTint != tint)
	{
		globalTint = tint;
		onTintChanged.Broadcast(tint);
	}
}


LcWorldScale::LcWorldScale() : scaleList{ {LcSize(1920, 1080), LcDefaults::OneVec2} }, scale(LcDefaults::OneVec2), scaleFonts(true)
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
			if (prevScale != scale) onScaleChanged.Broadcast(scale);
			return;
		}
	}

	for (auto entry : scaleList)
	{
		// check greater
		if (entry.resolution.y >= newScreenSize.y)
		{
			scale = entry.scale;
			if (prevScale != scale) onScaleChanged.Broadcast(scale);
			return;
		}
	}

	if (scaleList.size() > 0)
	{
		// accept any
		scale = scaleList.begin()->scale;
		if (prevScale != scale) onScaleChanged.Broadcast(scale);
	}
}

TWorldPtr GetWorld(LcAppContext& context)
{
	return std::make_shared<LcWorld>(context);
}
