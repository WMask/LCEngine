/**
* WidgetInterface.h
* 10.09.2023
* (c) Denis Romakhov
*/

#pragma once

#include "Module.h"
#include "World/Visual.h"

#include <functional>
#include <string>


struct LcWidgetTextComponent;
struct LcWidgetButtonComponent;
struct LcWidgetCheckboxComponent;
struct LcWidgetClickComponent;
struct LcWidgetCheckComponent;


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


/**
* Widget interface */
class GUI_API IWidget : public IVisualBase
{
public:
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
    virtual void OnKeys(int btn, LcKeyState state, const LcAppContext& context) = 0;
    /**
    * Recreate font after World scale change */
    virtual void RecreateFont(const LcAppContext& context) = 0;
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


/** Widget helper */
class GUI_API LcWidgetHelper
{
public:
    LcWidgetHelper(const LcAppContext& inContext) : context(inContext) {}


public:
    /**
    * Add texture component to the last added sprite or widget */
    void AddTextureComponent(const std::string& inTexture) const;
    /**
    * Add texture component to the last added sprite or widget */
    void AddTextureComponent(const LcBytes& inData) const;
    /**
    * Add text component to the last added widget */
    void AddTextComponent(const std::wstring& inText, LcColor4 inTextColor = LcDefaults::Black4,
        const std::wstring& inFontName = L"Calibri", unsigned short inFontSize = 22,
        LcFontWeight inFontWeight = LcFontWeight::Normal) const;
    /**
    * Add button component to the last added widget */
    void AddButtonComponent(const std::string& texture, LcVector2 idlePos, LcVector2 overPos, LcVector2 pressedPos) const;
    /**
    * Add checkbox component to the last added widget */
    void AddCheckboxComponent(const std::string& texture, LcVector2 uncheckedPos,
        LcVector2 uncheckedHoveredPos, LcVector2 checkedPos, LcVector2 checkedHoveredPos) const;
    /**
    * Add click handler component to the last added widget */
    void AddClickHandlerComponent(LcClickHandler handler, bool addDefaultButtonSkin = true) const;
    /**
    * Add check handler component to the last added widget */
    void AddCheckHandlerComponent(LcCheckHandler handler, bool addDefaultCheckboxSkin = true) const;


protected:
    const LcAppContext& context;

};
