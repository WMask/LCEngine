/**
* WidgetSample.cpp
* 30.08.2023
* (c) Denis Romakhov
*/

#include "framework.h"
#include "WidgetSample.h"
#include "Application/AppConfig.h"
#include "Application/Application.h"
#include "Application/Windows/Module.h"
#include "RenderSystem/RenderSystemDX10/Module.h"
#include "World/WorldInterface.h"
#include "World/Sprites.h"
#include "GUI/Widgets/LabelWidget.h"
#include "GUI/Module.h"
#include "Core/LCUtils.h"


int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);

    try
    {
        auto onInitHandler = [](IApplication* app)
        {
            auto world = app->GetWorld();

            if (auto sprite = world->AddSprite(500, 400, 200, 200))
            {
                sprite->AddTintComponent(LcColor3(0.0f, 0.8f, 0.0f));
            }

            world->AddWidget(std::make_shared<LcLabelWidget>(
                L"Label Text", L"Calibri", 40, LcDefaults::White4, To3(LcVector2(500, 400)), LcSizef(200, 200)));
        };

        auto onKeyboardHandler = [](int key, LcKeyState keyEvent, IApplication* app)
        {
            if (key == 'Q' && keyEvent == LcKeyState::Down) app->RequestQuit();
        };

        auto cfg = LoadConfig();
        int winWidth = cfg["appWinWidth"].iValue;
        int winHeight = cfg["appWinHeight"].iValue;

        auto app = GetApp();
        auto world = GetWorld();
        app->SetRenderSystem(GetRenderSystem());
        app->SetGuiManager(GetGuiManager());
        app->SetInitHandler(onInitHandler);
        app->SetKeyboardHandler(onKeyboardHandler);
        app->SetWindowSize(winWidth, winHeight);
        app->Init(hInstance, world);
        app->Run();
    }
    catch (const std::exception& ex)
    {
        DebugMsg("\n# WidgetSample error: %s\n\n", ex.what());
    }

    return 0;
}
