/**
* SpriteRender.h
* 28.01.2023
* (c) Denis Romakhov
*/

#pragma once

#include "World/Sprites.h"
#include "RenderSystem.h"


/**
* Sprite renderer interface */
class ISpriteRender : public IVisual2DRender
{
public:
	virtual void RenderWidget(const class IWidget* widget, const LcAppContext& context) override {}


protected:
	virtual const struct ITextFont* AddFont(const std::wstring& fontName, unsigned short fontSize,
		LcFontWeight fontWeight) override { return nullptr; }
};
