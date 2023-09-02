/**
* WidgetRender.h
* 31.01.2023
* (c) Denis Romakhov
*/

#pragma once

#include "GUI/Widgets.h"
#include "RenderSystem.h"


/**
* Widget renderer interface */
class IWidgetRender : public IVisual2DRender
{
public:
	virtual void RenderSprite(const class ISprite* sprite) override {}
	//
	virtual EWRMode GetRenderMode() const = 0;
	//
	virtual void SetRenderMode(EWRMode inRenderMode) = 0;


protected:
	virtual bool Supports(const TVFeaturesList& features) const override { return false; }

};
