/**
* ClickMoveBehavior.cpp
* 31.01.2023
* (c) Denis Romakhov
*/

#include "framework.h"
#include "ClickMoveBehavior.h"
#include "MainMenuViewModel.h"

#include <NsGui/DependencyData.h>
#include <NsGui/Canvas.h>
#include <NsDrawing/Thickness.h>


Noesis::Ptr<Noesis::Freezable> ClickMoveBehavior::CreateInstanceCore() const
{
    return *new ClickMoveBehavior();
}

void ClickMoveBehavior::OnAttached()
{
    if (Noesis::FrameworkElement* obj = GetAssociatedObject())
    {
        obj->MouseLeftButtonDown() += MakeDelegate(this, &ClickMoveBehavior::OnMouseLeftButtonDown);
    }
}

void ClickMoveBehavior::OnDetaching()
{
    if (Noesis::FrameworkElement* obj = GetAssociatedObject())
    {
        obj->MouseLeftButtonDown() -= MakeDelegate(this, &ClickMoveBehavior::OnMouseLeftButtonDown);
    }
}

void ClickMoveBehavior::OnMouseLeftButtonDown(BaseComponent*, const Noesis::MouseButtonEventArgs& args)
{
    if (Noesis::FrameworkElement* obj = GetAssociatedObject())
    {
        MainMenuViewModel* viewModel = Noesis::DynamicCast<MainMenuViewModel*>(obj->GetDataContext());
        if (viewModel)
        {
            auto margin = obj->GetMargin();
            viewModel->SetPosX(args.position.x - margin.left);
            viewModel->SetPosY(args.position.y - margin.top);
        }
    }
}

NS_IMPLEMENT_REFLECTION(ClickMoveBehavior, "common.ClickMoveBehavior")
{
    NS_UNUSED(helper);
}
