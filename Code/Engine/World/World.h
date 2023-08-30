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
	/**
	* Constructor */
	LcWorld();
	/**
	* Destructor */
	~LcWorld();
	/**
	* Set sprite factory */
	virtual void SetSpriteFactory(TSpriteFactoryPtr inSpriteFactory) override { spriteFactory = inSpriteFactory; }
	/**
	* Add sprite */
	virtual ISprite* AddSprite(const LcSpriteData& sprite) override;
	/**
	* Add sprite */
	virtual ISprite* AddSprite(float x, float y, float z, float width, float height, float inRotZ = 0.0f, bool inVisible = true) override;
	/**
	* Add sprite */
	virtual ISprite* AddSprite(float x, float y, float width, float height, float inRotZ = 0.0f, bool inVisible = true) override;
	/**
	* Remove sprite */
	virtual void RemoveSprite(ISprite* sprite) override;
	/**
	* Get sprites */
	virtual SPRITE_LIST& GetSprites() override { return sprites; }
	/**
	* Add widget */
	virtual void AddWidget(std::shared_ptr<IWidget> widget) override;
	/**
	* Remove widget */
	virtual void RemoveWidget(IWidget* widget) override;
	/**
	* Get widgets */
	virtual WIDGET_LIST& GetWidgets() override { return widgets; }
	/**
	* Get camera */
	virtual const LcCamera& GetCamera() const override { return camera; }
	/**
	* Get camera */
	virtual LcCamera& GetCamera() override { return camera; }


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
	LcCamera camera;

};
