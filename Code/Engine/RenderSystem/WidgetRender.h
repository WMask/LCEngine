/**
* WidgetRender.h
* 31.01.2023
* (c) Denis Romakhov
*/

#pragma once

#include "GUI/Widgets.h"

/**
* Widget renderer interface */
class IWidgetRender
{
public:
	/**
	* Add font */
	virtual const ITextFont* AddFont(const std::wstring& fontName, unsigned short fontSize, LcFontWeight fontWeight = LcFontWeight::Normal) = 0;


protected:
	/**
	* Setup render state */
	virtual void Setup() = 0;
	/**
	* Render widget */
	virtual void Render(const class IWidget* widget) = 0;

};
