/**
* GUIManager.cpp
* 29.01.2023
* (c) Denis Romakhov
*/

#include "pch.h"
#include "GUI/GUIManager.h"
#include "Core/LCUtils.h"

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

        auto file = ReadTextFile("NoesisSample_Blend/MainMenu.xaml");
        auto xaml = Noesis::GUI::ParseXaml(file.c_str());
        auto control = Noesis::DynamicCast<Noesis::UserControl*>(xaml.GetPtr());
        auto grid = Noesis::DynamicCast<Noesis::Grid*>(control->GetContent());
        for (int i = 0; i < grid->GetChildren()->Count(); i++)
        {
            auto rect = Noesis::DynamicCast<Noesis::Rectangle*>(grid->GetChildren()->Get(i));
            auto brush = Noesis::DynamicCast<Noesis::SolidColorBrush*>(rect->GetFill());
            auto color = brush->GetColor();
            auto node = grid->GetChildren()->Get(i);
            node = nullptr;
        }
    }
}

void LCGUIManager::Shutdown()
{
    if (useNoesis)
    {
        useNoesis = false;
        Noesis::GUI::Shutdown();
    }
}
