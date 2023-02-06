/**
* NoesisWidget.cpp
* 04.02.2023
* (c) Denis Romakhov
*/

#include "pch.h"
#include "GUI/NoesisGUI/NoesisWidget.h"
#include "GUI/NoesisGUI/NoesisRenderContextD3D10.h"
#include "Core/LCUtils.h"

#include <NsGui/IntegrationAPI.h>
#include <NsApp/Interaction.h>
#include <NsGui/IRenderer.h>
#include <NsDrawing/Thickness.h>
#include <NsRender/RenderContext.h>
#include <NsRender/RenderDevice.h>


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


LcNoesisWidget::LcNoesisWidget(LcWidgetData inWidget, class NoesisApp::RenderContext* inContext)
    : widget(inWidget)
    , context(inContext)
{
}

LcNoesisWidget::~LcNoesisWidget()
{
    if (view)
    {
        view->GetRenderer()->Shutdown();
        view.Reset();
    }
}

void LcNoesisWidget::Init()
{
    if (auto device = context ? context->GetDevice() : nullptr)
    {
        if (view)
        {
            view->GetRenderer()->Init(device);

            LcSize viewportSize(device->GetOffscreenWidth(), device->GetOffscreenHeight());
            view->SetSize(viewportSize.x(), viewportSize.y());
            size = ToF(viewportSize);
        }

        SetPos(widget.pos);
    }
}

void LcNoesisWidget::SetSize(LcSizef inSize)
{
    size = inSize;

    if (view)
    {
        auto sizeI = ToI(size);
        view->SetSize(sizeI.x(), sizeI.y());
    }
}

void LcNoesisWidget::SetPos(LcVector3 inPos)
{
    widget.pos = inPos;

    if (control)
    {
        control->SetMargin(Noesis::Thickness(widget.pos.x(), widget.pos.y(), 0.0f, 0.0f));
    }
}

void LcNoesisWidget::AddPos(LcVector3 inPos)
{
    widget.pos += inPos;

    if (control)
    {
        control->SetMargin(Noesis::Thickness(widget.pos.x(), widget.pos.y(), 0.0f, 0.0f));
    }
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

void LcNoesisWidget::Update(float DeltaSeconds)
{
    if (view) view->Update(DeltaSeconds);
}

void LcNoesisWidget::PreRender()
{
    if (view && view->GetRenderer())
    {
        if (view->GetRenderer()->UpdateRenderTree())
        {
            view->GetRenderer()->RenderOffscreen();
        }
    }
}

void LcNoesisWidget::PostRender()
{
    if (view && view->GetRenderer()) view->GetRenderer()->Render();
}

std::shared_ptr<IWidget> LcNoesisWidgetFactory::Build(const LcWidgetData& data)
{
    LcNoesisWidget* newWidget = new LcNoesisWidget(data, context);
    std::shared_ptr<IWidget> newWidgetPtr(newWidget);

    if (newWidget->control = Noesis::GUI::LoadXaml<Noesis::UserControl>(data.name.c_str()))
    {
        newWidget->view = Noesis::GUI::CreateView(newWidget->control);
        newWidget->view->SetFlags(Noesis::RenderFlags_PPAA | Noesis::RenderFlags_LCD);
        newWidget->Init();
    }
    else
    {
        throw std::exception("LcNoesisWidgetFactory::Build(): Cannot create widget");
    }

    return newWidgetPtr;
}

IWidgetFactoryPtr GetWidgetFactory()
{
    return IWidgetFactoryPtr(new LcNoesisWidgetFactory(GetContext()));
}
