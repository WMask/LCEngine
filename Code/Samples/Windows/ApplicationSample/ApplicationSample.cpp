/**
* ApplicationSample.cpp
* 03.09.2023
* (c) Denis Romakhov
*/

#include "framework.h"
#include "ApplicationSample.h"
#include "Application/AppConfig.h"
#include "Application/Windows/Module.h"
#include "RenderSystem/RenderSystemDX10/Module.h"
#include "World/WorldInterface.h"
#include "GUI/WidgetInterface.h"
#include "World/Sprites.h"
#include "Core/LCUtils.h"


int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);

    try
    {
        auto onInitHandler = [](IApplication* app)
        {
            auto world = app->GetWorld();
            world->GetWorldScale().scaleList.clear();
            world->GetWorldScale().scaleList.insert({ {1920, 1080}, {1.4f, 1.4f} });
            world->GetWorldScale().scaleList.insert({ {1600, 900}, {1.2f, 1.2f} });
            world->GetWorldScale().scaleList.insert({ {1280, 720}, {1.0f, 1.0f} });

            // sprites
            if (auto sprite1 = world->AddSprite2D(200, 200, 300, 300))
            {
                sprite1->AddTintComponent(LcColor3(0.7f, 0.7f, 0.7f));
            }

            if (auto sprite2 = world->AddSprite2D(550, 200, 300, 300))
            {
                sprite2->AddTextureComponent("../../Assets/tree.png");
                sprite2->AddColorsComponent(LcColor3(0, 1, 0), LcColor3(0, 1, 0), LcColor3(1, 0, 0), LcColor3(1, 0, 0));
            }

            if (auto sprite3 = world->AddSprite2D(460, 315, 100, 100))
            {
                sprite3->AddTextureComponent("../../Assets/anim.png");
                sprite3->AddAnimationComponent(LcSizef(128, 128), 10, 12);
            }

            // window resize controls
            if (auto label = world->AddWidget(182, 550, 100, 32))
            {
                label->AddTextComponent(L"Fullscreen", LcDefaults::White4, L"Calibri", 18);
            }

            if (auto widget = world->AddWidget(242, 552, 32, 32))
            {
                auto onToggleMode = [app](bool fullscreen) {
                    app->SetWindowMode(fullscreen ? LcWinMode::Fullscreen : LcWinMode::Windowed);
                };
                widget->AddCheckHandlerComponent(onToggleMode);
            }

            if (auto button1 = world->AddWidget(200, 500, 124, 40))
            {
                button1->AddClickHandlerComponent([app]() { app->SetWindowSize(1920, 1080); });
                button1->AddTextComponent(L"1920x1080");
            }

            if (auto button2 = world->AddWidget(200, 450, 124, 40))
            {
                button2->AddClickHandlerComponent([app]() { app->SetWindowSize(1600, 900); });
                button2->AddTextComponent(L"1600x900");
            }

            if (auto button3 = world->AddWidget(200, 400, 124, 40))
            {
                button3->AddClickHandlerComponent([app]() { app->SetWindowSize(1280, 720); });
                button3->AddTextComponent(L"1280x720");
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
        app->SetVSync(true);
        app->SetRenderSystem(GetRenderSystem());
        app->SetGuiManager(GetGuiManager());
        app->SetInitHandler(onInitHandler);
        app->SetUpdateHandler(onUpdateHandler);
        app->SetKeyboardHandler(onKeyboardHandler);
        app->SetWindowSize(winWidth, winHeight);
        app->Init(hInstance);
        app->Run();
    }
    catch (const std::exception& ex)
    {
        DebugMsg("\n# ApplicationSample error: %s\n\n", ex.what());
    }

    return 0;
}
