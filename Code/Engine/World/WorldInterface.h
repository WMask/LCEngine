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


constexpr float LcMinLayer = -0.9f;
constexpr float LcMaxLayer =  0.0f;

/** Ranged float (-0.9f, 0.0f) */
typedef LcRange<float, LcMinLayer, LcMaxLayer> LcLayersRange;

/** 2D object layers. Z0 - front, Z9 - back. */
namespace LcLayers
{
	static const LcLayersRange Z0 = LcLayersRange( 0.0f);
	static const LcLayersRange Z1 = LcLayersRange(-0.1f);
	static const LcLayersRange Z2 = LcLayersRange(-0.2f);
	static const LcLayersRange Z3 = LcLayersRange(-0.3f);
	static const LcLayersRange Z4 = LcLayersRange(-0.4f);
	static const LcLayersRange Z5 = LcLayersRange(-0.5f);
	static const LcLayersRange Z6 = LcLayersRange(-0.6f);
	static const LcLayersRange Z7 = LcLayersRange(-0.7f);
	static const LcLayersRange Z8 = LcLayersRange(-0.8f);
	static const LcLayersRange Z9 = LcLayersRange(-0.9f);
};


/**
* Game world scaling parameters for different resolutions.
* For resolutions not added to scaleList scale selected by resolution.y >= newScreenSize.y
*/
class LcWorldScale
{
public:
	LcDelegate<LcVector2> onScaleChanged;
	//
	struct ScalePair
	{
		LcSize resolution;
		LcVector2 scale;
		inline bool operator<(const ScalePair& s) const { return resolution.y < s.resolution.y; }
	};


public:
	LcWorldScale();
	//
	LcWorldScale(const LcWorldScale& scale) = default;
	//
	LcWorldScale& operator=(const LcWorldScale& scale) = default;
	//
	void UpdateWorldScale(LcSize newScreenSize);
	//
	inline bool GetScaleFonts() const { return scaleFonts; }
	//
	inline const std::set<ScalePair>& GetScaleList() const { return scaleList; }
	//
	inline std::set<ScalePair>& GetScaleList() { return scaleList; }
	//
	inline LcVector2 GetScale() const { return scale; }


protected:
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
	typedef std::multiset<std::shared_ptr<class IVisual>> TVisualSet;
	/**
	* Subscribe to get changes of sprites global tint. */
	LcDelegate<LcColor3> onTintChanged;


public:
	/**
	* Destructor */
	virtual ~IWorld() {}
	/**
	* Add sprite to layer z */
	virtual class ISprite* AddSprite(float x, float y, LcLayersRange z, float width, float height, float inRotZ = 0.0f, bool inVisible = true) = 0;
	/**
	* Add sprite to layer 0 */
	virtual class ISprite* AddSprite(float x, float y, float width, float height, float inRotZ = 0.0f, bool inVisible = true) = 0;
	/**
	* Remove sprite */
	virtual void RemoveSprite(class ISprite* sprite) = 0;
	/**
	* Add widget */
	virtual class IWidget* AddWidget(float x, float y, LcLayersRange z, float width, float height, bool inVisible = true) = 0;
	/**
	* Add widget */
	virtual class IWidget* AddWidget(float x, float y, float width, float height, bool inVisible = true) = 0;
	/**
	* Remove widget */
	virtual void RemoveWidget(class IWidget* widget) = 0;
	/**
	* Remove all sprites and widgets */
	virtual void Clear(bool removeRooted = false) = 0;
	/**
	* Get visual by tag */
	virtual class IVisual* GetVisualByTag(ObjectTag tag) const = 0;
	/**
	* Get object by tag */
	template<class T>
	T* GetObjectByTag(ObjectTag tag) const { return static_cast<T*>(GetVisualByTag(tag)); }
	/**
	* Get sprites and widgets */
	virtual const TVisualSet& GetVisuals() const = 0;
	/**
	* Get sprites and widgets */
	virtual TVisualSet& GetVisuals() = 0;
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
	* Update world scale */
	virtual void UpdateWorldScale(LcSize newScreenSize) = 0;
	/**
	* Set global sprites and widgets tint color */
	virtual void SetGlobalTint(LcColor3 tint) = 0;
	/**
	* Get global sprites and widgets tint color */
	virtual LcColor3 GetGlobalTint() const = 0;
	/**
	* Get last added visual */
	virtual class IVisual* GetLastAddedVisual() const = 0;
	/**
	* Get visual helper */
	virtual const class LcVisualHelper& GetVisualHelper() const = 0;
	/**
	* Get sprite helper */
	virtual const class LcSpriteHelper& GetSpriteHelper() const = 0;
	/**
	* Get widget helper */
	virtual const class LcWidgetHelper& GetWidgetHelper() const = 0;

};
