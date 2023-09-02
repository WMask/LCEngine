/**
* Widgets.cpp
* 31.08.2023
* (c) Denis Romakhov
*/

#pragma once

#include "pch.h"
#include "GUI/Widgets.h"


LcWidgetButtonComponent::LcWidgetButtonComponent(const LcWidgetButtonComponent& button) : state(EBtnState::Idle)
{
    memcpy(idle, button.idle, sizeof(LcVector4) * 4);
    memcpy(over, button.over, sizeof(LcVector4) * 4);
    memcpy(pressed, button.pressed, sizeof(LcVector4) * 4);
}

LcWidgetButtonComponent::LcWidgetButtonComponent(LcVector2 idlePos, LcVector2 overPos, LcVector2 pressedPos) : state(EBtnState::Idle)
{
    idle[0].x = idlePos.x;
    idle[0].y = idlePos.y;

    over[0].x = overPos.x;
    over[0].y = overPos.y;

    pressed[0].x = pressedPos.x;
    pressed[0].y = pressedPos.y;
}

void LcWidgetButtonComponent::Init(class IWorld& world)
{
    IVisualComponent::Init(world);

    if (!owner) throw std::exception("LcWidgetButtonComponent::Init(): Invalid owner");

    auto texComp = (LcVisualTextureComponent*)owner->GetComponent(EVCType::Texture).get();
    if (!texComp) throw std::exception("LcWidgetButtonComponent::Init(): No texture component found");

    LcSizef size = owner->GetSize();
    LcSizef texSize = texComp->texSize;
    LcVector2 idlePos{ idle[0].x, idle[0].y };
    LcVector2 overPos{ over[0].x, over[0].y };
    LcVector2 pressedPos{ pressed[0].x, pressed[0].y };

    // generate UVs
    idle[0] = To4(LcVector2( idlePos.x                  / texSize.x,  idlePos.y                 / texSize.y));
    idle[1] = To4(LcVector2((idlePos.x + size.x)        / texSize.x,  idlePos.y                 / texSize.y));
    idle[2] = To4(LcVector2((idlePos.x + size.x)        / texSize.x, (idlePos.y + size.y)       / texSize.y));
    idle[3] = To4(LcVector2( idlePos.x                  / texSize.x, (idlePos.y + size.y)       / texSize.y));

    over[0] = To4(LcVector2( overPos.x                  / texSize.x,  overPos.y                 / texSize.y));
    over[1] = To4(LcVector2((overPos.x + size.x)        / texSize.x,  overPos.y                 / texSize.y));
    over[2] = To4(LcVector2((overPos.x + size.x)        / texSize.x, (overPos.y + size.y)       / texSize.y));
    over[3] = To4(LcVector2( overPos.x                  / texSize.x, (overPos.y + size.y)       / texSize.y));

    pressed[0] = To4(LcVector2( pressedPos.x            / texSize.x,  pressedPos.y              / texSize.y));
    pressed[1] = To4(LcVector2((pressedPos.x + size.x)  / texSize.x,  pressedPos.y              / texSize.y));
    pressed[2] = To4(LcVector2((pressedPos.x + size.x)  / texSize.x, (pressedPos.y + size.y)    / texSize.y));
    pressed[3] = To4(LcVector2( pressedPos.x            / texSize.x, (pressedPos.y + size.y)    / texSize.y));
}

const void* LcWidgetButtonComponent::GetData() const
{
    switch (state)
    {
    case EBtnState::Over: return over;
    case EBtnState::Pressed: return pressed;
    }

    return idle;
}

void LcWidget::OnMouseButton(LcMouseBtn btn, LcKeyState state, int x, int y)
{
    if (auto button = GetButtonComponent())
    {
        if (state == LcKeyState::Down)
        {
            button->state = EBtnState::Pressed;
        }
        else
        {
            button->state = IsHovered() ? EBtnState::Over : EBtnState::Idle;
        }
    }
}

void LcWidget::OnMouseEnter()
{
    if (auto button = GetButtonComponent())
    {
        button->state = EBtnState::Over;
    }
}

void LcWidget::OnMouseLeave()
{
    if (auto button = GetButtonComponent())
    {
        button->state = EBtnState::Idle;
    }
}
