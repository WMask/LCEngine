/**
* NoesisWidget.cpp
* 04.02.2023
* (c) Denis Romakhov
*/

#include "pch.h"
#include "GUI/LCNoesisGUI/NoesisWidget.h"
#include "Core/LCUtils.h"

#include <NsGui/IntegrationAPI.h>
#include <NsApp/Interaction.h>


Noesis::Key MapKeyboardKeys(int btn)
{
    return Noesis::Key_None;
}

Noesis::MouseButton MapMouseKeys(LcMouseBtn btn)
{
    switch (btn)
    {
    case LcMouseBtn::Right: return Noesis::MouseButton_Right;
    case LcMouseBtn::Middle: return Noesis::MouseButton_Middle;
    }

    return Noesis::MouseButton_Left;
}

void LcNoesisWidget::OnKeyboard(int btn, LcKeyState state)
{
    if (view)
    {
        switch (state)
        {
        case LcKeyState::Down:
            view->KeyDown(MapKeyboardKeys(btn));
            break;
        case LcKeyState::Up:
            view->KeyUp(MapKeyboardKeys(btn));
            break;
        }
    }
}

void LcNoesisWidget::OnMouseButton(LcMouseBtn btn, LcKeyState state, int x, int y)
{
    if (view)
    {
        switch (state)
        {
        case LcKeyState::Down:
            view->MouseButtonDown(x, y, MapMouseKeys(btn));
            break;
        case LcKeyState::Up:
            view->MouseButtonUp(x, y, MapMouseKeys(btn));
            break;
        }
    }
}

void LcNoesisWidget::OnMouseMove(int x, int y)
{
    if (view) view->MouseMove(x, y);
}

std::shared_ptr<IWidget> LcNoesisWidgetFactory::Build(const LcWidgetData& data)
{
    LcNoesisWidget* newWidget = new LcNoesisWidget(data);
    std::shared_ptr<IWidget> newWidgetPtr(newWidget);

    if (newWidget->control = Noesis::GUI::LoadXaml<Noesis::UserControl>(data.name.c_str()))
    {
        newWidget->view = Noesis::GUI::CreateView(newWidget->control);
        newWidget->view->SetFlags(Noesis::RenderFlags_PPAA | Noesis::RenderFlags_LCD);
        newWidget->SetSize(LcSizef(newWidget->control->GetWidth(), newWidget->control->GetHeight()));
    }
    else
    {
        throw std::exception("LcNoesisWidgetFactory::Build(): Cannot create widget");
    }

    return newWidgetPtr;
}

IWidgetFactoryPtr GetWidgetFactory()
{
    return IWidgetFactoryPtr(new LcNoesisWidgetFactory());
}
