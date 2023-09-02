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
#include "GUI/Widgets.h"
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

            if (auto label = world->AddWidget(500, 450, 200, 100))
            {
                label->AddTextComponent(L"Label Text", L"Calibri", 30, LcDefaults::White4);
            }

            if (auto button = world->AddWidget(500, 350, 124, 40))
            {
                button->AddButtonComponent("../../Assets/button.png", LcVector2(2.0f, 2.0f), LcVector2(2.0f, 44.0f), LcVector2(2.0f, 86.0f));
                button->AddTextComponent(L"SUBMIT", L"Calibri", 22, LcDefaults::Black4);
                button->AddClickHandlerComponent([]() { DebugMsg("SUBMIT button pressed!\n"); });
            }
        };

        auto onKeyboardHandler = [](int key, LcKeyState keyEvent, IApplication* app)
        {
            if (key == 'Q' && keyEvent == LcKeyState::Down) app->RequestQuit();
        };

        auto app = GetApp();
        auto world = GetWorld();
        app->SetRenderSystem(GetRenderSystem());
        app->SetGuiManager(GetGuiManager());
        app->SetInitHandler(onInitHandler);
        app->SetKeyboardHandler(onKeyboardHandler);
        app->SetWindowSize(1024, 768);
        app->Init(hInstance, world);
        app->Run();
    }
    catch (const std::exception& ex)
    {
        DebugMsg("\n# WidgetSample error: %s\n\n", ex.what());
    }

    return 0;
}
