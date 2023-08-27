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
	* Set widget factory */
	virtual void SetWidgetFactory(TWidgetFactoryPtr inWidgetFactory) override { widgetFactory = inWidgetFactory; }
	/**
	* Add widget */
	virtual IWidget* AddWidget(const LcWidgetData& widget) override;
	/**
	* Remove widget */
	virtual void RemoveWidget(IWidget* widget) override;
	/**
	* Get widgets */
	virtual WIDGET_LIST& GetWidgets() override { return widgets; }
	/**
	* Set camera */
	virtual void SetCamera(LcVector3 newPos, LcVector3 newTarget) override
	{
		cameraPos = newPos;
		cameraTarget = newTarget;
	}
	/**
	* Move camera */
	virtual void MoveCamera(LcVector3 newPosOffset, LcVector3 newTargetOffset) override
	{
		cameraPos = cameraPos + newPosOffset;
		cameraTarget = cameraTarget + newTargetOffset;
	}
	/**
	* Get camera position */
	virtual LcVector3 GetCameraPos() const override { return cameraPos; }
	/**
	* Get camera target */
	virtual LcVector3 GetCameraTarget() const override { return cameraTarget; }


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
	LcVector3 cameraPos;
	//
	LcVector3 cameraTarget;

};
