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
	//
	LcWidgetData() : visible(true) {}
	//
	LcWidgetData(const std::string& inName, LcVector3 inPos, bool inVisible = true)
		: name(inName), pos(inPos), visible(inVisible)
	{
	}
};


/**
* Widget interface */
class IWidget : public IVisual
{
public:
	/**
	* Widget name */
	virtual const std::string& GetName() const = 0;

};
