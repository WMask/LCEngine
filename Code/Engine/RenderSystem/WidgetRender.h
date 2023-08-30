/**
* WidgetRender.h
* 29.08.2023
* (c) Denis Romakhov
*/

#pragma once

#include "World/Widgets.h"


/**
* Widget renderer interface */
class IWidgetRender
{
public:
	/**
	* Add font */
	virtual const ITextFont* AddFont(const std::wstring& fontName, unsigned short fontSize, LcFontWeight fontWeight = LcFontWeight::Normal) = 0;
	/**
	* Remove font */
	virtual bool RemoveFont(const ITextFont* font) = 0;
	/**
	* Render text */
	virtual void RenderText(const std::wstring& text, const LcRectf& rect, const LcColor4& color, const ITextFont* font) = 0;

};
