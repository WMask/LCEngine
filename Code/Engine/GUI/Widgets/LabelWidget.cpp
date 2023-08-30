/**
* LabelWidget.cpp
* 28.08.2023
* (c) Denis Romakhov
*/

#include "pch.h"
#include "GUI/Widgets/LabelWidget.h"
#include "RenderSystem/WidgetRender.h"


void LcLabelWidget::Init(class IWidgetRender& render)
{
	render.AddFont(fontName, fontSize, fontWeight);
}

void LcLabelWidget::Render(class IWidgetRender& render) const
{
	LcRectf rect{widget.pos.x, widget.pos.y, widget.pos.x + size.x, widget.pos.y + size.y };
	render.RenderText(text, rect, textColor, font);
}
