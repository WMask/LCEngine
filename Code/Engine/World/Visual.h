/**
* Visual.h
* 03.02.2023
* (c) Denis Romakhov
*/

#pragma once

#include <set>
#include <memory>

#include "Core/LCTypesEx.h"


/**
* Visual component type */
enum class EVCType : int
{
	Tint,
	VertexColor,
	Texture,
	FrameAnimation
};


/** Visual feature list */
typedef std::set<EVCType> TVFeaturesList;


/**
* Visual component interface */
class IVisualComponent
{
public:
	/**
	* Constructor */
	IVisualComponent() : owner(nullptr) {}
	/**
	* Virtual destructor */
	virtual ~IVisualComponent() {}
	/**
	* Update component */
	virtual void Update(float deltaSeconds) {}
	/**
	* Get type */
	virtual EVCType GetType() const = 0;
	/**
	* Set owner */
	inline void SetOwner(class IVisual* inOwner) { owner = inOwner; }
	/**
	* Get owner */
	inline class IVisual* GetOwner() const { return owner; }


protected:
	class IVisual* owner;
	friend class IVisual;

};


/** Visual component pointer */
typedef std::shared_ptr<IVisualComponent> TVComponentPtr;


/**
* Visual interface */
class IVisual
{
public:
	/**
	* Virtual destructor */
	virtual ~IVisual() {}
	/**
	* Update visual */
	virtual void Update(float deltaSeconds) {}
	/**
	* Render visual */
	virtual void PreRender() {}
	/**
	* Render visual */
	virtual void PostRender() {}
	/**
	* Add component */
	virtual void AddComponent(TVComponentPtr comp) = 0;
	/**
	* Get component */
	virtual TVComponentPtr GetComponent(EVCType type) const = 0;
	/**
	* Check for component type */
	virtual bool HasComponent(EVCType type) const = 0;
	/**
	* Return features list */
	virtual const TVFeaturesList& GetFeaturesList() const = 0;
	/**
	* Visual size in pixels */
	virtual void SetSize(LcSizef size) = 0;
	/**
	* Visual size in pixels */
	virtual LcSizef GetSize() const = 0;
	/**
	* Visual position in pixels */
	virtual void SetPos(LcVector3 pos) = 0;
	/**
	* Visual position in pixels */
	virtual void AddPos(LcVector3 pos) = 0;
	/**
	* Visual position in pixels */
	virtual LcVector3 GetPos() const = 0;
	/**
	* Visual rotation Z axis */
	virtual void SetRotZ(float rotZ) = 0;
	/**
	* Visual rotation Z axis */
	virtual void AddRotZ(float rotZ) = 0;
	/**
	* Visual rotation Z axis */
	virtual float GetRotZ() const = 0;
	/**
	* Visual state */
	virtual void SetVisible(bool visible) = 0;
	/**
	* Visual state */
	virtual bool IsVisible() const = 0;
	/**
	* Mouse button event */
	virtual void OnMouseButton(LcMouseBtn btn, LcKeyState state, int x, int y) = 0;
	/**
	* Mouse move event */
	virtual void OnMouseMove(int x, int y) = 0;

};
