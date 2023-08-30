/**
* GUIManager.cpp
* 29.01.2023
* (c) Denis Romakhov
*/

#include "pch.h"
#include "GUI/GUIManager.h"
#include "RenderSystem/RenderSystem.h"
#include "World/WorldInterface.h"
#include "World/Widgets.h"
#include "Core/LCUtils.h"


void LcGuiManagerBase::Init(TWeakWorld weakWorld, TWeakRenderSystem weakRender, void* window)
{
    worldPtr = weakWorld;
    renderPtr = weakRender;
    auto world = worldPtr.lock();
    auto render = renderPtr.lock();
    auto widgetRender = render ? render->GetWidgetRender() : nullptr;

    if (world && widgetRender)
    {
        auto& widgetList = world->GetWidgets();

        for (auto& widget : widgetList)
        {
            widget->Init(*widgetRender);
        }
    }
}

void LcGuiManagerBase::Update(float DeltaSeconds)
{
    if (auto world = worldPtr.lock())
    {
        auto& widgetList = world->GetWidgets();
        if (!widgetList.empty())
        {
            auto render = renderPtr.lock();
            auto widgetRender = render ? render->GetWidgetRender() : nullptr;

            for (auto& widget : widgetList)
            {
                if (!widget->IsInitialized() && widgetRender) widget->Init(*widgetRender);

                if (widget->IsVisible()) widget->Update(DeltaSeconds);
            }
        }
    }
}

void LcGuiManagerBase::Render()
{
    auto world = worldPtr.lock();
    auto render = renderPtr.lock();
    auto widgetRender = render ? render->GetWidgetRender() : nullptr;

    if (world && widgetRender)
    {
        auto& widgetList = world->GetWidgets();
        if (!widgetList.empty())
        {
            PreRender(*render.get());

            for (auto& widget : widgetList)
            {
                if (widget->IsVisible()) widget->Render(*widgetRender);
            }

            PostRender(*render.get());
        }
    }
}

void LcGuiManagerBase::PreRender(class IRenderSystem& render)
{
    if (auto widgetRender = render.GetWidgetRender()) widgetRender->BeginRender();
}

void LcGuiManagerBase::PostRender(class IRenderSystem& render)
{
    if (auto widgetRender = render.GetWidgetRender()) widgetRender->EndRender();
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

TGuiManagerPtr GetGuiManager()
{
    return std::make_shared<LcGuiManagerBase>();
}
