/**
* NoesisGuiManager.cpp
* 02.02.2023
* (c) Denis Romakhov
*/

#include "pch.h"
#include "GUI/LCNoesisGUI/NoesisGUIManager.h"
#include "Core/LCUtils.h"

#include <NsCore/RegisterComponent.h>
#include <NsApp/BehaviorCollection.h>
#include <NsApp/Interaction.h>
#include <NsGui/IntegrationAPI.h>
#include <NsGui/UIElementCollection.h>
#include <NsGui/UserControl.h>
#include <NsGui/IRenderer.h>
#include <NsGui/IView.h>


class LcNoesisWidget : public IWidget
{
public:
    LcNoesisWidget(LcWidgetData inWidget) : widget(inWidget) {}
    //
    ~LcNoesisWidget() { if (view) view.Reset(); }
    //
    LcWidgetData widget;
    //
    LcSizef size;
    //
    Noesis::Ptr<Noesis::IView> view;


public: // IWidget interface implementation
    virtual const std::string& GetName() const override { return widget.name; }


public: // IVisual interface implementation
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

};


class LcNoesisWidgetFactory : public TWorldFactory<IWidget, LcWidgetData>
{
public:
    LcNoesisWidgetFactory() {}
    //
    virtual std::shared_ptr<IWidget> Build(const LcWidgetData& data) override
    {
        LcNoesisWidget* newWidget = new LcNoesisWidget(data);
        std::shared_ptr<IWidget> newWidgetPtr(newWidget);

        auto xaml = Noesis::GUI::LoadXaml<Noesis::UserControl>(data.name.c_str());
        newWidget->view = Noesis::GUI::CreateView(xaml);
        newWidget->SetSize(LcSizef(xaml->GetWidth(), xaml->GetHeight()));

        return newWidgetPtr;
    }

};


Noesis::Key MapKeyboardKeys(int btn);
Noesis::MouseButton MapMouseKeys(LcMouseBtn btn);

LcNoesisGuiManager::LcNoesisGuiManager()
{
    isInit = false;
}

LcNoesisGuiManager::~LcNoesisGuiManager()
{
    if (isInit)
    {
        isInit = false;
        Shutdown();
    }
}

void LcNoesisGuiManager::NoesisInit(Noesis::Ptr<Noesis::XamlProvider> provider, const std::string& inResources)
{
    if (isInit) throw std::exception("LcNoesisGuiManager::NoesisInit(): Already initialized");

    xamlProvider = provider;
    resources = inResources;

    Noesis::SetLogHandler([](const char*, uint32_t, uint32_t level, const char*, const char* msg)
    {
        const char* prefixes[] = { "T", "D", "I", "W", "E" };
        DebugMsg("[NOESIS/%s] %s\n", prefixes[level], msg);
    });

    Noesis::GUI::SetLicense(NS_LICENSE_NAME, NS_LICENSE_KEY);
    Noesis::GUI::Init();

    if (xamlProvider) Noesis::GUI::SetXamlProvider(xamlProvider);
    if (!resources.empty()) Noesis::GUI::LoadApplicationResources(resources.c_str());

    Noesis::RegisterComponent<NoesisApp::BehaviorCollection>();
    Noesis::TypeOf<NoesisApp::Interaction>();

    isInit = true;
}

void LcNoesisGuiManager::Init(LcSize inViewportSize)
{
    viewportSize = inViewportSize;
}

void LcNoesisGuiManager::Shutdown()
{
    auto world = GetWorld();
    auto& widgetList = world->GetWidgets();

    for (auto& widget : widgetList)
    {
        if (auto noesisWidget = static_cast<LcNoesisWidget*>(widget.get()))
        {
            if (noesisWidget->view) noesisWidget->view.Reset();
        }
    }

    Noesis::GUI::Shutdown();
}

void LcNoesisGuiManager::OnKeyboard(int btn, LcKeyState state)
{
    auto world = GetWorld();
    auto& widgetList = world->GetWidgets();

    for (auto& widget : widgetList)
    {
        if (auto noesisWidget = static_cast<LcNoesisWidget*>(widget.get()))
        {
            switch (state)
            {
            case LcKeyState::Down:
                if (noesisWidget->view) noesisWidget->view->KeyDown(MapKeyboardKeys(btn));
                break;
            case LcKeyState::Up:
                if (noesisWidget->view) noesisWidget->view->KeyUp(MapKeyboardKeys(btn));
                break;
            }
        }
    }
}

void LcNoesisGuiManager::OnMouseButton(LcMouseBtn btn, LcKeyState state, int x, int y)
{
    auto world = GetWorld();
    auto& widgetList = world->GetWidgets();

    for (auto& widget : widgetList)
    {
        if (auto noesisWidget = static_cast<LcNoesisWidget*>(widget.get()))
        {
            switch (state)
            {
            case LcKeyState::Down:
                if (noesisWidget->view) noesisWidget->view->MouseButtonDown(x, y, MapMouseKeys(btn));
                break;
            case LcKeyState::Up:
                if (noesisWidget->view) noesisWidget->view->MouseButtonUp(x, y, MapMouseKeys(btn));
                break;
            }
        }
    }
}

void LcNoesisGuiManager::OnMouseMove(int x, int y)
{
    auto world = GetWorld();
    auto& widgetList = world->GetWidgets();

    for (auto& widget : widgetList)
    {
        if (auto noesisWidget = static_cast<LcNoesisWidget*>(widget.get()))
        {
            if (noesisWidget->view) noesisWidget->view->MouseMove(x, y);
        }
    }
}

Noesis::Key MapKeyboardKeys(int btn)
{
    return Noesis::Key_None;
}

Noesis::MouseButton MapMouseKeys(LcMouseBtn btn)
{
    switch (btn)
    {
    case LcMouseBtn::Right: return Noesis::MouseButton_Right;
    case LcMouseBtn::Middle: return Noesis::MouseButton_Middle;
    }

    return Noesis::MouseButton_Left;
}

INoesisGuiManagerPtr GetGuiManager()
{
    return INoesisGuiManagerPtr(new LcNoesisGuiManager());
}

IWidgetFactoryPtr GetWidgetFactory()
{
    return IWidgetFactoryPtr(new LcNoesisWidgetFactory());
}
