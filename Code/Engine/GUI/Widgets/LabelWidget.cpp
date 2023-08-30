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
	LcWidgetBase::Init(render);

	font = render.AddFont(fontName, fontSize, fontWeight);
}

void LcLabelWidget::Render(class IWidgetRender& render) const
{
	if (IsInitialized())
	{
		LcRectf rect{
			widget.pos.x - size.x / 2.0f,
			widget.pos.y - size.y / 2.0f,
			widget.pos.x + size.x / 2.0f,
			widget.pos.y + size.x / 2.0f
		};
		render.RenderText(text, rect, textColor, font);
	}
}
