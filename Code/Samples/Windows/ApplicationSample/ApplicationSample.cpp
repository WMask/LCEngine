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
#include "World/SpriteInterface.h"
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
            world->GetWorldScale().GetScaleList().clear();
            world->GetWorldScale().GetScaleList().insert({ {1920, 1080}, {1.4f, 1.4f} });
            world->GetWorldScale().GetScaleList().insert({ {1600, 900}, {1.2f, 1.2f} });
            world->GetWorldScale().GetScaleList().insert({ {1280, 720}, {1.0f, 1.0f} });

            // sprites
            auto& spriteHelper = world->GetSpriteHelper();
            if (world->AddSprite2D(200, 200, 300, 300))
            {
                spriteHelper.AddTintComponent(LcColor3(0.7f, 0.7f, 0.7f));
            }

            if (world->AddSprite(550, 200, LcLayers::Z1, 300, 300))
            {
                spriteHelper.AddTextureComponent("../../Assets/tree.png");
                spriteHelper.AddColorsComponent(LcColor3(0, 1, 0), LcColor3(0, 1, 0), LcColor3(1, 0, 0), LcColor3(1, 0, 0));
            }

            if (world->AddSprite2D(460, 315, 100, 100))
            {
                spriteHelper.AddTextureComponent("../../Assets/anim.png");
                spriteHelper.AddAnimationComponent(LcSizef(128, 128), 10, 12);
            }

            // window resize controls
            auto& widgetHelper = world->GetWidgetHelper();
            if (world->AddWidget(45, 16, 80, 32))
            {
                widgetHelper.AddAlignedTextComponent(L"FPS: 0", LcDefaults::White4, LcTextAlignment::Left, L"Calibri", 18);
            }

            if (world->AddWidget(182, 550, 94, 32))
            {
                widgetHelper.AddTextComponent(L"Fullscreen", LcDefaults::White4, L"Calibri", 18);
            }

            if (world->AddWidget(242, 552, 32, 32))
            {
                auto onToggleMode = [app](bool fullscreen) {
                    app->SetWindowMode(fullscreen ? LcWinMode::Fullscreen : LcWinMode::Windowed);
                };
                widgetHelper.AddCheckHandlerComponent(onToggleMode);
            }

            if (world->AddWidget(200, 500, 124, 40))
            {
                widgetHelper.AddClickHandlerComponent([app]() { app->SetWindowSize(1920, 1080); });
                widgetHelper.AddTextComponent(L"1920x1080");
            }

            if (world->AddWidget(200, 450, 124, 40))
            {
                widgetHelper.AddClickHandlerComponent([app]() { app->SetWindowSize(1600, 900); });
                widgetHelper.AddTextComponent(L"1600x900");
            }

            if (world->AddWidget(200, 400, 124, 40))
            {
                widgetHelper.AddClickHandlerComponent([app]() { app->SetWindowSize(1280, 720); });
                widgetHelper.AddTextComponent(L"1280x720");
            }
        };

        auto onUpdateHandler = [](float deltaSeconds, IApplication* app)
        {
            DebugMsg("FPS: %.3f\n", (1.0f / deltaSeconds));

            if (auto widget = app->GetWorld()->GetWidgets()[0])
            {
                widget->GetTextComponent()->SetText(L"FPS: " + ToStringW(int(1.0f / deltaSeconds)));
            }

            auto sprite = app->GetWorld()->GetSprites()[0];
            auto value = sin(double(GetTickCount64()) / 1000.0);
            auto tint = float(abs(value));
            sprite->GetTintComponent()->SetColor(LcColor4(1.0f - tint, tint, 0.0f, 1.0f));
        };

        auto onKeysHandler = [](int key, LcKeyState keyEvent, IApplication* app)
        {
            if (key == 'Q' || key == LcJKeys::Menu) app->RequestQuit();
        };

        LCAppConfig cfg;
        LoadConfig(cfg, "../../Assets/config.txt");

        auto app = GetApp();
        app->SetVSync(true);
        app->SetRenderSystem(GetRenderSystem());
        app->SetGuiManager(GetGuiManager());
        app->SetInitHandler(onInitHandler);
        app->SetUpdateHandler(onUpdateHandler);
        app->GetInputSystem()->SetKeysHandler(onKeysHandler);
        app->SetWindowSize(cfg.WinWidth, cfg.WinHeight);
        app->Init(hInstance);
        app->Run();
    }
    catch (const std::exception& ex)
    {
        DebugMsg("\n# ApplicationSample error: %s\n\n", ex.what());
    }

    return 0;
}
