/**
* ClickMoveBehavior.cpp
* 31.01.2023
* (c) Denis Romakhov
*/

#include "framework.h"
#include "ClickMoveBehavior.h"
#include "MainMenuViewModel.h"


Noesis::Ptr<Noesis::Freezable> ClickMoveBehavior::CreateInstanceCore() const
{
    return *new ClickMoveBehavior();
}

void ClickMoveBehavior::OnAttached()
{
    Noesis::FrameworkElement* obj = GetAssociatedObject();
    obj->MouseLeftButtonDown() += MakeDelegate(this, &ClickMoveBehavior::OnMouseLeftButtonDown);
}

void ClickMoveBehavior::OnDetaching()
{
    Noesis::FrameworkElement* obj = GetAssociatedObject();
    obj->MouseLeftButtonDown() -= MakeDelegate(this, &ClickMoveBehavior::OnMouseLeftButtonDown);
}

void ClickMoveBehavior::OnMouseLeftButtonDown(BaseComponent*, const Noesis::MouseButtonEventArgs& args)
{
    Noesis::FrameworkElement* obj = GetAssociatedObject();
    MainMenuViewModel* viewModel = Noesis::DynamicCast<MainMenuViewModel*>(obj->GetParent()->GetParent()->GetParent()->GetDataContext());
    if (viewModel)
    {
        viewModel->SetPosX(args.position.x);
        viewModel->SetPosY(args.position.y);
    }
}

NS_IMPLEMENT_REFLECTION(ClickMoveBehavior, "common.ClickMoveBehavior")
{
    NS_UNUSED(helper);
}
