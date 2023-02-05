/**
* Widgets.h
* 03.02.2023
* (c) Denis Romakhov
*/

#pragma once

#include "Visual.h"


/**
* Widget data */
struct LcWidgetData
{
	std::string name;
	LcVector3 pos;
	bool visible;
	bool active;
	//
	LcWidgetData() : visible(true), active(false) {}
	//
	LcWidgetData(const std::string& inName, LcVector3 inPos, bool inVisible = true, bool inActive = false)
		: name(inName), pos(inPos), visible(inVisible), active(inActive)
	{
	}
};


/**
* Widget interface */
class IWidget : public IVisual
{
public:
	/**
	* Initialize widget */
	virtual void Init(LcSize viewportSize) = 0;
	/**
	* Widget name */
	virtual const std::string& GetName() const = 0;
	/**
	* Active state */
	virtual void SetActive(bool active) = 0;
	/**
	* Active state */
	virtual bool IsActive() const = 0;
	/**
	* Keyboard key event */
	virtual void OnKeyboard(int btn, LcKeyState state) = 0;

};
