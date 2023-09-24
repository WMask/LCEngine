/**
* Widgets.h
* 31.08.2023
* (c) Denis Romakhov
*/

#pragma once

#include "Module.h"
#include "GUI/WidgetInterface.h"

#include <string>


/**
* Widget text component */
struct LcWidgetTextComponent : public IVisualComponent
{
    std::wstring text;
    std::wstring fontName;
    unsigned short fontSize;
    LcFontWeight fontWeight;
    LcTextAlignment textAlign;
    LcColor4 textColor;
    //
    LcWidgetTextComponent() : fontSize(0), fontWeight(LcFontWeight::Normal), textAlign(LcTextAlignment::Center), textColor(LcDefaults::White4) {}
    //
    LcWidgetTextComponent(const LcWidgetTextComponent& textComp) :
        text(textComp.text), textColor(textComp.textColor), textAlign(textComp.textAlign),
        fontName(textComp.fontName), fontSize(textComp.fontSize), fontWeight(textComp.fontWeight) {}
    //
    LcWidgetTextComponent(const std::wstring& inText, LcColor4 inTextColor, LcTextAlignment inTextAlign,
        const std::wstring& inFontName, unsigned short inFontSize, LcFontWeight inFontWeight = LcFontWeight::Normal) :
        text(inText), textColor(inTextColor), textAlign(inTextAlign), fontName(inFontName), fontSize(inFontSize), fontWeight(inFontWeight)
    {
    }
    // IVisualComponent interface implementation
    virtual EVCType GetType() const override { return EVCType::Text; }

};


/**
* Widget button component */
struct GUI_API LcWidgetButtonComponent : public IVisualComponent
{
    EBtnState state;
    LcVector4 idle[4];      // custom UVs for Idle state
    LcVector4 over[4];      // custom UVs for Mouse Over state
    LcVector4 pressed[4];   // custom UVs for Pressed state
    //
    LcWidgetButtonComponent() : state(EBtnState::Idle) {}
    //
    LcWidgetButtonComponent(const LcWidgetButtonComponent& button);
    //
    LcWidgetButtonComponent(LcVector2 idlePos, LcVector2 overPos, LcVector2 pressedPos);
    //
   const void* GetData() const;


public:// IVisualComponent interface implementation
    //
    virtual void Init(const LcAppContext& context) override;
    //
    virtual EVCType GetType() const override { return EVCType::Button; }

};


/**
* Widget checkbox component */
struct GUI_API LcWidgetCheckboxComponent : public IVisualComponent
{
    ECheckboxState state;
    LcVector4 unchecked[4];     // custom UVs for Unchecked state
    LcVector4 uncheckedH[4];    // custom UVs for UncheckedHovered state
    LcVector4 checked[4];       // custom UVs for Checked state
    LcVector4 checkedH[4];      // custom UVs for CheckedHovered state
    //
    LcWidgetCheckboxComponent() : state(ECheckboxState::Unchecked) {}
    //
    LcWidgetCheckboxComponent(const LcWidgetCheckboxComponent& checkbox);
    //
    LcWidgetCheckboxComponent(LcVector2 uncheckedPos, LcVector2 uncheckedHoveredPos, LcVector2 checkedPos, LcVector2 checkedHoveredPos);
    //
    inline bool IsChecked() const { return state == ECheckboxState::Checked || state == ECheckboxState::CheckedHovered; }
    //
    const void* GetData() const;


public:// IVisualComponent interface implementation
    //
    virtual void Init(const LcAppContext& context) override;
    //
    virtual EVCType GetType() const override { return EVCType::Checkbox; }

};


/**
* Widget click component */
struct LcWidgetClickComponent : public IVisualComponent
{
    LcClickHandler handler;
    //
    LcWidgetClickComponent() {}
    //
    LcWidgetClickComponent(const LcWidgetClickComponent& widget) : handler(widget.handler) {}
    //
    LcWidgetClickComponent(LcClickHandler inHandler) : handler(inHandler) {}
    // IVisualComponent interface implementation
    virtual EVCType GetType() const override { return EVCType::ClickHandler; }

};


/**
* Widget check component */
struct LcWidgetCheckComponent : public IVisualComponent
{
    LcCheckHandler handler;
    //
    LcWidgetCheckComponent() {}
    //
    LcWidgetCheckComponent(const LcWidgetCheckComponent& widget) : handler(widget.handler) {}
    //
    LcWidgetCheckComponent(LcCheckHandler inHandler) : handler(inHandler) {}
    // IVisualComponent interface implementation
    virtual EVCType GetType() const override { return EVCType::CheckHandler; }

};


/**
* Widget class */
class GUI_API LcWidget : public IWidget
{
public:
    LcWidget()
        : pos(LcDefaults::ZeroVec3)
        , size(LcDefaults::ZeroSize)
        , visible(true)
        , disabled(false)
        , focused(false)
        , hovered(false)
    {
    }
    //
    ~LcWidget() {}


public:// IWidget interface implementation
    //
    virtual void OnKeys(int btn, LcKeyState state, const LcAppContext& context) override {}
    //
    virtual void RecreateFont(const LcAppContext& context) override {}
    //
    virtual void SetDisabled(bool inDisabled) override { disabled = inDisabled; }
    //
    virtual bool IsDisabled() const override { return disabled; }
    //
    virtual void SetHovered(bool inHovered) override { hovered = inHovered; }
    //
    virtual bool IsHovered() const override { return hovered; }
    //
    virtual void SetFocus(bool inFocus) override { focused = inFocus; }
    //
    virtual bool HasFocus() const override { return focused; }


public:// IVisual interface implementation
    //
    virtual void SetSize(LcSizef inSize) override { size = inSize; }
    //
    virtual LcSizef GetSize() const override { return size; }
    //
    virtual void SetPos(LcVector3 inPos) override { pos = inPos; }
    //
    virtual void AddPos(LcVector3 inPos) override { pos = pos + inPos; }
    //
    virtual LcVector3 GetPos() const override { return pos; }
    //
    virtual void SetRotZ(float inRotZ) override {}
    //
    virtual void AddRotZ(float inRotZ) override {}
    //
    virtual float GetRotZ() const override { return 0.0f; }
    //
    virtual void SetVisible(bool inVisible) override { visible = inVisible; }
    //
    virtual bool IsVisible() const override { return visible; }
    //
    virtual void OnMouseButton(LcMouseBtn btn, LcKeyState state, int x, int y, const LcAppContext& context) override;
    //
    virtual void OnMouseMove(LcVector3 pos, const LcAppContext& context) override {}
    //
    virtual void OnMouseEnter(const LcAppContext& context) override;
    //
    virtual void OnMouseLeave(const LcAppContext& context) override;


protected:
    // [0,0] - leftTop, x - right, y - down
    LcVector3 pos;
    // widget size in pixels
    LcSizef size;
    //
    bool visible;
    //
    bool disabled;
    //
    bool hovered;
    //
    bool focused;

};
