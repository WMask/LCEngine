/**
* NoesisGuiManager.cpp
* 02.02.2023
* (c) Denis Romakhov
*/

#include "pch.h"
#include "GUI/NoesisGUI/NoesisGUIManager.h"
#include "GUI/NoesisGUI/NoesisRenderContextD3D10.h"
#include "GUI/NoesisGUI/NoesisWidget.h"
#include "Core/LCUtils.h"

// copy NoesisGUI code to LCEngine/Code/Engine/GUI/NoesisGUI
#include <NsGui/IntegrationAPI.h>
#include <NsCore/RegisterComponent.h>
#include <NsApp/BehaviorCollection.h>
#include <NsApp/Interaction.h>


LcNoesisGuiManager::LcNoesisGuiManager(LcRenderContext* inContext)
{
    context = inContext;
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

void LcNoesisGuiManager::NoesisInit(
    Noesis::Ptr<Noesis::XamlProvider> xamls,
    Noesis::Ptr<Noesis::TextureProvider> textures,
    Noesis::Ptr<Noesis::FontProvider> fonts,
    const char* resources, const char* inShadersPath)
{
    if (isInit) throw std::exception("LcNoesisGuiManager::NoesisInit(): Already initialized");

    xamlProvider = xamls;
    textureProvider = textures;
    fontProvider = fonts;
    shadersPath = inShadersPath;

    Noesis::SetLogHandler([](const char*, uint32_t, uint32_t level, const char*, const char* msg)
    {
        const char* prefixes[] = { "T", "D", "I", "W", "E" };
        DebugMsg("[NOESIS/%s] %s\n", prefixes[level], msg);
    });

    Noesis::GUI::SetLicense("Denis Korablev", "iBt72mhQe9raMxzrGjLz5NT8sTp8LRrz4se3KToFhnr5kZ4K");
    Noesis::GUI::Init();

    if (textureProvider) Noesis::GUI::SetTextureProvider(textureProvider);
    if (fontProvider) Noesis::GUI::SetFontProvider(fontProvider);
    if (xamlProvider)
    {
        Noesis::GUI::SetXamlProvider(xamlProvider);

        if (resources) Noesis::GUI::LoadApplicationResources(resources);
    }

    Noesis::RegisterComponent<NoesisApp::BehaviorCollection>();
    Noesis::TypeOf<NoesisApp::Interaction>();

    isInit = true;
}

void LcNoesisGuiManager::Init(void* window, LcSize inViewportSize)
{
    viewportSize = inViewportSize;

    if (context)
    {
        uint32_t samples;
        context->SetShadersPath(shadersPath.c_str());
        context->Init(window, samples, true, false);
        context->Resize();
        context->SetClearColor(0.0f, 0.0f, 1.0f, 1.0f);
    }

    IGuiManager::Init(window, inViewportSize);
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

    textureProvider.Reset();
    fontProvider.Reset();
    xamlProvider.Reset();

    if (context)
    {
        context->Shutdown();
        context = nullptr;
    }

    Noesis::GUI::Shutdown(); // default memory leak is 4112 bytes
}

INoesisGuiManagerPtr GetGuiManager()
{
    return INoesisGuiManagerPtr(new LcNoesisGuiManager(GetContext()));
}
