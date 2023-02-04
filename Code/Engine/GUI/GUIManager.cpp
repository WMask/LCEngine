/**
* GUIManager.cpp
* 29.01.2023
* (c) Denis Romakhov
*/

#include "pch.h"
#include "GUI/GUIManager.h"
#include "World/Module.h"


void IGuiManager::OnKeyboard(int btn, LcKeyState state)
{
    auto world = GetWorld();
    auto& widgetList = world->GetWidgets();

    for (auto& widget : widgetList)
    {
        if (widget->IsVisible() && widget->IsActive()) widget->OnKeyboard(btn, state);
    }
}

void IGuiManager::OnMouseButton(LcMouseBtn btn, LcKeyState state, int x, int y)
{
    auto world = GetWorld();
    auto& widgetList = world->GetWidgets();

    for (auto& widget : widgetList)
    {
        if (!widget->IsVisible()) continue;

        LcVector2 pos2d(widget->GetPos().x(), widget->GetPos().z());
        LcRectf widgetBox(pos2d, pos2d + widget->GetSize());
        if (widgetBox.contains(LcVector2((float)x, (float)y)))
        {
            int localX = x - (int)round(pos2d.x());
            int localY = y - (int)round(pos2d.y());
            widget->OnMouseButton(btn, state, localX, localY);
        }
    }
}

void IGuiManager::OnMouseMove(int x, int y)
{
    auto world = GetWorld();
    auto& widgetList = world->GetWidgets();

    for (auto& widget : widgetList)
    {
        if (!widget->IsVisible()) continue;

        LcVector2 pos2d(widget->GetPos().x(), widget->GetPos().z());
        LcRectf widgetBox(pos2d, pos2d + widget->GetSize());
        if (widgetBox.contains(LcVector2((float)x, (float)y)))
        {
            int localX = x - (int)round(pos2d.x());
            int localY = y - (int)round(pos2d.y());
            widget->OnMouseMove(localX, localY);
        }
    }
}
