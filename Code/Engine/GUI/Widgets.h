/**
* Widgets.h
* 31.08.2023
* (c) Denis Romakhov
*/

#pragma once

#include "World/Visual.h"
#include <string>


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

/** Text font */
struct ITextFont
{
public:
    virtual ~ITextFont() {}
    //
    virtual const std::wstring& GetFontName() const = 0;
};

/** Font weight */
enum class LcFontWeight { Light, Normal, Bold };


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
    LcWidgetTextComponent* GetTextComponent() const { return (LcWidgetTextComponent*)GetComponent(EVCType::Text).get(); }


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
	* Set focused state */
	virtual void SetFocus(bool focus) = 0;
	/**
	* Focused state */
	virtual bool HasFocus() const = 0;

};


/**
* Widget class */
class LcWidget : public IWidget
{
public:
    LcWidget(LcWidgetData inWidget) : widget(inWidget), focused(false) {}
    //
    ~LcWidget() {}
    //
    LcWidgetData widget;
    //
    bool focused;


public: // IWidget interface implementation
    //
    virtual void OnKeyboard(int btn, LcKeyState state) override {}
    //
    virtual void SetActive(bool inActive) override { widget.active = inActive; }
    //
    virtual bool IsActive() const override { return widget.active; }
    //
    virtual void SetFocus(bool inFocus) override { focused = inFocus; }
    //
    virtual bool HasFocus() const override { return focused; }


public: // IVisual interface implementation
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
    virtual void OnMouseButton(LcMouseBtn btn, LcKeyState state, int x, int y) override {}
    //
    virtual void OnMouseMove(int x, int y) override {}

};
