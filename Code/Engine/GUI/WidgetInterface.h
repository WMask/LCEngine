/**
* WidgetInterface.h
* 10.09.2023
* (c) Denis Romakhov
*/

#pragma once

#include "Module.h"
#include "Core/Visual.h"

#include <functional>
#include <string>


namespace LcComponents
{
    constexpr int Text = 50;
    constexpr int Button = 51;
    constexpr int Checkbox = 52;
    constexpr int ClickHandler = 53;
    constexpr int CheckHandler = 54;
}


/** Text block settings */
struct LcTextBlockSettings
{
    LcTextBlockSettings()
        : textColor(LcDefaults::Black4)
        , textAlign(LcTextAlignment::Center)
        , fontWeight(LcFontWeight::Normal)
        , fontSize(14)
    {}
    //
    LcColor4 textColor;
    //
    LcTextAlignment textAlign;
    //
    std::wstring fontName;
    //
    LcFontWeight fontWeight;
    //
    unsigned short fontSize;
};


/**
* Widget text component */
class IWidgetTextComponent : public IVisualComponent
{
public:
    //
    virtual void SetTextKey(const std::string& textKey) = 0;
    //
    virtual const std::string& GetTextKey() const = 0;
    //
    virtual const LcTextBlockSettings& GetSettings() const = 0;
};


/** Button state */
enum class EBtnState : int
{
    Idle,
    Over,
    Pressed
};

/** Widget click handler */
typedef std::function<void()> LcClickHandler;

/**
* Widget button component */
class IWidgetButtonComponent : public IVisualComponent
{
public:
    //
    virtual void SetState(EBtnState state) = 0;
    //
    virtual EBtnState GetState() const = 0;
    // returns vertex data LcVector4[4]
    virtual const void* GetData() const = 0;
};


/** Checkbox state */
enum class ECheckboxState : int
{
    Unchecked,
    UncheckedHovered,
    Checked,
    CheckedHovered
};

/** Widget check handler */
typedef std::function<void(bool)> LcCheckHandler;

/**
* Widget checkbox component */
class IWidgetCheckboxComponent : public IVisualComponent
{
public:
    //
    virtual void SetState(ECheckboxState state) = 0;
    //
    virtual bool IsChecked() const = 0;
    // returns vertex data LcVector4[4]
    virtual const void* GetData() const = 0;
};


/**
* Widget click component */
class IWidgetClickComponent : public IVisualComponent
{
public:
    //
    virtual LcClickHandler& GetHandler() = 0;
};


/**
* Widget check component */
class IWidgetCheckComponent : public IVisualComponent
{
public:
    //
    virtual LcCheckHandler& GetHandler() = 0;
};


/**
* Widget interface */
class GUI_API IWidget : public IVisualBase
{
public: // IVisual interface implementation
    //
    virtual int GetTypeId() const override { return LcCreatables::Widget; }


public:
    typedef std::deque<class IWidget*> TChildsList;
    /**
    * Add child widget */
    virtual void AddChild(IWidget* child) = 0;
    /**
    * Remove child widget */
    virtual void RemoveChild(IWidget* child) = 0;
    /**
    * Get parent widget */
    virtual IWidget* GetParent() const = 0;
    /**
    * Get child widgets */
    virtual const TChildsList& GetChilds() const = 0;
    /**
    * Get child widgets */
    virtual TChildsList& GetChilds() = 0;
    //
    static int GetStaticId() { return LcCreatables::Widget; }
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


public:
    /**
    * Add text component to the last added widget */
    void AddTextComponent(const LcAppContext& context, const std::string& inTextKey, const LcTextBlockSettings& inSettings);
    /**
    * Add button component to the last added widget */
    void AddButtonComponent(const LcAppContext& context, const std::string& texture, LcVector2 idlePos, LcVector2 overPos, LcVector2 pressedPos);
    /**
    * Add checkbox component to the last added widget */
    void AddCheckboxComponent(const LcAppContext& context, const std::string& texture, LcVector2 uncheckedPos, LcVector2 uncheckedHoveredPos,
        LcVector2 checkedPos, LcVector2 checkedHoveredPos);
    /**
    * Add click handler component to the last added widget */
    void AddClickHandlerComponent(const LcAppContext& context, LcClickHandler handler, bool addDefaultButtonSkin = true);
    /**
    * Add check handler component to the last added widget */
    void AddCheckHandlerComponent(const LcAppContext& context, LcCheckHandler handler, bool addDefaultCheckboxSkin = true);


public:
    //
    IWidgetTextComponent* GetTextComponent() const { return (IWidgetTextComponent*)GetComponent(LcComponents::Text).get(); }
    //
    IWidgetButtonComponent* GetButtonComponent() const { return (IWidgetButtonComponent*)GetComponent(LcComponents::Button).get(); }
    //
    IWidgetCheckboxComponent* GetCheckboxComponent() const { return (IWidgetCheckboxComponent*)GetComponent(LcComponents::Checkbox).get(); }
    //
    IWidgetClickComponent* GetClickHandlerComponent() const { return (IWidgetClickComponent*)GetComponent(LcComponents::ClickHandler).get(); }
    //
    IWidgetCheckComponent* GetCheckHandlerComponent() const { return (IWidgetCheckComponent*)GetComponent(LcComponents::CheckHandler).get(); }

};


/** Widget helper */
class GUI_API LcWidgetHelper : public LcVisualHelper
{
public:
    LcWidgetHelper(const LcAppContext& inContext) : LcVisualHelper(inContext) {}


public:
    /**
    * Add text component to the last added widget */
    void AddTextComponent(const std::string& inTextKey, const LcTextBlockSettings& inSettings) const;
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

};
