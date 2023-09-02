/**
* Widgets.h
* 31.08.2023
* (c) Denis Romakhov
*/

#pragma once

#include "Module.h"
#include "World/Visual.h"

#include <string>
#include <functional>


/** Widget data */
struct LcWidgetData
{
    LcVector3 pos;	// [0,0] - leftBottom, x - right, y - up
    LcSizef size;	// widget size in pixels
	bool visible;
	bool active;
	//
	LcWidgetData() : pos(LcDefaults::ZeroVec3), size(LcDefaults::ZeroSize), visible(true), active(false) {}
	//
	LcWidgetData(LcVector3 inPos, LcSizef inSize, bool inVisible = true, bool inActive = false)
		: pos(inPos), size(inSize), visible(inVisible), active(inActive)
	{
	}
    //
    LcWidgetData(LcVector2 inPos, LcSizef inSize, bool inVisible = true, bool inActive = false)
        : pos(To3(inPos)), size(inSize), visible(inVisible), active(inActive)
    {
    }
};


/** Widget texture component */
typedef struct LcSpriteTextureComponent LcWidgetTextureComponent;

/**
* Widget text component */
struct LcWidgetTextComponent : public IVisualComponent
{
    std::wstring text;
    std::wstring fontName;
    unsigned short fontSize;
    LcFontWeight fontWeight;
    LcColor4 textColor;
    //
    LcWidgetTextComponent() : fontSize(0), fontWeight(LcFontWeight::Normal), textColor(LcDefaults::White4) {}
    //
    LcWidgetTextComponent(const LcWidgetTextComponent& texture) :
        text(texture.text), fontName(texture.fontName), fontSize(texture.fontSize), fontWeight(texture.fontWeight), textColor(texture.textColor) {}
    //
    LcWidgetTextComponent(const std::wstring& inText, const std::wstring& inFontName, unsigned short inFontSize,
        LcColor4 inTextColor, LcFontWeight inFontWeight = LcFontWeight::Normal) :
        text(inText), fontName(inFontName), fontSize(inFontSize), fontWeight(inFontWeight), textColor(inTextColor)
    {
    }
    // IVisualComponent interface implementation
    virtual EVCType GetType() const override { return EVCType::Text; }

};


/** Visual component type */
enum class EBtnState : int
{
    Idle,
    Over,
    Pressed
};

/** Button click handler */
typedef std::function<void()> FBtnHandler;

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
    virtual void Init(class IWorld& world) override;
    //
    virtual EVCType GetType() const override { return EVCType::Button; }

};


/** Widget click handler */
typedef std::function<void()> LcClickHandler;

/**
* Widget click component */
struct GUI_API LcWidgetClickComponent : public IVisualComponent
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
* Widget interface */
class IWidget : public IVisualBase
{
public:
    void AddTextComponent(const std::wstring& inText, const std::wstring& inFontName, unsigned short inFontSize,
        LcColor4 inTextColor, LcFontWeight inFontWeight = LcFontWeight::Normal)
    {
        AddComponent(std::make_shared<LcWidgetTextComponent>(inText, inFontName, inFontSize, inTextColor, inFontWeight));
    }
    //
    void AddTextureComponent(const std::string& texture)
    {
        AddComponent(std::make_shared<LcVisualTextureComponent>(texture));
    }
    //
    void AddTextureComponent(const LcBytes& inData)
    {
        AddComponent(std::make_shared<LcVisualTextureComponent>(inData));
    }
    void AddButtonComponent()
    {
        AddComponent(std::make_shared<LcWidgetButtonComponent>());
    }
    void AddButtonComponent(const std::string& texture, LcVector2 idlePos, LcVector2 overPos, LcVector2 pressedPos)
    {
        AddComponent(std::make_shared<LcVisualTextureComponent>(texture));
        AddComponent(std::make_shared<LcWidgetButtonComponent>(idlePos, overPos, pressedPos));
    }
    void AddClickHandlerComponent(LcClickHandler handler)
    {
        AddComponent(std::make_shared<LcWidgetClickComponent>(handler));
    }
    //
    LcWidgetTextComponent* GetTextComponent() const { return (LcWidgetTextComponent*)GetComponent(EVCType::Text).get(); }
    //
    LcVisualTextureComponent* GetTextureComponent() const { return (LcVisualTextureComponent*)GetComponent(EVCType::Texture).get(); }
    //
    LcWidgetButtonComponent* GetButtonComponent() const { return (LcWidgetButtonComponent*)GetComponent(EVCType::Button).get(); }
    //
    LcWidgetClickComponent* GetClickHandlerComponent() const { return (LcWidgetClickComponent*)GetComponent(EVCType::ClickHandler).get(); }


public:
    /**
    * Keyboard key event */
    virtual void OnKeyboard(int btn, LcKeyState state) = 0;
	/**
	* Set active state */
	virtual void SetActive(bool active) = 0;
	/**
	* Active state */
	virtual bool IsActive() const = 0;
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


/**
* Widget class */
class GUI_API LcWidget : public IWidget
{
public:
    LcWidget(LcWidgetData inWidget) : widget(inWidget), focused(false), hovered(false) {}
    //
    ~LcWidget() {}
    //
    LcWidgetData widget;
    //
    bool hovered;
    //
    bool focused;


public:// IWidget interface implementation
    //
    virtual void OnKeyboard(int btn, LcKeyState state) override {}
    //
    virtual void SetActive(bool inActive) override { widget.active = inActive; }
    //
    virtual bool IsActive() const override { return widget.active; }
    //
    virtual void SetHovered(bool inHovered) override { hovered = inHovered; }
    //
    virtual bool IsHovered() const override { return hovered; }
    //
    virtual void SetFocus(bool inFocus) override { focused = inFocus; }
    //
    virtual bool HasFocus() const override { return focused; }


public:// IVisual interface implementation
    virtual void Update(float DeltaSeconds) {}
    //
    virtual void SetSize(LcSizef inSize) override { widget.size = inSize; }
    //
    virtual LcSizef GetSize() const override { return widget.size; }
    //
    virtual void SetPos(LcVector3 inPos) override {}
    //
    virtual void AddPos(LcVector3 inPos) override {}
    //
    virtual LcVector3 GetPos() const override { return widget.pos; }
    //
    virtual void SetRotZ(float inRotZ) override {}
    //
    virtual void AddRotZ(float inRotZ) override {}
    //
    virtual float GetRotZ() const override { return 0.0f; }
    //
    virtual void SetVisible(bool inVisible) override { widget.visible = inVisible; }
    //
    virtual bool IsVisible() const override { return widget.visible; }
    //
    virtual void OnMouseButton(LcMouseBtn btn, LcKeyState state, int x, int y) override;
    //
    virtual void OnMouseMove(LcVector3 pos) override {}
    //
    virtual void OnMouseEnter() override;
    //
    virtual void OnMouseLeave() override;

};
