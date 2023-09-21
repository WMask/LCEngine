/**
* World.h
* 29.01.2023
* (c) Denis Romakhov
*/

#pragma once

#include "WorldInterface.h"
#include "Core/LCCreator.h"
#include "Camera.h"

#pragma warning(disable : 4251)


/**
* Game world manager. Contains default sprite implementation */
class LcWorld : public IWorld
{
public:
	typedef std::shared_ptr<LcLifetimeStrategy<ISprite>> TSpriteLifetime;
	typedef std::shared_ptr<LcLifetimeStrategy<IWidget>> TWidgetLifetime;
	typedef std::unique_ptr<class LcSpriteHelper> TSpriteHelperPtr;
	typedef std::unique_ptr<class LcWidgetHelper> TWidgetHelperPtr;

public:
	LcWorld(const LcAppContext& context);
	//
	LcWorld(const LcWorld&) = delete;
	//
	LcWorld& operator=(const LcWorld&) = delete;
	//
	void SetSpriteLifetimeStrategy(TSpriteLifetime inSpriteLifetime) { sprites.SetLifetimeStrategy(inSpriteLifetime); }
	//
	void SetWidgetLifetimeStrategy(TWidgetLifetime inWidgetLifetime) { widgets.SetLifetimeStrategy(inWidgetLifetime); }


public: // IWorld interface implementation
	//
	virtual ~LcWorld() override {}
	//
	virtual ISprite* AddSprite(float x, float y, LcLayersRange z, float width, float height, float rotZ = 0.0f, bool visible = true) override;
	//
	virtual ISprite* AddSprite2D(float x, float y, float width, float height, float rotZ = 0.0f, bool visible = true) override;
	//
	virtual void RemoveSprite(ISprite* sprite) override;
	//
	virtual TSpriteList& GetSprites() override { return sprites.GetList(); }
	//
	virtual IWidget* AddWidget(float x, float y, float z, float width, float height, bool visible = true) override;
	//
	virtual IWidget* AddWidget(float x, float y, float width, float height, bool visible = true) override;
	//
	virtual void RemoveWidget(IWidget* widget) override;
	//
	virtual TWidgetList& GetWidgets() override { return widgets.GetList(); }
	//
	virtual const LcCamera& GetCamera() const override { return camera; }
	//
	virtual LcCamera& GetCamera() override { return camera; }
	//
	virtual const LcWorldScale& GetWorldScale() const override { return worldScale; }
	//
	virtual LcWorldScale& GetWorldScale() override { return worldScale; }
	//
	virtual IVisual* GetLastAddedVisual() const override { return lastVisual; }
	//
	virtual const class LcSpriteHelper& GetSpriteHelper() const override { return *spriteHelper.get(); }
	//
	virtual const class LcWidgetHelper& GetWidgetHelper() const override { return *widgetHelper.get(); }


protected:
	const LcAppContext& context;
	//
	LcCreator<class ISprite, class LcSprite> sprites;
	//
	LcCreator<class IWidget, class LcWidget> widgets;
	//
	TSpriteHelperPtr spriteHelper;
	//
	TWidgetHelperPtr widgetHelper;
	//
	LcWorldScale worldScale;
	//
	LcCamera camera;
	//
	IVisual* lastVisual;

};
