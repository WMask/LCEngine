/**
* MainMenuViewModel.cpp
* 31.01.2023
* (c) Denis Romakhov
*/

#include "framework.h"
#include "MainMenuViewModel.h"

#include <NsGui/DependencyData.h>


const Noesis::DependencyProperty* MainMenuViewModel::PosXProperty;
const Noesis::DependencyProperty* MainMenuViewModel::PosYProperty;

MainMenuViewModel::MainMenuViewModel()
{
}

MainMenuViewModel::~MainMenuViewModel()
{
}

bool MainMenuViewModel::OnPropertyChanged(const Noesis::DependencyPropertyChangedEventArgs& e)
{
    bool handled = ParentClass::OnPropertyChanged(e);

    if (!handled)
    {
        if (e.prop == PosXProperty)
        {
            float x = GetPosX();
            return true;
        }
        if (e.prop == PosYProperty)
        {
            float y = GetPosY();
            return true;
        }
    }

    return handled;
}

float MainMenuViewModel::GetPosX() const
{
    return GetValue<float>(PosXProperty);
}

float MainMenuViewModel::GetPosY() const
{
    return GetValue<float>(PosYProperty);
}

void MainMenuViewModel::SetPosX(float posX)
{
    SetValue<float>(PosXProperty, posX);
}

void MainMenuViewModel::SetPosY(float posY)
{
    SetValue<float>(PosYProperty, posY);
}

NS_IMPLEMENT_REFLECTION(MainMenuViewModel, "common.MainMenuViewModel")
{
    Noesis::DependencyData* data = NsMeta<Noesis::DependencyData>(Noesis::TypeOf<SelfClass>());

    data->RegisterProperty<float>(PosXProperty, "PosX", Noesis::PropertyMetadata::Create(100.0f));
    data->RegisterProperty<float>(PosYProperty, "PosY", Noesis::PropertyMetadata::Create(100.0f));
}
