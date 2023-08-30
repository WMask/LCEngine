/**
* WidgetRender.h
* 29.08.2023
* (c) Denis Romakhov
*/

#pragma once

#include "World/Widgets.h"


/**
* Text font */
struct ITextFont
{
public:
	virtual ~ITextFont() {}
	//
	virtual const std::wstring& GetFontName() const = 0;
};


/** Font weight */
enum class LcFontWeight { Light, Normal, Bold };


/**
* Widget renderer interface */
class IWidgetRender
{
public:
	/**
	* Add font */
	virtual const struct ITextFont* AddFont(const std::wstring& fontName, unsigned short fontSize, LcFontWeight fontWeight = LcFontWeight::Normal) = 0;
	/**
	* Remove font */
	virtual bool RemoveFont(const struct ITextFont* font) = 0;
	/**
	* Render text */
	virtual void RenderText(const std::wstring& text, const LcRectf& rect, const LcColor4& color, const ITextFont* font) = 0;
	/**
	* Begin render */
	virtual void BeginRender() = 0;
	/**
	* End render */
	virtual long EndRender() = 0;

};
