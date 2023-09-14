/**
* WidgetSample.cpp
* 30.08.2023
* (c) Denis Romakhov
*/

#include "framework.h"
#include "WidgetSample.h"
#include "Application/Windows/Module.h"
#include "RenderSystem/RenderSystemDX10/Module.h"
#include "World/SpriteInterface.h"
#include "World/WorldInterface.h"
#include "GUI/WidgetInterface.h"
#include "Core/LCUtils.h"


int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);

    try
    {
        auto onInitHandler = [](IApplication* app)
        {
            auto world = app->GetWorld();

            if (auto sprite = world->AddSprite3D(500, 400, -0.1f, 200, 200))
            {
                sprite->AddTintComponent(LcColor3(0.0f, 0.8f, 0.0f));
            }

            if (auto label = world->AddWidget(500, 350, 200, 100))
            {
                label->AddTextComponent(L"Label Text", LcDefaults::White4, L"Calibri", 30);
            }

            if (auto widget = world->AddWidget(500, 400, 32, 32))
            {
                auto onCheck = [](bool checked) {
                    DebugMsg("Checkbox is %s\n", checked ? "checked" : "unchecked");
                };
                widget->AddCheckHandlerComponent(onCheck);
            }

            if (auto button = world->AddWidget(500, 450, 124, 40))
            {
                button->AddClickHandlerComponent([]() { DebugMsg("SUBMIT button pressed\n"); });
                button->AddTextComponent(L"SUBMIT");
            }
        };

        auto onKeyboardHandler = [](int key, LcKeyState keyEvent, IApplication* app)
        {
            if (key == 'Q' && keyEvent == LcKeyState::Down) app->RequestQuit();
        };

        auto app = GetApp();
        app->SetRenderSystem(GetRenderSystem());
        app->SetGuiManager(GetGuiManager());
        app->SetInitHandler(onInitHandler);
        app->SetKeyboardHandler(onKeyboardHandler);
        app->SetWindowSize(1024, 768);
        app->Init(hInstance);
        app->Run();
    }
    catch (const std::exception& ex)
    {
        DebugMsg("\n# WidgetSample error: %s\n\n", ex.what());
    }

    return 0;
}
