/**
* NoesisWidget.h
* 04.02.2023
* (c) Denis Romakhov
*/

#pragma once

#include "Module.h"
#include "Core/LCTypes.h"

#include <NsGui/XamlProvider.h>
#include <NsGui/UserControl.h>
#include <NsGui/IView.h>


namespace NoesisApp { class RenderContext; }

class LcNoesisWidget : public IWidget
{
public:
    LcNoesisWidget(LcWidgetData inWidget, class NoesisApp::RenderContext* inContext);
    //
    ~LcNoesisWidget();
    //
    LcWidgetData widget;
    //
    LcSizef size;
    //
    Noesis::Ptr<Noesis::UserControl> control;
    //
    Noesis::Ptr<Noesis::IView> view;
    //
    class NoesisApp::RenderContext* context;


public: // IWidget interface implementation
    virtual void Init(LcSize viewportSize) override;
    //
    virtual void OnKeyboard(int btn, LcKeyState state) override;
    //
    virtual const std::string& GetName() const override { return widget.name; }


public: // IVisual interface implementation
    virtual void Update(float DeltaSeconds);
    //
    virtual void PreRender();
    //
    virtual void PostRender();
    //
    virtual void SetSize(LcSizef inSize) override { if (view) view->SetSize((uint32_t)inSize.x(), (uint32_t)inSize.y()); size = inSize; }
    //
    virtual LcSizef GetSize() const override { return size; }
    //
    virtual void SetPos(LcVector3 inPos) override { widget.pos = inPos; }
    //
    virtual void AddPos(LcVector3 inPos) override { widget.pos += inPos; }
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
    virtual void SetActive(bool inActive) override { widget.active = inActive; }
    //
    virtual bool IsActive() const override { return widget.active; }
    //
    virtual void OnMouseButton(LcMouseBtn btn, LcKeyState state, int x, int y) override;
    //
    virtual void OnMouseMove(int x, int y) override;

};


class LcNoesisWidgetFactory : public TWorldFactory<IWidget, LcWidgetData>
{
public:
    LcNoesisWidgetFactory(class NoesisApp::RenderContext* inContext) : context(inContext) {}
    //
    virtual std::shared_ptr<IWidget> Build(const LcWidgetData& data) override;


protected:
    class NoesisApp::RenderContext* context;

};
