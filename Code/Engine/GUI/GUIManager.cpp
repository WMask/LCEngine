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
#include "Core/LCUtils.h"


void LcGuiManagerBase::Init(TWeakWorld weakWorld)
{
    worldPtr = weakWorld;
}

void LcGuiManagerBase::Update(float DeltaSeconds)
{
    if (auto world = worldPtr.lock())
    {
        auto& widgetList = world->GetWidgets();
        if (!widgetList.empty())
        {
            for (auto& widget : widgetList)
            {
                if (widget->IsVisible()) widget->Update(DeltaSeconds);
            }
        }
    }
}

void LcGuiManagerBase::OnKeyboard(int btn, LcKeyState state)
{
    if (auto world = worldPtr.lock())
    {
        auto& widgetList = world->GetWidgets();

        for (auto& widget : widgetList)
        {
            if (widget->IsVisible() && widget->IsActive()) widget->OnKeyboard(btn, state);
        }
    }
}

void LcGuiManagerBase::OnMouseButton(LcMouseBtn btn, LcKeyState state, int x, int y)
{
    if (auto world = worldPtr.lock())
    {
        auto& widgetList = world->GetWidgets();
        auto scale2 = world->GetWorldScale().scale;
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
                widget->OnMouseButton(btn, state, result.x, result.y);
            }
        }
    }
}

void LcGuiManagerBase::OnMouseMove(int x, int y)
{
    if (auto world = worldPtr.lock())
    {
        auto& widgetList = world->GetWidgets();
        auto scale2 = world->GetWorldScale().scale;
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
                    widget->OnMouseEnter();
                }

                widget->OnMouseMove(To3(point));
            }
            else
            {
                if (widget->IsHovered())
                {
                    widget->SetHovered(false);
                    widget->OnMouseLeave();
                }
            }
        }
    }
}

TGuiManagerPtr GetGuiManager()
{
    return std::make_shared<LcGuiManagerBase>();
}
