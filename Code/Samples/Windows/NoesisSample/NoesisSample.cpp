/**
* NoesisSample.cpp
* 29.01.2023
* (c) Denis Romakhov
*/

#include "framework.h"
#include "NoesisSample.h"
#include "Application/Windows/Module.h"
#include "RenderSystem/RenderSystemDX10/Module.h"
#include "GUI/NoesisGUI/NoesisUtils.h"
#include "Core/LCUtils.h"
#include "World/World.h"
#include "MainMenuViewModel.h"
#include "ClickMoveBehavior.h"

#include <NsCore/ReflectionImplement.h>
#include <NsApp/EmbeddedXamlProvider.h>
#include <NsApp/EmbeddedTextureProvider.h>
#include <NsCore/RegisterComponent.h>
#include <NsGui/IntegrationAPI.h>
#include <NsGui/UserControl.h>

#include "x64/Debug/MainMenu.xaml.bin.h"
#include "x64/Debug/SampleDictionary.xaml.bin.h"


class MainMenu : public Noesis::UserControl
{
public:
    MainMenu() {}
    //
    ~MainMenu() {}
    //
    NS_IMPLEMENT_INLINE_REFLECTION(MainMenu, UserControl, "common.MainMenu")
    {
        NS_UNUSED(helper);
    }
};


int APIENTRY wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);

    try
    {
        auto world = GetWorld();
        auto app = GetApp();
        TWeakApp weakApp(app);

        LcSizef size(200, 200);
        LcVector3 pos(200, 200, 0);
        LcSpriteColors colors(LcColor4(1, 0, 0, 1), LcColor4(0, 0, 0, 1), LcColor4(1, 0, 1, 1), LcColor4(0, 1, 0, 1));
        auto sprite = world->AddSprite(LcSpriteData(LcSpriteType::Colored, pos, size, colors));

        BYTE keys[256];
        memset(keys, 0, sizeof(keys));

        auto onUpdateHandler = [weakApp, sprite, &keys](float DeltaSeconds) {
            DebugMsg("FPS: %.3f\n", (1.0f / DeltaSeconds));
        };

        auto onKeyboardHandler = [weakApp, world, &keys](int key, LcKeyState keyEvent) {
            keys[key] = (keyEvent == LcKeyState::Down) ? 1 : 0;

            if (auto app = weakApp.lock())
            {
                if (keyEvent == LcKeyState::Down)
                {
                    if (key == 'Q') app->RequestQuit();
                    if (key == 'N') world->AddWidget(LcWidgetData("MainMenu.xaml", LcVector3(600, 0, 0)));
                }
            }
        };

        Noesis::RegisterComponent<MainMenu>();
        Noesis::RegisterComponent<MainMenuViewModel>();
        Noesis::RegisterComponent<ClickMoveBehavior>();

        NoesisApp::EmbeddedXaml xamls[] =
        {
            { "MainMenu.xaml", MainMenu_xaml },
            { "SampleDictionary.xaml", SampleDictionary_xaml }
        };

        auto tex = ReadBinaryFile("Media/338.jpg");
        NoesisApp::EmbeddedTexture textures[] =
        {
            { "Media/338.jpg", ToArray(tex) }
        };

        auto guiManager = GetGuiManager();
        guiManager->NoesisInit(
            *new NoesisApp::EmbeddedXamlProvider(xamls),
            *new NoesisApp::EmbeddedTextureProvider(textures),
            "SampleDictionary.xaml", "../../../Shaders/NoesisSM4/");

        world->SetWidgetFactory(GetWidgetFactory());
        auto widget = world->AddWidget(LcWidgetData("MainMenu.xaml", LcVector3::Zero()));
        widget->SetActive(true);

        app->SetGuiManager(std::move(guiManager));
        app->SetUpdateHandler(onUpdateHandler);
        app->SetKeyboardHandler(onKeyboardHandler);
        app->SetWindowSize(LcSize(1024, 768));
        app->Init(hInstance, lpCmdLine);
        app->Run();
    }
    catch (const std::exception& ex)
    {
        DebugMsg("\n# HelloWorld error: %s\n\n", ex.what());
    }

    return 0;
}
