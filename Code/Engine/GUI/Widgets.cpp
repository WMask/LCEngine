/**
* Widgets.cpp
* 31.08.2023
* (c) Denis Romakhov
*/

#pragma once

#include "pch.h"
#include "GUI/Widgets.h"
#include "World/WorldInterface.h"


void LcWidgetHelper::AddTextComponent(const std::wstring& inText, LcColor4 inTextColor, const std::wstring& inFontName,
    unsigned short inFontSize, LcFontWeight inFontWeight, LcTextAlignment inTextAlign) const
{
    if (auto visual = context.world->GetLastAddedVisual())
    {
        visual->AddComponent(std::make_shared<LcWidgetTextComponent>(inText, inTextColor, inTextAlign, inFontName, inFontSize, inFontWeight), context);
    }
}

void LcWidgetHelper::AddAlignedTextComponent(const std::wstring& inText, LcColor4 inTextColor, LcTextAlignment inTextAlign, const std::wstring& inFontName,
    unsigned short inFontSize, LcFontWeight inFontWeight) const
{
    if (auto visual = context.world->GetLastAddedVisual())
    {
        visual->AddComponent(std::make_shared<LcWidgetTextComponent>(inText, inTextColor, inTextAlign, inFontName, inFontSize, inFontWeight), context);
    }
}

void LcWidgetHelper::AddButtonComponent(const std::string& texture, LcVector2 idlePos, LcVector2 overPos, LcVector2 pressedPos) const
{
    if (auto visual = context.world->GetLastAddedVisual())
    {
        AddTextureComponent(texture);
        visual->AddComponent(std::make_shared<LcWidgetButtonComponent>(idlePos, overPos, pressedPos), context);
    }
}

void LcWidgetHelper::AddCheckboxComponent(const std::string& texture, LcVector2 uncheckedPos, LcVector2 uncheckedHoveredPos,
    LcVector2 checkedPos, LcVector2 checkedHoveredPos) const
{
    if (auto visual = context.world->GetLastAddedVisual())
    {
        AddTextureComponent(texture);
        visual->AddComponent(std::make_shared<LcWidgetCheckboxComponent>(uncheckedPos, uncheckedHoveredPos, checkedPos, checkedHoveredPos), context);
    }
}

void LcWidgetHelper::AddClickHandlerComponent(LcClickHandler handler, bool addDefaultSkin) const
{
    if (auto visual = context.world->GetLastAddedVisual())
    {
        visual->AddComponent(std::make_shared<LcWidgetClickComponent>(handler), context);

        if (addDefaultSkin) AddButtonComponent("../../Assets/button.png",
            LcVector2(2.0f, 2.0f), LcVector2(2.0f, 44.0f), LcVector2(2.0f, 86.0f));
    }
}

void LcWidgetHelper::AddCheckHandlerComponent(LcCheckHandler handler, bool addDefaultSkin) const
{
    if (auto visual = context.world->GetLastAddedVisual())
    {
        visual->AddComponent(std::make_shared<LcWidgetCheckComponent>(handler), context);

        if (addDefaultSkin) AddCheckboxComponent("../../Assets/checkbox.png",
            LcVector2(0.0f, 0.0f), LcVector2(32.0f, 0.0f), LcVector2(0.0f, 32.0f), LcVector2(32.0f, 32.0f));
    }
}

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

