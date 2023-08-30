/**
* GUIManager.cpp
* 29.01.2023
* (c) Denis Romakhov
*/

#include "pch.h"
#include "GUI/GUIManager.h"
#include "World/WorldInterface.h"
#include "World/Widgets.h"
#include "Core/LCUtils.h"


void LcGuiManagerBase::Init(TWeakWorld weakWorld, class IWidgetRender* render, void* window)
{
    if (!render) throw std::exception("LcGuiManagerBase::Init(): Invalid widget renderer");

    worldPtr = weakWorld;

    if (auto world = worldPtr.lock())
    {
        auto& widgetList = world->GetWidgets();

        for (auto& widget : widgetList)
        {
            widget->Init(*render);
        }
    }
}

void LcGuiManagerBase::Update(float DeltaSeconds)
{
    if (auto world = worldPtr.lock())
    {
        auto& widgetList = world->GetWidgets();

        for (auto& widget : widgetList)
        {
            if (widget->IsVisible()) widget->Update(DeltaSeconds);
        }
    }
}

void LcGuiManagerBase::PreRender()
{
    if (auto world = worldPtr.lock())
    {
        auto& widgetList = world->GetWidgets();

        for (auto& widget : widgetList)
        {
            if (widget->IsVisible()) widget->PreRender();
        }
    }
}

void LcGuiManagerBase::PostRender()
{
    if (auto world = worldPtr.lock())
    {
        auto& widgetList = world->GetWidgets();

        for (auto& widget : widgetList)
        {
            if (widget->IsVisible()) widget->PostRender();
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

        for (auto& widget : widgetList)
        {
            if (!widget->IsVisible()) continue;

            LcVector2 clickPt((float)x, (float)y);
            LcVector2 widgetPos = To2(widget->GetPos());
            LcRectf widgetBox = ToF(widgetPos, widgetPos + widget->GetSize());
            if (Contains(widgetBox, clickPt))
            {
                auto result = ToI(clickPt);
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

        for (auto& widget : widgetList)
        {
            if (!widget->IsVisible()) continue;

            LcVector2 clickPt((float)x, (float)y);
            LcVector2 widgetPos = To2(widget->GetPos());
            LcRectf widgetBox = ToF(widgetPos, widgetPos + widget->GetSize());
            if (Contains(widgetBox, clickPt))
            {
                auto result = ToI(clickPt);
                widget->OnMouseMove(result.x, result.y);
            }
        }
    }
}
