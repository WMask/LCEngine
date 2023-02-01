/**
* GUIManager.cpp
* 29.01.2023
* (c) Denis Romakhov
*/

#include "pch.h"
#include "GUI/GUIManager.h"
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


LcGUIManager& LcGUIManager::GetInstance()
{
	static LcGUIManager instance;
	return instance;
}

LcGUIManager::~LcGUIManager()
{
}

LcGUIManager::LcGUIManager()
{
    useNoesis = false;
}

LcGUIManager::LcGUIManager(const LcGUIManager&)
{
    useNoesis = false;
}

LcGUIManager& LcGUIManager::operator=(const LcGUIManager&)
{
	return *this;
}

void LcGUIManager::Init(LcSize inViewportSize, bool inUseNoesis)
{
    viewportSize = inViewportSize;
    useNoesis = inUseNoesis;

    if (useNoesis)
    {
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
}

void LcGUIManager::Shutdown()
{
    if (useNoesis)
    {
        useNoesis = false;
        view.Reset();
        Noesis::GUI::Shutdown();
    }
}

void LcGUIManager::MouseButtonDown(LcMouseBtn btn, int x, int y)
{
    if (view) view->MouseButtonDown(x, y, Noesis::MouseButton_Left);
}

void LcGUIManager::MouseButtonUp(LcMouseBtn btn, int x, int y)
{
    if (view) view->MouseButtonUp(x, y, Noesis::MouseButton_Left);
}