void LcWidgetButtonComponent::Init(const LcAppContext& context)
{
    IVisualComponent::Init(context);

    if (auto texComp = owner ? owner->GetTextureComponent() : nullptr)
    {
        LcSizef size = owner->GetSize();
        LcSizef texSize = texComp->GetTextureSize();
        LcVector2 idlePos{ idle[0].x, idle[0].y };
        LcVector2 overPos{ over[0].x, over[0].y };
        LcVector2 pressedPos{ pressed[0].x, pressed[0].y };

        // generate UVs
        idle[0] = To4(LcVector2(idlePos.x / texSize.x, idlePos.y / texSize.y));
        idle[1] = To4(LcVector2((idlePos.x + size.x) / texSize.x, idlePos.y / texSize.y));
        idle[2] = To4(LcVector2((idlePos.x + size.x) / texSize.x, (idlePos.y + size.y) / texSize.y));
        idle[3] = To4(LcVector2(idlePos.x / texSize.x, (idlePos.y + size.y) / texSize.y));

        over[0] = To4(LcVector2(overPos.x / texSize.x, overPos.y / texSize.y));
        over[1] = To4(LcVector2((overPos.x + size.x) / texSize.x, overPos.y / texSize.y));
        over[2] = To4(LcVector2((overPos.x + size.x) / texSize.x, (overPos.y + size.y) / texSize.y));
        over[3] = To4(LcVector2(overPos.x / texSize.x, (overPos.y + size.y) / texSize.y));

        pressed[0] = To4(LcVector2(pressedPos.x / texSize.x, pressedPos.y / texSize.y));
        pressed[1] = To4(LcVector2((pressedPos.x + size.x) / texSize.x, pressedPos.y / texSize.y));
        pressed[2] = To4(LcVector2((pressedPos.x + size.x) / texSize.x, (pressedPos.y + size.y) / texSize.y));
        pressed[3] = To4(LcVector2(pressedPos.x / texSize.x, (pressedPos.y + size.y) / texSize.y));
    }
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

LcWidgetCheckboxComponent::LcWidgetCheckboxComponent(const LcWidgetCheckboxComponent& checkbox) : state(ECheckboxState::Unchecked)
{
    memcpy(unchecked, checkbox.unchecked, sizeof(LcVector4) * 4);
    memcpy(uncheckedH, checkbox.uncheckedH, sizeof(LcVector4) * 4);
    memcpy(checked, checkbox.checked, sizeof(LcVector4) * 4);
    memcpy(checkedH, checkbox.checkedH, sizeof(LcVector4) * 4);
}

LcWidgetCheckboxComponent::LcWidgetCheckboxComponent(
    LcVector2 uncheckedPos, LcVector2 uncheckedHoveredPos, LcVector2 checkedPos, LcVector2 checkedHoveredPos) : state(ECheckboxState::Unchecked)
{
    unchecked[0].x = uncheckedPos.x;
    unchecked[0].y = uncheckedPos.y;

    uncheckedH[0].x = uncheckedHoveredPos.x;
    uncheckedH[0].y = uncheckedHoveredPos.y;

    checked[0].x = checkedPos.x;
    checked[0].y = checkedPos.y;

    checkedH[0].x = checkedHoveredPos.x;
    checkedH[0].y = checkedHoveredPos.y;
}

void LcWidgetCheckboxComponent::Init(const LcAppContext& context)
{
    IVisualComponent::Init(context);

    if (auto texComp = owner ? owner->GetTextureComponent() : nullptr)
    {
        LcSizef size = owner->GetSize();
        LcSizef texSize = texComp->GetTextureSize();
        LcVector2 uncheckedPos{ unchecked[0].x, unchecked[0].y };
        LcVector2 uncheckedHPos{ uncheckedH[0].x, uncheckedH[0].y };
        LcVector2 checkedPos{ checked[0].x, checked[0].y };
        LcVector2 checkedHPos{ checkedH[0].x, checkedH[0].y };

        // generate UVs
        unchecked[0] = To4(LcVector2(uncheckedPos.x / texSize.x, uncheckedPos.y / texSize.y));
        unchecked[1] = To4(LcVector2((uncheckedPos.x + size.x) / texSize.x, uncheckedPos.y / texSize.y));
        unchecked[2] = To4(LcVector2((uncheckedPos.x + size.x) / texSize.x, (uncheckedPos.y + size.y) / texSize.y));
        unchecked[3] = To4(LcVector2(uncheckedPos.x / texSize.x, (uncheckedPos.y + size.y) / texSize.y));

        uncheckedH[0] = To4(LcVector2(uncheckedHPos.x / texSize.x, uncheckedHPos.y / texSize.y));
        uncheckedH[1] = To4(LcVector2((uncheckedHPos.x + size.x) / texSize.x, uncheckedHPos.y / texSize.y));
        uncheckedH[2] = To4(LcVector2((uncheckedHPos.x + size.x) / texSize.x, (uncheckedHPos.y + size.y) / texSize.y));
        uncheckedH[3] = To4(LcVector2(uncheckedHPos.x / texSize.x, (uncheckedHPos.y + size.y) / texSize.y));

        checked[0] = To4(LcVector2(checkedPos.x / texSize.x, checkedPos.y / texSize.y));
        checked[1] = To4(LcVector2((checkedPos.x + size.x) / texSize.x, checkedPos.y / texSize.y));
        checked[2] = To4(LcVector2((checkedPos.x + size.x) / texSize.x, (checkedPos.y + size.y) / texSize.y));
        checked[3] = To4(LcVector2(checkedPos.x / texSize.x, (checkedPos.y + size.y) / texSize.y));

        checkedH[0] = To4(LcVector2(checkedHPos.x / texSize.x, checkedHPos.y / texSize.y));
        checkedH[1] = To4(LcVector2((checkedHPos.x + size.x) / texSize.x, checkedHPos.y / texSize.y));
        checkedH[2] = To4(LcVector2((checkedHPos.x + size.x) / texSize.x, (checkedHPos.y + size.y) / texSize.y));
        checkedH[3] = To4(LcVector2(checkedHPos.x / texSize.x, (checkedHPos.y + size.y) / texSize.y));
    }
}

const void* LcWidgetCheckboxComponent::GetData() const
{
    switch (state)
    {
    case ECheckboxState::UncheckedHovered:   return uncheckedH;
    case ECheckboxState::CheckedHovered:     return checkedH;
    case ECheckboxState::Checked:            return checked;
    }

    return unchecked;
}

void LcWidget::AddChild(IWidget* child)
{
    if (!child) throw std::exception("LcWidget::AddChild(): Invalid child");

    childs.push_back(child);

    auto widget = static_cast<LcWidget*>(child);
    widget->parent = this;
}

void LcWidget::RemoveChild(IWidget* child)
{
    if (!child) throw std::exception("LcWidget::RemoveChild(): Invalid child");

    auto it = std::find(childs.begin(), childs.end(), child);
    if (it != childs.end())
    {
        auto widget = static_cast<LcWidget*>(*it);
        widget->parent = nullptr;
        childs.erase(it);

    }
}

void LcWidget::OnMouseButton(LcMouseBtn btn, LcKeyState state, int x, int y, const LcAppContext& context)
{
    if (auto button = GetButtonComponent())
    {
        if (state == LcKeyState::Down)
        {
            button->SetState(EBtnState::Pressed);
        }
        else
        {
            if (button->GetState() == EBtnState::Pressed)
            {
                if (auto clickComp = GetClickHandlerComponent())
                {
                    if (auto& handler = clickComp->GetHandler()) handler();
                }
            }

            button->SetState(IsHovered() ? EBtnState::Over : EBtnState::Idle);
        }
    }

    if (auto checkbox = GetCheckboxComponent())
    {
        if (state == LcKeyState::Down)
        {
            checkbox->SetState(checkbox->IsChecked() ? ECheckboxState::UncheckedHovered : ECheckboxState::CheckedHovered);

            if (auto checkComp = GetCheckHandlerComponent())
            {
                if (auto handler = checkComp->GetHandler()) handler(checkbox->IsChecked());
            }
        }
    }
}

void LcWidget::OnMouseEnter(const LcAppContext& context)
{
    if (auto button = GetButtonComponent())
    {
        button->SetState(EBtnState::Over);
    }

    if (auto checkbox = GetCheckboxComponent())
    {
        checkbox->SetState(checkbox->IsChecked() ? ECheckboxState::CheckedHovered : ECheckboxState::UncheckedHovered);
    }
}

void LcWidget::OnMouseLeave(const LcAppContext& context)
{
    if (auto button = GetButtonComponent())
    {
        button->SetState(EBtnState::Idle);
    }

    if (auto checkbox = GetCheckboxComponent())
    {
        checkbox->SetState(checkbox->IsChecked() ? ECheckboxState::Checked : ECheckboxState::Unchecked);
    }
}
