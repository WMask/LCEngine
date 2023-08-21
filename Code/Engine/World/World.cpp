/**
* World.cpp
* 29.01.2023
* (c) Denis Romakhov
*/

#pragma once

#include "pch.h"
#include "World/World.h"


/**
* Default Sprite implementation */
class LcSprite : public ISprite
{
public:
	LcSprite(LcSpriteData inSprite) : sprite(inSprite) {}
	//
	LcSpriteData sprite;


public: // ISprite interface implementation
	virtual ~LcSprite() override {}
	//
	virtual void SetColors(LcSpriteColors inColors) override { sprite.colors = inColors; }
	//
	virtual LcSpriteColors GetColors() const override { return sprite.colors; }
	//
	virtual LcSpriteType GetType() const override { return sprite.type; }


public: // IVisual interface implementation
	virtual void SetSize(LcSizef inSize) override { sprite.size = inSize; }
	//
	virtual LcSizef GetSize() const override { return sprite.size; }
	//
	virtual void SetPos(LcVector3 inPos) override { sprite.pos = inPos; }
	//
	virtual void AddPos(LcVector3 inPos) override { sprite.pos = sprite.pos + inPos; }
	//
	virtual LcVector3 GetPos() const override { return sprite.pos; }
	//
	virtual void SetRotZ(float inRotZ) override { sprite.rotZ = inRotZ; }
	//
	virtual void AddRotZ(float inRotZ) override { sprite.rotZ += inRotZ; }
	//
	virtual float GetRotZ() const override { return sprite.rotZ; }
	//
	virtual void SetVisible(bool inVisible) override { sprite.visible = inVisible; }
	//
	virtual bool IsVisible() const override { return sprite.visible; }
	//
	virtual void OnMouseButton(LcMouseBtn btn, LcKeyState state, int x, int y) {}
	//
	virtual void OnMouseMove(int x, int y) {}

};


/**
* Default Sprite factory implementation */
class LcSpriteFactory : public TWorldFactory<ISprite, LcSpriteData>
{
public:
	LcSpriteFactory() {}
	//
	virtual std::shared_ptr<ISprite> Build(const LcSpriteData& data) override
	{
		return std::shared_ptr<ISprite>(new LcSprite(data));
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
