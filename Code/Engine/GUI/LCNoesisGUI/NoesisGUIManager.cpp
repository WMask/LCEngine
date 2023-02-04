/**
* NoesisGuiManager.cpp
* 02.02.2023
* (c) Denis Romakhov
*/

#include "pch.h"
#include "GUI/LCNoesisGUI/NoesisGUIManager.h"
#include "GUI/LCNoesisGUI/NoesisRenderContextD3D10.h"
#include "GUI/LCNoesisGUI/NoesisRenderDeviceD3D10.h"
#include "GUI/LCNoesisGUI/NoesisWidget.h"
#include "Core/LCUtils.h"

#include <NsGui/IntegrationAPI.h>
#include <NsCore/RegisterComponent.h>
#include <NsApp/BehaviorCollection.h>
#include <NsApp/Interaction.h>


LcNoesisGuiManager::LcNoesisGuiManager()
{
    context.reset(new LcNoesisRenderContextD3D10());
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

void LcNoesisGuiManager::NoesisInit(Noesis::Ptr<Noesis::XamlProvider> provider, const std::string& resources)
{
    if (isInit) throw std::exception("LcNoesisGuiManager::NoesisInit(): Already initialized");

    xamlProvider = provider;

    Noesis::SetLogHandler([](const char*, uint32_t, uint32_t level, const char*, const char* msg)
    {
        const char* prefixes[] = { "T", "D", "I", "W", "E" };
        DebugMsg("[NOESIS/%s] %s\n", prefixes[level], msg);
    });

    Noesis::GUI::SetLicense(NS_LICENSE_NAME, NS_LICENSE_KEY);
    Noesis::GUI::Init();

    if (xamlProvider)
    {
        Noesis::GUI::SetXamlProvider(xamlProvider);

        if (!resources.empty()) Noesis::GUI::LoadApplicationResources(resources.c_str());
    }

    Noesis::RegisterComponent<NoesisApp::BehaviorCollection>();
    Noesis::TypeOf<NoesisApp::Interaction>();

    isInit = true;
}

void LcNoesisGuiManager::Init(void* window, LcSize inViewportSize)
{
    viewportSize = inViewportSize;

    uint32_t samples;
    context->Init(window, samples, true, false);
    context->Resize();
    context->SetClearColor(0.0f, 0.0f, 1.0f, 1.0f);
}

void LcNoesisGuiManager::Render()
{
    if (!context) return;

    PreRender();

    context->SetDefaultRenderTarget(0, 0, true);

    PostRender();

    context->Swap();
}

void LcNoesisGuiManager::Shutdown()
{
    auto world = GetWorld();
    auto& widgetList = world->GetWidgets();
    widgetList.clear();

    xamlProvider.Reset();

    if (context)
    {
        context->Shutdown();
        context.reset();
    }

    Noesis::GUI::Shutdown(); // default memory leak is 4112 bytes
}

INoesisGuiManagerPtr GetGuiManager()
{
    return INoesisGuiManagerPtr(new LcNoesisGuiManager());
}

IWidgetFactoryPtr GetWidgetFactory(INoesisGuiManager* gui)
{
    LcNoesisGuiManager* guiManager = (LcNoesisGuiManager*)gui;
    return IWidgetFactoryPtr(new LcNoesisWidgetFactory(guiManager->context.get()));
}
