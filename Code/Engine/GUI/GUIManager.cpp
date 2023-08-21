/**
* GUIManager.cpp
* 29.01.2023
* (c) Denis Romakhov
*/

#include "pch.h"
#include "GUI/GUIManager.h"
#include "World/WorldInterface.h"
#include "Core/LCUtils.h"


void LcGuiManagerBase::Init(void* window, LcSize viewportSize)
{
    auto world = GetWorld();
    auto& widgetList = world->GetWidgets();

    for (auto& widget : widgetList)
    {
        widget->Init();
    }
}

void LcGuiManagerBase::Update(float DeltaSeconds)
{
    auto world = GetWorld();
    auto& widgetList = world->GetWidgets();

    for (auto& widget : widgetList)
    {
        if (widget->IsVisible()) widget->Update(DeltaSeconds);
    }
}

void LcGuiManagerBase::PreRender()
{
    auto world = GetWorld();
    auto& widgetList = world->GetWidgets();

    for (auto& widget : widgetList)
    {
        if (widget->IsVisible()) widget->PreRender();
    }
}

void LcGuiManagerBase::PostRender()
{
    auto world = GetWorld();
    auto& widgetList = world->GetWidgets();

    for (auto& widget : widgetList)
    {
        if (widget->IsVisible()) widget->PostRender();
    }
}

void LcGuiManagerBase::OnKeyboard(int btn, LcKeyState state)
{
    auto world = GetWorld();
    auto& widgetList = world->GetWidgets();

    for (auto& widget : widgetList)
    {
        if (widget->IsVisible() && widget->IsActive()) widget->OnKeyboard(btn, state);
    }
}

void LcGuiManagerBase::OnMouseButton(LcMouseBtn btn, LcKeyState state, int x, int y)
{
    auto world = GetWorld();
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

void LcGuiManagerBase::OnMouseMove(int x, int y)
{
    auto world = GetWorld();
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
