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


LCGUIManager& LCGUIManager::GetInstance()
{
	static LCGUIManager instance;
	return instance;
}

LCGUIManager::~LCGUIManager()
{
}

LCGUIManager::LCGUIManager()
{
    useNoesis = false;
}

LCGUIManager::LCGUIManager(const LCGUIManager&)
{
    useNoesis = false;
}

LCGUIManager& LCGUIManager::operator=(const LCGUIManager&)
{
	return *this;
}

void LCGUIManager::Init(bool inUseNoesis)
{
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
        view->SetSize(800, 600);
    }
}

void LCGUIManager::Shutdown()
{
    if (useNoesis)
    {
        useNoesis = false;
        view.Reset();
        Noesis::GUI::Shutdown();
    }
}

void LCGUIManager::MouseButtonDown(int x, int y)
{
    if (view) view->MouseButtonDown(x, y, Noesis::MouseButton_Left);
}

void LCGUIManager::MouseButtonUp(int x, int y)
{
    if (view) view->MouseButtonUp(x, y, Noesis::MouseButton_Left);
}
