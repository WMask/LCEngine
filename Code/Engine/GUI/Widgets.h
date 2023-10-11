/**
* Widgets.h
* 31.08.2023
* (c) Denis Romakhov
*/

#pragma once

#include "Module.h"
#include "GUI/WidgetInterface.h"

#include <string>


class LcWidgetTextComponent : public IWidgetTextComponent
{
public:
    LcWidgetTextComponent() {}
    //
    LcWidgetTextComponent(const LcWidgetTextComponent& textComp) : text(textComp.text), settings(textComp.settings) {}
    //
    LcWidgetTextComponent(const std::wstring& inText, const LcTextBlockSettings& inSettings) : text(inText), settings(inSettings) {}


public: // IWidgetTextComponent interface implementation
    //
    virtual void SetText(const std::wstring& inText) override { text = inText; }
    //
    virtual const std::wstring& GetText() const override { return text; }
    //
    virtual const LcTextBlockSettings& GetSettings() const { return settings; }


public: // IVisualComponent interface implementation
    //
    virtual EVCType GetType() const override { return LcComponents::Text; }


protected:
    std::wstring text;
    //
    LcTextBlockSettings settings;
};


class GUI_API LcWidgetButtonComponent : public IWidgetButtonComponent
{
public:
    //
    LcWidgetButtonComponent() : state(EBtnState::Idle) {}
    //
    LcWidgetButtonComponent(const LcWidgetButtonComponent& button);
    //
    LcWidgetButtonComponent(LcVector2 idlePos, LcVector2 overPos, LcVector2 pressedPos);


public: // IWidgetButtonComponent interface implementation
    //
    virtual void SetState(EBtnState inState) override { state = inState; }
    //
    virtual EBtnState GetState() const override { return state; }
    //
    virtual const void* GetData() const override;


public: // IVisualComponent interface implementation
    //
    virtual void Init(const LcAppContext& context) override;
    //
    virtual EVCType GetType() const override { return LcComponents::Button; }


protected:
    EBtnState state;
    LcVector4 idle[4];      // custom UVs for Idle state
    LcVector4 over[4];      // custom UVs for Mouse Over state
    LcVector4 pressed[4];   // custom UVs for Pressed state

};


class GUI_API LcWidgetCheckboxComponent : public IWidgetCheckboxComponent
{
public:
    //
    LcWidgetCheckboxComponent() : state(ECheckboxState::Unchecked) {}
    //
    LcWidgetCheckboxComponent(const LcWidgetCheckboxComponent& checkbox);
    //
    LcWidgetCheckboxComponent(LcVector2 uncheckedPos, LcVector2 uncheckedHoveredPos, LcVector2 checkedPos, LcVector2 checkedHoveredPos);


public: // IWidgetCheckboxComponent interface implementation
    //
    virtual void SetState(ECheckboxState inState) override { state = inState; }
    //
    virtual bool IsChecked() const override { return state == ECheckboxState::Checked || state == ECheckboxState::CheckedHovered; }
    //
    virtual const void* GetData() const override;


public: // IVisualComponent interface implementation
    //
    virtual void Init(const LcAppContext& context) override;
    //
    virtual EVCType GetType() const override { return LcComponents::Checkbox; }


protected:
    ECheckboxState state;
    LcVector4 unchecked[4];     // custom UVs for Unchecked state
    LcVector4 uncheckedH[4];    // custom UVs for UncheckedHovered state
    LcVector4 checked[4];       // custom UVs for Checked state
    LcVector4 checkedH[4];      // custom UVs for CheckedHovered state

};


class LcWidgetClickComponent : public IWidgetClickComponent
{
public:
    //
    LcWidgetClickComponent() {}
    //
    LcWidgetClickComponent(const LcWidgetClickComponent& widget) : handler(widget.handler) {}
    //
    LcWidgetClickComponent(LcClickHandler inHandler) : handler(inHandler) {}


public: // IWidgetClickComponent interface implementation
    //
    virtual LcClickHandler& GetHandler() override { return handler; }


public: // IVisualComponent interface implementation
    //
    virtual EVCType GetType() const override { return LcComponents::ClickHandler; }


protected:
    LcClickHandler handler;

};


class LcWidgetCheckComponent : public IWidgetCheckComponent
{
public:
    //
    LcWidgetCheckComponent() {}
    //
    LcWidgetCheckComponent(const LcWidgetCheckComponent& widget) : handler(widget.handler) {}
    //
    LcWidgetCheckComponent(LcCheckHandler inHandler) : handler(inHandler) {}


public: // IWidgetCheckComponent interface implementation
    //
    virtual LcCheckHandler& GetHandler() override { return handler; }


public: // IVisualComponent interface implementation
    //
    virtual EVCType GetType() const override { return LcComponents::CheckHandler; }


protected:
    LcCheckHandler handler;

};


/**
* Widget class */
class GUI_API LcWidget : public IWidget
{
public:
    LcWidget()
        : parent(nullptr)
        , features{ LcComponents::Texture }
        , pos(LcDefaults::ZeroVec3)
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
    virtual void AddChild(IWidget* child) override;
    //
    virtual void RemoveChild(IWidget* child) override;
    //
    virtual IWidget* GetParent() const override { return parent; }
    //
    virtual const TChildsList& GetChilds() const override { return childs; }
    //
    virtual TChildsList& GetChilds() override { return childs; }
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
    virtual const TVFeaturesList& GetFeaturesList() const override { return features; }
    //
    virtual void SetSize(LcSizef inSize) override { size = inSize; }
    //
    virtual LcSizef GetSize() const override { return size; }
    //
    virtual void SetPos(LcVector3 inPos) override { pos = inPos; }
    //
    virtual void SetPos(LcVector2 inPos) override { pos = LcVector3{ inPos.x, inPos.y, pos.z }; }
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
    virtual void OnMouseButton(int btn, LcKeyState state, int x, int y, const LcAppContext& context) override;
    //
    virtual void OnMouseMove(LcVector3 pos, const LcAppContext& context) override {}
    //
    virtual void OnMouseEnter(const LcAppContext& context) override;
    //
    virtual void OnMouseLeave(const LcAppContext& context) override;


protected:
    //
    IWidget* parent;
    //
    TChildsList childs;
    //
    TVFeaturesList features;
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
