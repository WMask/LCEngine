/**
* WorldInterface.h
* 03.02.2023
* (c) Denis Romakhov
*/

#pragma once

#include "Module.h"
#include "Core/LCTypesEx.h"
#include "Core/LCDelegate.h"

#include <deque>
#include <memory>
#include <set>

#pragma warning(disable : 4251)


/** 2D object layers. Z0 - front, Z9 - back. */
namespace LcLayers
{
	static const float Z0 =  0.0f;
	static const float Z1 = -0.1f;
	static const float Z2 = -0.2f;
	static const float Z3 = -0.3f;
	static const float Z4 = -0.4f;
	static const float Z5 = -0.5f;
	static const float Z6 = -0.6f;
	static const float Z7 = -0.7f;
	static const float Z8 = -0.8f;
	static const float Z9 = -0.9f;
};

static const float LcMinLayer = LcLayers::Z9;
static const float LcMaxLayer = LcLayers::Z0;

/** Ranged float (-0.9f, 0.0f) */
typedef LcRange<float, LcMinLayer, LcMaxLayer> LcLayersRange;


/**
* Game world scaling parameters for different resolutions.
* For resolutions not added to scaleList scale selected by resolution.y >= newScreenSize.y
*/
struct WORLD_API LcWorldScale
{
	LcWorldScale();
	//
	LcWorldScale(const LcWorldScale& scale) = default;
	//
	LcWorldScale& operator=(const LcWorldScale& scale) = default;
	//
	void UpdateWorldScale(LcSize newScreenSize);
	//
	LcDelegate<void(LcVector2)> scaleUpdatedHandler;
	//
	struct ScalePair
	{
		LcSize resolution;
		LcVector2 scale;
		inline bool operator<(const ScalePair& s) const { return resolution.y < s.resolution.y; }
	};
	std::set<ScalePair> scaleList;
	//
	LcVector2 scale;
	//
	bool scaleFonts;
};


/**
* Game world interface */
class IWorld
{
public:
	typedef std::deque<std::shared_ptr<class ISprite>> TSpriteList;
	//
	typedef std::deque<std::shared_ptr<class IWidget>> TWidgetList;


public:
	/**
	* Destructor */
	virtual ~IWorld() {}
	/**
	* Add sprite to layer z */
	virtual ISprite* AddSprite(float x, float y, LcLayersRange z, float width, float height, float inRotZ = 0.0f, bool inVisible = true) = 0;
	/**
	* Add sprite to layer 0 */
	virtual ISprite* AddSprite2D(float x, float y, float width, float height, float inRotZ = 0.0f, bool inVisible = true) = 0;
	/**
	* Remove sprite */
	virtual void RemoveSprite(ISprite* sprite) = 0;
	/**
	* Get sprites */
	virtual TSpriteList& GetSprites() = 0;
	/**
	* Add widget */
	virtual IWidget* AddWidget(float x, float y, float z, float width, float height, bool inVisible = true) = 0;
	/**
	* Add widget */
	virtual IWidget* AddWidget(float x, float y, float width, float height, bool inVisible = true) = 0;
	/**
	* Remove widget */
	virtual void RemoveWidget(IWidget* widget) = 0;
	/**
	* Get widgets */
	virtual TWidgetList& GetWidgets() = 0;
	/**
	* Get camera */
	virtual const class LcCamera& GetCamera() const = 0;
	/**
	* Get camera */
	virtual class LcCamera& GetCamera() = 0;
	/**
	* Get world scale */
	virtual const LcWorldScale& GetWorldScale() const = 0;
	/**
	* Get world scale */
	virtual LcWorldScale& GetWorldScale() = 0;
	/**
	* Get last added visual */
	virtual class IVisual* GetLastAddedVisual() const = 0;
	/**
	* Get sprite helper */
	virtual const class LcSpriteHelper& GetSpriteHelper() const = 0;
	/**
	* Get widget helper */
	virtual const class LcWidgetHelper& GetWidgetHelper() const = 0;

};
