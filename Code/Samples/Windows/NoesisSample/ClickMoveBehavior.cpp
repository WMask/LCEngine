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


const Noesis::DependencyProperty* ClickMoveBehavior::MoveTargetProperty;

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
            viewModel->SetPosX(args.position.x);
            viewModel->SetPosY(args.position.y);
        }
    }

    if (auto redBox = GetMoveTarget())
    {
        redBox->SetValue<float>(Noesis::Canvas::LeftProperty, args.position.x);
        redBox->SetValue<float>(Noesis::Canvas::TopProperty, args.position.y);
    }
}

bool ClickMoveBehavior::OnPropertyChanged(const Noesis::DependencyPropertyChangedEventArgs& e)
{
    bool handled = ParentClass::OnPropertyChanged(e);

    if (!handled)
    {
        if (e.prop == MoveTargetProperty)
        {
            Noesis::FrameworkElement* elem = GetMoveTarget();
            return true;
        }
    }

    return handled;
}

Noesis::FrameworkElement* ClickMoveBehavior::GetMoveTarget() const
{
    return GetValue<Noesis::FrameworkElement*>(MoveTargetProperty);
}

NS_IMPLEMENT_REFLECTION(ClickMoveBehavior, "common.ClickMoveBehavior")
{
    Noesis::DependencyData* data = NsMeta<Noesis::DependencyData>(Noesis::TypeOf<SelfClass>());

    data->RegisterProperty<Noesis::Ptr<Noesis::FrameworkElement>>(MoveTargetProperty, "MoveTarget",
        Noesis::PropertyMetadata::Create(Noesis::Ptr<Noesis::FrameworkElement>()));
}
