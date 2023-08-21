/**
* World.h
* 29.01.2023
* (c) Denis Romakhov
*/

#pragma once

#include "WorldInterface.h"

#pragma warning(disable : 4251)


/**
* Game world manager. Contains default sprite implementation */
class LcWorld : public IWorld
{
public: // IWorld interface implementation
	/**
	* Set sprite factory */
	virtual void SetSpriteFactory(ISpriteFactoryPtr inSpriteFactory) override { spriteFactory = inSpriteFactory; }
	/**
	* Add sprite */
	virtual ISprite* AddSprite(const LcSpriteData& sprite) override;
	/**
	* Remove sprite */
	virtual void RemoveSprite(ISprite* sprite) override;
	/**
	* Get sprites */
	virtual SPRITE_LIST& GetSprites() override { return sprites; }
	/**
	* Set widget factory */
	virtual void SetWidgetFactory(IWidgetFactoryPtr inWidgetFactory) override { widgetFactory = inWidgetFactory; }
	/**
	* Add widget */
	virtual IWidget* AddWidget(const LcWidgetData& widget) override;
	/**
	* Remove widget */
	virtual void RemoveWidget(IWidget* widget) override;
	/**
	* Get widgets */
	virtual WIDGET_LIST& GetWidgets() override { return widgets; }


protected:
	LcWorld();
	//
	~LcWorld();
	//
	LcWorld(const LcWorld&);
	//
	LcWorld& operator=(const LcWorld&);
	//
	friend WORLD_API IWorld* GetWorld();


protected:
	SPRITE_LIST sprites;
	//
	WIDGET_LIST widgets;
	//
	ISpriteFactoryPtr spriteFactory;
	//
	IWidgetFactoryPtr widgetFactory;

};
