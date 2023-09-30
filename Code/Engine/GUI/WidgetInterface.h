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
* Widget text component */
class IWidgetTextComponent : public IVisualComponent
{
public:
    //
    virtual void SetText(const std::wstring& text) = 0;
    //
    virtual const std::wstring& GetText() const = 0;
    //
    virtual const std::wstring& GetFontName() const = 0;
    //
    virtual unsigned short GetFontSize() const = 0;
    //
    virtual LcFontWeight GetFontWeight() const = 0;
    //
    virtual LcTextAlignment GetTextAlignment() const = 0;
    //
    virtual LcColor4 GetTextColor() const = 0;
};


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
    void AddTextComponent(const std::wstring& inText, LcColor4 inTextColor = LcDefaults::Black4,
        const std::wstring& inFontName = L"Calibri", unsigned short inFontSize = 22, LcFontWeight inFontWeight = LcFontWeight::Normal,
        LcTextAlignment inTextAlign = LcTextAlignment::Center, const LcAppContext* context = nullptr);
    /**
    * Add text component to the last added widget */
    void AddAlignedTextComponent(const std::wstring& inText, LcColor4 inTextColor = LcDefaults::Black4,
        LcTextAlignment inTextAlign = LcTextAlignment::Center, const std::wstring& inFontName = L"Calibri",
        unsigned short inFontSize = 22, LcFontWeight inFontWeight = LcFontWeight::Normal, const LcAppContext* context = nullptr);
    /**
    * Add button component to the last added widget */
    void AddButtonComponent(const std::string& texture, LcVector2 idlePos, LcVector2 overPos, LcVector2 pressedPos, const LcAppContext& context);
    /**
    * Add checkbox component to the last added widget */
    void AddCheckboxComponent(const std::string& texture, LcVector2 uncheckedPos, LcVector2 uncheckedHoveredPos,
        LcVector2 checkedPos, LcVector2 checkedHoveredPos, const LcAppContext& context);
    /**
    * Add click handler component to the last added widget */
    void AddClickHandlerComponent(LcClickHandler handler, bool addDefaultButtonSkin = true, const LcAppContext* context = nullptr);
    /**
    * Add check handler component to the last added widget */
    void AddCheckHandlerComponent(LcCheckHandler handler, bool addDefaultCheckboxSkin = true, const LcAppContext* context = nullptr);


public:
    //
    IWidgetTextComponent* GetTextComponent() const { return (IWidgetTextComponent*)GetComponent(EVCType::Text).get(); }
    //
    IWidgetButtonComponent* GetButtonComponent() const { return (IWidgetButtonComponent*)GetComponent(EVCType::Button).get(); }
    //
    IWidgetCheckboxComponent* GetCheckboxComponent() const { return (IWidgetCheckboxComponent*)GetComponent(EVCType::Checkbox).get(); }
    //
    IWidgetClickComponent* GetClickHandlerComponent() const { return (IWidgetClickComponent*)GetComponent(EVCType::ClickHandler).get(); }
    //
    IWidgetCheckComponent* GetCheckHandlerComponent() const { return (IWidgetCheckComponent*)GetComponent(EVCType::CheckHandler).get(); }

};


/** Widget helper */
class GUI_API LcWidgetHelper : public LcVisualHelper
{
public:
    LcWidgetHelper(const LcAppContext& inContext) : LcVisualHelper(inContext) {}


public:
    /**
    * Add text component to the last added widget */
    void AddTextComponent(const std::wstring& inText, LcColor4 inTextColor = LcDefaults::Black4,
        const std::wstring& inFontName = L"Calibri", unsigned short inFontSize = 22,
        LcFontWeight inFontWeight = LcFontWeight::Normal, LcTextAlignment inTextAlign = LcTextAlignment::Center) const;
    /**
    * Add text component to the last added widget */
    void AddAlignedTextComponent(const std::wstring& inText, LcColor4 inTextColor = LcDefaults::Black4,
        LcTextAlignment inTextAlign = LcTextAlignment::Center, const std::wstring& inFontName = L"Calibri",
        unsigned short inFontSize = 22, LcFontWeight inFontWeight = LcFontWeight::Normal) const;
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
