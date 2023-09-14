/**
* World.h
* 29.01.2023
* (c) Denis Romakhov
*/

#pragma once

#include "WorldInterface.h"
#include "Camera.h"

#pragma warning(disable : 4251)


/**
* Game world manager. Contains default sprite implementation */
class LcWorld : public IWorld
{
public: // IWorld interface implementation
	//
	LcWorld();
	//
	~LcWorld();
	//
	virtual void SetSpriteFactory(TSpriteFactoryPtr inSpriteFactory) override { spriteFactory = inSpriteFactory; }
	//
	virtual void SetWidgetFactory(TWidgetFactoryPtr inWidgetFactory) override { widgetFactory = inWidgetFactory; }
	//
	virtual ISprite* AddSprite(const LcSpriteData& sprite) override;
	//
	virtual ISprite* AddSprite(float x, float y, float z, float width, float height, float inRotZ = 0.0f, bool inVisible = true) override;
	//
	virtual ISprite* AddSprite2D(float x, float y, float width, float height, float inRotZ = 0.0f, bool inVisible = true) override;
	//
	virtual void RemoveSprite(ISprite* sprite) override;
	//
	virtual SPRITE_LIST& GetSprites() override { return sprites; }
	//
	virtual IWidget* AddWidget(const LcWidgetData& widget) override;
	//
	virtual IWidget* AddWidget(float x, float y, float z, float width, float height, bool inVisible = true) override;
	//
	virtual IWidget* AddWidget(float x, float y, float width, float height, bool inVisible = true) override;
	//
	virtual void RemoveWidget(IWidget* widget) override;
	//
	virtual WIDGET_LIST& GetWidgets() override { return widgets; }
	//
	virtual const LcCamera& GetCamera() const override { return camera; }
	//
	virtual LcCamera& GetCamera() override { return camera; }
	//
	virtual const LcWorldScale& GetWorldScale() const override { return worldScale; }
	//
	virtual LcWorldScale& GetWorldScale() override { return worldScale; }


protected:
	LcWorld(const LcWorld&);
	//
	LcWorld& operator=(const LcWorld&);
	//
	friend WORLD_API TWorldPtr GetWorld();


protected:
	SPRITE_LIST sprites;
	//
	WIDGET_LIST widgets;
	//
	TSpriteFactoryPtr spriteFactory;
	//
	TWidgetFactoryPtr widgetFactory;
	//
	LcWorldScale worldScale;
	//
	LcCamera camera;

};
