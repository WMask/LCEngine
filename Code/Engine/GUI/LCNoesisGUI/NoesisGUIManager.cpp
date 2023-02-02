/**
* GUIManager.cpp
* 29.01.2023
* (c) Denis Romakhov
*/

#include "pch.h"
#include "GUI/LCNoesisGUI/NoesisGUIManager.h"
#include "Core/LCUtils.h"

#include <NsGui/IntegrationAPI.h>
#include <NsGui/UIElementCollection.h>
#include <NsGui/FontProperties.h>
#include <NsGui/IntegrationAPI.h>
#include <NsGui/IRenderer.h>
#include <NsGui/IView.h>
#include <NsGui/Grid.h>
#include <NsGui/Page.h>
#include <NsGui/Rectangle.h>
#include <NsGui/Brush.h>
#include <NsGui/SolidColorBrush.h>
#include <NsDrawing/Color.h>


Noesis::MouseButton MapMouseKeys(LcMouseBtn btn);

LcNoesisGuiManager::LcNoesisGuiManager()
{
}

LcNoesisGuiManager::~LcNoesisGuiManager()
{
}

void LcNoesisGuiManager::Init(LcSize inViewportSize)
{
    viewportSize = inViewportSize;

    Noesis::SetLogHandler([](const char*, uint32_t, uint32_t level, const char*, const char* msg)
    {
        const char* prefixes[] = { "T", "D", "I", "W", "E" };
        DebugMsg("[NOESIS/%s] %s\n", prefixes[level], msg);
    });

    Noesis::GUI::SetLicense(NS_LICENSE_NAME, NS_LICENSE_KEY);
    Noesis::GUI::Init();
    Noesis::GUI::SetXamlProvider(xamlProvider);
    Noesis::GUI::LoadApplicationResources("SampleDictionary.xaml");

    auto xaml = Noesis::GUI::LoadXaml<Noesis::FrameworkElement>("MainMenu.xaml");
    view = Noesis::GUI::CreateView(xaml);
    view->SetSize(inViewportSize.x(), inViewportSize.y());
}

void LcNoesisGuiManager::Shutdown()
{
    if (view) view.Reset();
    Noesis::GUI::Shutdown();
}

void LcNoesisGuiManager::OnKeyboard(LcMouseBtn btn, int x, int y)
{
}

void LcNoesisGuiManager::OnMouseButton(LcMouseBtn btn, LcKeyState state, int x, int y)
{
    if (view) view->MouseButtonDown(x, y, MapMouseKeys(btn));
}

void LcNoesisGuiManager::OnMouseMove(int x, int y)
{
    if (view) view->MouseMove(x, y);
}

std::shared_ptr<INoesisGuiManager> GetGuiManager()
{
    return std::shared_ptr<INoesisGuiManager>(new LcNoesisGuiManager());
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
