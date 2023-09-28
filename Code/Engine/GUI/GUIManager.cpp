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


bool HasInvisibleParent(IWidget* widget)
{
    if (widget)
    {
        if (widget->GetParent())
        {
            if (!widget->GetParent()->IsVisible())
                return true;
            else
                return HasInvisibleParent(widget->GetParent());
        }
    }

    return false;
}

void LcGuiManager::OnKeys(int btn, LcKeyState state, const LcAppContext& context)
{
    LC_TRY

    auto& visuals = context.world->GetVisuals();
    for (auto& visual : visuals)
    {
        if ((visual->GetTypeId() != LcCreatables::Widget) ||
            !visual->IsVisible())
        {
            continue;
        }

        auto widget = static_cast<IWidget*>(visual.get());
        if (HasInvisibleParent(widget)) continue;

        if (widget->HasFocus() && !widget->IsDisabled())
        {
            widget->OnKeys(btn, state, context);
        }
    }

    LC_CATCH{ LC_THROW("LcGuiManager::OnKeys()") }
}

void LcGuiManager::OnMouseButton(LcMouseBtn btn, LcKeyState state, int x, int y, const LcAppContext& context)
{
    LC_TRY

    auto& visuals = context.world->GetVisuals();
    auto scale2 = context.world->GetWorldScale().GetScale();
    auto scale3 = LcVector3(scale2.x, scale2.y, 1.0f);

    for (auto& visual : visuals)
    {
        if ((visual->GetTypeId() != LcCreatables::Widget) ||
            !visual->IsVisible())
        {
            continue;
        }

        auto widget = static_cast<IWidget*>(visual.get());
        if (HasInvisibleParent(widget)) continue;

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

    auto& visuals = context.world->GetVisuals();
    auto scale2 = context.world->GetWorldScale().GetScale();
    auto scale3 = LcVector3(scale2.x, scale2.y, 1.0f);

    for (auto& visual : visuals)
    {
        if ((visual->GetTypeId() != LcCreatables::Widget) ||
            !visual->IsVisible())
        {
            continue;
        }

        auto widget = static_cast<IWidget*>(visual.get());
        if (HasInvisibleParent(widget)) continue;

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
