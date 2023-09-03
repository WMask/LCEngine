/**
* ApplicationSample.cpp
* 03.09.2023
* (c) Denis Romakhov
*/

#include "framework.h"
#include "ApplicationSample.h"
#include "Application/AppConfig.h"
#include "Application/Application.h"
#include "Application/Windows/Module.h"
#include "RenderSystem/RenderSystemDX10/Module.h"
#include "World/WorldInterface.h"
#include "World/Sprites.h"
#include "GUI/Widgets.h"
#include "Core/LCUtils.h"


#define ADD_DEFAULT_BUTTON(button, text) \
    button->AddButtonComponent("../../Assets/button.png", LcVector2(2.0f, 2.0f), LcVector2(2.0f, 44.0f), LcVector2(2.0f, 86.0f)); \
    button->AddTextComponent(text, L"Calibri", 22, LcDefaults::Black4);

#define ADD_DEFAULT_CHECKBOX(checkbox) \
    checkbox->AddCheckboxComponent("../../Assets/checkbox.png", \
        LcVector2(0.0f, 0.0f), LcVector2(32.0f, 0.0f), LcVector2(0.0f, 32.0f), LcVector2(32.0f, 32.0f));


int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);

    try
    {
        bool windowedMode = true;

        auto onInitHandler = [&windowedMode](IApplication* app)
        {
            auto world = app->GetWorld();

            // sprites
            if (auto sprite1 = world->AddSprite(150, 150, 200, 200))
            {
                sprite1->AddTintComponent(LcColor3(0.7f, 0.7f, 0.7f));
            }

            if (auto sprite2 = world->AddSprite(450, 200, 300, 300))
            {
                sprite2->AddTextureComponent("../../Assets/tree.png");
                sprite2->AddColorsComponent(LcColor3(0, 1, 0), LcColor3(0, 1, 0), LcColor3(1, 0, 0), LcColor3(1, 0, 0));
            }

            if (auto sprite3 = world->AddSprite(380, 85, 100, 100))
            {
                sprite3->AddTextureComponent("../../Assets/anim.png");
                sprite3->AddAnimationComponent(LcSizef(128, 128), 10, 12);
            }

            // window resize controls
            if (auto label = world->AddWidget(130, 450, 100, 32))
            {
                label->AddTextComponent(L"Fullscreen", L"Calibri", 18, LcDefaults::White4);
            }

            if (auto widget = world->AddWidget(195, 450, 32, 32))
            {
                auto onToggleMode = [&windowedMode, app](bool checked) {
                    windowedMode = !windowedMode;
                    app->SetWindowMode(windowedMode ? LcWinMode::Windowed : LcWinMode::Fullscreen);
                };
                widget->AddCheckHandlerComponent(onToggleMode);
                ADD_DEFAULT_CHECKBOX(widget);
            }

            if (auto button1 = world->AddWidget(150, 400, 124, 40))
            {
                button1->AddClickHandlerComponent([app]() { app->SetWindowSize(1920, 1080); });
                ADD_DEFAULT_BUTTON(button1, L"1920x1080");
            }

            if (auto button2 = world->AddWidget(150, 350, 124, 40))
            {
                button2->AddClickHandlerComponent([app]() { app->SetWindowSize(1600, 900); });
                ADD_DEFAULT_BUTTON(button2, L"1600x900");
            }

            if (auto button3 = world->AddWidget(150, 300, 124, 40))
            {
                button3->AddClickHandlerComponent([app]() { app->SetWindowSize(1280, 720); });
                ADD_DEFAULT_BUTTON(button3, L"1280x720");
            }
        };

        KEYS keys;
        auto onUpdateHandler = [&keys](float deltaSeconds, IApplication* app)
        {
            DebugMsg("FPS: %.3f\n", (1.0f / deltaSeconds));

            auto sprite = app->GetWorld()->GetSprites()[0];
            auto value = sin(double(GetTickCount64()) / 1000.0);
            auto tint = float(abs(value));
            sprite->GetTintComponent()->SetColor(LcColor4(1.0f - tint, tint, 0.0f, 1.0f));
        };

        auto onKeyboardHandler = [&keys](int key, LcKeyState keyEvent, IApplication* app)
        {
            keys[key] = (keyEvent == LcKeyState::Down) ? 1 : 0;

            if (key == 'Q') app->RequestQuit();
        };

        auto cfg = LoadConfig("../../Assets/config.txt");
        int winWidth = cfg["appWinWidth"].iValue;
        int winHeight = cfg["appWinHeight"].iValue;

        auto app = GetApp();
        auto world = GetWorld();
        app->SetRenderSystem(GetRenderSystem());
        app->SetGuiManager(GetGuiManager());
        app->SetInitHandler(onInitHandler);
        app->SetUpdateHandler(onUpdateHandler);
        app->SetKeyboardHandler(onKeyboardHandler);
        app->SetWindowSize(winWidth, winHeight);
        app->Init(hInstance, world);
        app->Run();
    }
    catch (const std::exception& ex)
    {
        DebugMsg("\n# ApplicationSample error: %s\n\n", ex.what());
    }

    return 0;
}
