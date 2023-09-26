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
        auto onInitHandler = [](const LcAppContext& context)
        {
            auto& spriteHelper = context.world->GetSpriteHelper();
            if (context.world->AddSprite(500, 400, LcLayers::Z2, 200, 200))
            {
                spriteHelper.AddTintComponent(LcColor3(0.0f, 0.8f, 0.0f));
            }

            if (context.world->AddSprite(500, 340, LcLayers::Z1, 180, 60))
            {
                spriteHelper.AddTintComponent(LcColor3(0.9f, 0.9f, 0.9f));
            }

            auto& widgetHelper = context.world->GetWidgetHelper();
            if (context.world->AddWidget(500, 340, 200, 50))
            {
                widgetHelper.AddTextComponent(L"Label Text", LcDefaults::Black4, L"Calibri", 30);
            }

            if (context.world->AddWidget(500, 400, 32, 32))
            {
                auto onCheck = [](bool checked) {
                    DebugMsg("Checkbox is %s\n", checked ? "checked" : "unchecked");
                };
                widgetHelper.AddCheckHandlerComponent(onCheck);
            }

            if (context.world->AddWidget(500, 450, 124, 40))
            {
                widgetHelper.AddClickHandlerComponent([]() { DebugMsg("SUBMIT button pressed\n"); });
                widgetHelper.AddTextComponent(L"SUBMIT");
            }
        };

        auto onKeysHandler = [](int key, LcKeyState keyEvent, const LcAppContext& context)
        {
            if (key == 'Q' && keyEvent == LcKeyState::Down) context.app->RequestQuit();
        };

        auto app = GetApp();
        app->SetRenderSystem(GetRenderSystem());
        app->SetGuiManager(GetGuiManager());
        app->SetInitHandler(onInitHandler);
        app->GetInputSystem()->SetKeysHandler(onKeysHandler);
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
