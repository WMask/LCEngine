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


/** Default Sprite factory implementation */
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

/** Default Sprite factory implementation */
class LcWidgetFactory : public TWorldFactory<IWidget, LcWidgetData>
{
public:
	LcWidgetFactory() {}
	//
	virtual std::shared_ptr<IWidget> Build(const LcWidgetData& data) override
	{
		return std::make_shared<LcWidget>(data);
	}
};


LcWorld::LcWorld()
{
	spriteFactory = TSpriteFactoryPtr(new LcSpriteFactory());
	widgetFactory = TWidgetFactoryPtr(new LcWidgetFactory());
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
	{
		newSprite->Init(this);
		sprites.push_back(newSprite);
	}
	else
		throw std::exception("LcWorld::AddSprite(): Cannot create sprite");

	return newSprite.get();
}

ISprite* LcWorld::AddSprite3D(float x, float y, float z, float width, float height, float inRotZ, bool inVisible)
{
	return AddSprite(LcSpriteData(LcVector3(x, y, z), LcSizef(width, height), inRotZ, inVisible));
}

ISprite* LcWorld::AddSprite2D(float x, float y, float width, float height, float inRotZ, bool inVisible)
{
	return AddSprite(LcSpriteData(LcVector3(x, y, 0.0f), LcSizef(width, height), inRotZ, inVisible));
}

void LcWorld::RemoveSprite(ISprite* sprite)
{
	auto it = std::find_if(sprites.begin(), sprites.end(), [sprite](std::shared_ptr<ISprite>& data) { return data.get() == sprite; });
	if (it != sprites.end())
	{
		(*it)->Destroy(this);
		sprites.erase(it);
	}
}

IWidget* LcWorld::AddWidget(const LcWidgetData& inWidget)
{
	auto newWidget = widgetFactory ? widgetFactory->Build(inWidget) : nullptr;
	if (newWidget)
	{
		newWidget->Init(this);
		widgets.push_back(newWidget);
	}
	else
		throw std::exception("LcWorld::AddWidget(): Cannot create widget");

	return newWidget.get();
}

IWidget* LcWorld::AddWidget(float x, float y, float z, float width, float height, bool inVisible)
{
	return AddWidget(LcWidgetData(LcVector3(x, y, z), LcSizef(width, height), inVisible));

}

IWidget* LcWorld::AddWidget(float x, float y, float width, float height, bool inVisible)
{
	return AddWidget(LcWidgetData(LcVector3(x, y, 0.0f), LcSizef(width, height), inVisible));
}

void LcWorld::RemoveWidget(IWidget* widget)
{
	auto it = std::find_if(widgets.begin(), widgets.end(), [widget](std::shared_ptr<IWidget>& data) { return data.get() == widget; });
	if (it != widgets.end())
	{
		(*it)->Destroy(this);
		widgets.erase(it);
	}
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

TWorldPtr GetWorld()
{
	return std::make_shared<LcWorld>();
}
