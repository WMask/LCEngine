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
            auto& widgetHelper = context.world->GetWidgetHelper();

            IWidget* parent = nullptr;
            if (parent = context.world->AddWidget(500, 400, LcLayers::Z2, 200, 200))
            {
                widgetHelper.AddTintComponent(LcColor3(0.0f, 0.8f, 0.0f));
                widgetHelper.SetTag(1);
            }

            if (auto child = context.world->AddWidget(500, 340, LcLayers::Z1, 180, 60))
            {
                widgetHelper.AddTintComponent(LcColor3(0.9f, 0.9f, 0.9f));
                parent->AddChild(child);
            }

            if (auto child = context.world->AddWidget(500, 340, 200, 50))
            {
                widgetHelper.AddTextComponent(L"Label Text", LcDefaults::Black4, L"Calibri", 30);
                parent->AddChild(child);
            }

            if (auto child = context.world->AddWidget(500, 400, 32, 32))
            {
                auto onCheck = [](bool checked) {
                    DebugMsg("Checkbox is %s\n", checked ? "checked" : "unchecked");
                };
                widgetHelper.AddCheckHandlerComponent(onCheck);
                parent->AddChild(child);
            }

            if (auto child = context.world->AddWidget(500, 450, 124, 40))
            {
                widgetHelper.AddClickHandlerComponent([]() { DebugMsg("SUBMIT button pressed\n"); });
                widgetHelper.AddTextComponent(L"SUBMIT", LcDefaults::Black4, L"Calibri", 22, LcFontWeight::Bold);
                widgetHelper.SetTag(2);
                parent->AddChild(child);
            }
        };

        auto onKeysHandler = [](int key, LcKeyState keyEvent, const LcAppContext& context)
        {
            auto parent = context.world->GetObjectByTag<IWidget>(1);
            auto child = context.world->GetObjectByTag<IWidget>(2);

            if (key == 'Q' && keyEvent == LcKeyState::Down) context.app->RequestQuit();
            if (key == 'T' && keyEvent == LcKeyState::Down)
            {
                parent->SetVisible(!parent->IsVisible());
            }
            if (key == 'C' && keyEvent == LcKeyState::Down)
            {
                if (child->GetParent())
                    parent->RemoveChild(child);
                else
                    parent->AddChild(child);
            }
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
