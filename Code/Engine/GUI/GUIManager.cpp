/**
* GUIManager.cpp
* 29.01.2023
* (c) Denis Romakhov
*/

#include "pch.h"
#include "GUI/Widgets.h"
#include "GUI/GUIManager.h"
#include "RenderSystem/RenderSystem.h"
#include "World/WorldInterface.h"
#include "Core/LCException.h"
#include "Core/LCUtils.h"


void LcGuiManager::Update(float deltaSeconds, const LcAppContext& context)
{
    LC_TRY

    auto& widgetList = context.world.GetWidgets();
    if (!widgetList.empty())
    {
        for (auto& widget : widgetList)
        {
            if (widget->IsVisible()) widget->Update(deltaSeconds, context);
        }
    }

    LC_CATCH{ LC_THROW("LcGuiManager::Update()") }
}

void LcGuiManager::OnKeyboard(int btn, LcKeyState state, const LcAppContext& context)
{
    LC_TRY

    auto& widgetList = context.world.GetWidgets();
    for (auto& widget : widgetList)
    {
        if (widget->IsVisible() && !widget->IsDisabled()) widget->OnKeyboard(btn, state, context);
    }

    LC_CATCH{ LC_THROW("LcGuiManager::OnKeyboard()") }
}

void LcGuiManager::OnMouseButton(LcMouseBtn btn, LcKeyState state, int x, int y, const LcAppContext& context)
{
    LC_TRY

    auto& widgetList = context.world.GetWidgets();
    auto scale2 = context.world.GetWorldScale().scale;
    auto scale3 = LcVector3(scale2.x, scale2.y, 1.0f);

    for (auto& widget : widgetList)
    {
        if (!widget->IsVisible()) continue;

        LcVector2 point((float)x, (float)y);
        LcVector2 widgetPos = To2(widget->GetPos() * scale3);
        LcRectf widgetBox = ToF(
            widgetPos - widget->GetSize() / 2.0f * scale2,
            widgetPos + widget->GetSize() / 2.0f * scale2);
        if (Contains(widgetBox, point))
        {
            auto result = ToI(point);
            widget->OnMouseButton(btn, state, result.x, result.y, context);
        }
    }

    LC_CATCH{ LC_THROW("LcGuiManager::OnMouseButton()") }
}

void LcGuiManager::OnMouseMove(int x, int y, const LcAppContext& context)
{
    LC_TRY

    auto& widgetList = context.world.GetWidgets();
    auto scale2 = context.world.GetWorldScale().scale;
    auto scale3 = LcVector3(scale2.x, scale2.y, 1.0f);

    for (auto& widget : widgetList)
    {
        if (!widget->IsVisible()) continue;

        LcVector2 point((float)x, (float)y);
        LcVector2 widgetPos = To2(widget->GetPos() * scale3);
        LcRectf widgetBox = ToF(
            widgetPos - widget->GetSize() / 2.0f * scale2,
            widgetPos + widget->GetSize() / 2.0f * scale2);

        if (Contains(widgetBox, point))
        {
            if (!widget->IsHovered())
            {
                widget->SetHovered(true);
                widget->OnMouseEnter(context);
            }

            widget->OnMouseMove(To3(point), context);
        }
        else
        {
            if (widget->IsHovered())
            {
                widget->SetHovered(false);
                widget->OnMouseLeave(context);
            }
        }
    }

    LC_CATCH{ LC_THROW("LcGuiManager::OnMouseMove()") }
}

TGuiManagerPtr GetGuiManager()
{
    return std::make_shared<LcGuiManager>();
}
