/**
* Widgets.h
* 03.02.2023
* (c) Denis Romakhov
*/

#pragma once

#include "Visual.h"
#include <string>


/**
* Widget data */
struct LcWidgetData
{
	std::string name;
	LcVector3 pos;
	bool visible;
	bool active;
	//
	LcWidgetData() : pos(LcDefaults::ZeroVec3), visible(true), active(false) {}
	//
	LcWidgetData(const std::string& inName, LcVector3 inPos, bool inVisible = true, bool inActive = false)
		: name(inName), pos(inPos), visible(inVisible), active(inActive)
	{
	}
};


/**
* Widget interface */
class IWidget : public IVisual
{
public:
	/**
	* Initialize widget */
	virtual void Init() = 0;
	/**
	* Widget name */
	virtual const std::string& GetName() const = 0;
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
	/**
	* Keyboard key event */
	virtual void OnKeyboard(int btn, LcKeyState state) = 0;

};


/**
* Widget base */
class LcWidgetBase : public IWidget
{
public:
    LcWidgetBase(LcWidgetData inWidget) : widget(inWidget), size(LcDefaults::ZeroVec2), focused(false) {}
    //
    ~LcWidgetBase() {}
    //
    LcWidgetData widget;
    //
    LcSizef size;
    //
    bool focused;


public: // IWidget interface implementation
    virtual void Init() override {}
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
    //
    virtual const std::string& GetName() const override { return widget.name; }


public: // IVisual interface implementation
    virtual void Update(float DeltaSeconds) {}
    //
    virtual void PreRender() {}
    //
    virtual void PostRender() {}
    //
    virtual void SetSize(LcSizef inSize) override { size = inSize; }
    //
    virtual LcSizef GetSize() const override { return size; }
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
