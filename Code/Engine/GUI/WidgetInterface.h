/**
* WidgetInterface.h
* 10.09.2023
* (c) Denis Romakhov
*/

#pragma once

#include "Module.h"
#include "World/Visual.h"

#include <string>
#include <functional>


/** Button state */
enum class EBtnState : int
{
    Idle,
    Over,
    Pressed
};

/** Checkbox state */
enum class ECheckboxState : int
{
    Unchecked,
    UncheckedHovered,
    Checked,
    CheckedHovered
};

/** Widget click handler */
typedef std::function<void()> LcClickHandler;

/** Widget check handler */
typedef std::function<void(bool)> LcCheckHandler;

typedef struct LcSpriteTextureComponent LcWidgetTextureComponent;

struct LcWidgetTextComponent;
struct LcWidgetButtonComponent;
struct LcWidgetCheckboxComponent;
struct LcWidgetClickComponent;
struct LcWidgetCheckComponent;


/**
* Widget interface */
class GUI_API IWidget : public IVisualBase
{
public:
    void AddTextComponent(const std::wstring& inText, LcColor4 inTextColor = LcDefaults::Black4,
        const std::wstring& inFontName = L"Calibri", unsigned short inFontSize = 22,
        LcFontWeight inFontWeight = LcFontWeight::Normal);
    //
    void AddTextureComponent(const std::string& texture);
    //
    void AddTextureComponent(const LcBytes& inData);
    //
    void AddButtonComponent();
    //
    void AddButtonComponent(const std::string& texture, LcVector2 idlePos, LcVector2 overPos, LcVector2 pressedPos);
    //
    void AddCheckboxComponent(const std::string& texture, LcVector2 uncheckedPos,
        LcVector2 uncheckedHoveredPos, LcVector2 checkedPos, LcVector2 checkedHoveredPos);
    //
    void AddClickHandlerComponent(LcClickHandler handler, bool addDefaultSkin = true);
    //
    void AddCheckHandlerComponent(LcCheckHandler handler, bool addDefaultSkin = true);
    //
    LcWidgetTextComponent* GetTextComponent() const { return (LcWidgetTextComponent*)GetComponent(EVCType::Text).get(); }
    //
    LcVisualTextureComponent* GetTextureComponent() const { return (LcVisualTextureComponent*)GetComponent(EVCType::Texture).get(); }
    //
    LcWidgetButtonComponent* GetButtonComponent() const { return (LcWidgetButtonComponent*)GetComponent(EVCType::Button).get(); }
    //
    LcWidgetCheckboxComponent* GetCheckboxComponent() const { return (LcWidgetCheckboxComponent*)GetComponent(EVCType::Checkbox).get(); }
    //
    LcWidgetClickComponent* GetClickHandlerComponent() const { return (LcWidgetClickComponent*)GetComponent(EVCType::ClickHandler).get(); }
    //
    LcWidgetCheckComponent* GetCheckHandlerComponent() const { return (LcWidgetCheckComponent*)GetComponent(EVCType::CheckHandler).get(); }


public:
    /**
    * Keyboard key event */
    virtual void OnKeyboard(int btn, LcKeyState state) = 0;
    /**
    * Recreate font after World scale change */
    virtual void RecreateFont() = 0;
    /**
    * Set disabled state */
    virtual void SetDisabled(bool disabled) = 0;
    /**
    * Disabled state */
    virtual bool IsDisabled() const = 0;
    /**
    * Set mouse over */
    virtual void SetHovered(bool active) = 0;
    /**
    * Is mouse over */
    virtual bool IsHovered() const = 0;
    /**
    * Set focused state */
    virtual void SetFocus(bool focus) = 0;
    /**
    * Focused state */
    virtual bool HasFocus() const = 0;

};
