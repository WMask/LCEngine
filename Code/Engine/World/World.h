/**
* World.h
* 29.01.2023
* (c) Denis Romakhov
*/

#pragma once

#include "WorldInterface.h"
#include "Core/LCCreator.h"
#include "Visual.h"
#include "Camera.h"

#pragma warning(disable : 4251)


inline bool operator < (const std::shared_ptr<IVisual>& a, const std::shared_ptr<IVisual>& b)
{
	if (a->GetPos().z == b->GetPos().z) return a->GetTypeId() < b->GetTypeId();
	return (a->GetPos().z < b->GetPos().z);
}


/**
* Game world manager. Contains default sprite implementation */
class LcWorld : public IWorld
{
public:
	typedef LcCreator<class IVisual, LcLifetimeStrategy<class IVisual, TVisualSet>, TVisualSet> TVisualCreator;
	typedef std::shared_ptr<LcLifetimeStrategy<class IVisual, TVisualSet>> TVisualLifetime;
	typedef std::unique_ptr<class LcSpriteHelper> TSpriteHelperPtr;
	typedef std::unique_ptr<class LcWidgetHelper> TWidgetHelperPtr;


public:
	LcWorld(const LcAppContext& context);
	//
	LcWorld(const LcWorld&) = delete;
	//
	LcWorld& operator=(const LcWorld&) = delete;
	//
	void SetLifetimeStrategy(TVisualLifetime inVisualLifetime) { items.SetLifetimeStrategy(inVisualLifetime); }


public: // IWorld interface implementation
	//
	virtual ~LcWorld() override {}
	//
	virtual class ISprite* AddSprite(float x, float y, LcLayersRange z, float width, float height, float rotZ = 0.0f, bool visible = true) override;
	//
	virtual class ISprite* AddSprite(float x, float y, float width, float height, float rotZ = 0.0f, bool visible = true) override;
	//
	virtual void RemoveSprite(class ISprite* sprite) override;
	//
	virtual class IWidget* AddWidget(float x, float y, LcLayersRange z, float width, float height, bool visible = true) override;
	//
	virtual class IWidget* AddWidget(float x, float y, float width, float height, bool visible = true) override;
	//
	virtual void RemoveWidget(class IWidget* widget) override;
	//
	virtual void Clear() override { items.Clear(); }
	//
	virtual class IVisual* GetVisualByTag(VisualTag tag) const override;
	//
	virtual const TVisualSet& GetVisuals() const override { return items.GetItems(); }
	//
	virtual TVisualSet& GetVisuals() override { return items.GetItems(); }
	//
	virtual const LcCamera& GetCamera() const override { return camera; }
	//
	virtual LcCamera& GetCamera() override { return camera; }
	//
	virtual const LcWorldScale& GetWorldScale() const override { return worldScale; }
	//
	virtual LcWorldScale& GetWorldScale() override { return worldScale; }
	//
	virtual void SetGlobalTint(LcColor3 tint) override;
	//
	virtual LcColor3 GetGlobalTint() const override { return globalTint; }
	//
	virtual class IVisual* GetLastAddedVisual() const override { return lastVisual; }
	//
	virtual const class LcSpriteHelper& GetSpriteHelper() const override { return *spriteHelper.get(); }
	//
	virtual const class LcWidgetHelper& GetWidgetHelper() const override { return *widgetHelper.get(); }


protected:
	const LcAppContext& context;
	//
	TVisualCreator items;
	//
	TSpriteHelperPtr spriteHelper;
	//
	TWidgetHelperPtr widgetHelper;
	//
	LcWorldScale worldScale;
	//
	LcCamera camera;
	//
	LcColor3 globalTint;
	//
	class IVisual* lastVisual;

};
