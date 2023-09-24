/**
* WidgetRender.h
* 31.01.2023
* (c) Denis Romakhov
*/

#pragma once

#include "GUI/WidgetInterface.h"
#include "RenderSystem.h"


/**
* Widget renderer interface */
class IWidgetRender : public IVisual2DRender
{
public:
	/**
	* Add font */
	virtual const struct ITextFont* AddFont(const std::wstring& fontName, float fontSize, LcFontWeight fontWeight = LcFontWeight::Normal) = 0;
	/**
	* Remove font */
	virtual bool RemoveFont(const ITextFont* font) = 0;


public: // IVisual2DRender interface implementation
	//
	virtual void RenderSprite(const class ISprite* sprite, const LcAppContext& context) override {}
	//
	virtual bool Supports(const TVFeaturesList& features) const override { return false; }

};
