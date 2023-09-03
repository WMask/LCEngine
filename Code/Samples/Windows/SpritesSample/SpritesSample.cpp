/**
* SpritesSample.cpp
* 27.08.2023
* (c) Denis Romakhov
*/

#include "framework.h"
#include "SpritesSample.h"
#include "Application/AppConfig.h"
#include "Application/Application.h"
#include "Application/Windows/Module.h"
#include "RenderSystem/RenderSystemDX10/Module.h"
#include "World/WorldInterface.h"
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

            if (auto sprite1 = world->AddSprite(150, 400, 200, 200))
            {
                sprite1->AddColorsComponent(LcColor3(1, 0, 0), LcColor3(1, 0, 1), LcColor3(0, 0, 0), LcColor3(0, 1, 0));
            }

            if (auto sprite2 = world->AddSprite(150, 150, 200, 200))
            {
                sprite2->AddTintComponent(LcColor3(0.7f, 0.7f, 0.7f));
            }

            if (auto sprite3 = world->AddSprite(450, 200, 300, 300))
            {
                sprite3->AddTextureComponent("../../Assets/tree.png");
                sprite3->AddColorsComponent(LcColor3(0, 1, 0), LcColor3(0, 1, 0), LcColor3(1, 0, 0), LcColor3(1, 0, 0));
            }

            if (auto sprite4 = world->AddSprite(380, 85, 100, 100))
            {
                sprite4->AddTextureComponent("../../Assets/anim.png");
                sprite4->AddAnimationComponent(LcSizef(128, 128), 10, 12);
            }

            if (auto sprite5 = world->AddSprite(700, 400, 200, 200))
            {
                sprite5->AddTiledSpriteComponent("../../Assets/Map1.tmj");
            }
        };

        KEYS keys;
        auto onUpdateHandler = [&keys](float deltaSeconds, IApplication* app)
        {
            DebugMsg("FPS: %.3f\n", (1.0f / deltaSeconds));

            auto sprite1 = app->GetWorld()->GetSprites()[0];
            auto sprite2 = app->GetWorld()->GetSprites()[1];

            // move sprite
            if (keys[VK_LEFT]) sprite1->AddPos(LcVector3(-200 * deltaSeconds, 0, 0));
            if (keys[VK_RIGHT]) sprite1->AddPos(LcVector3(200 * deltaSeconds, 0, 0));

            if (keys[VK_UP]) sprite1->AddRotZ(-2 * deltaSeconds);
            if (keys[VK_DOWN]) sprite1->AddRotZ(2 * deltaSeconds);

            // change tint
            auto value = sin(double(GetTickCount64()) / 1000.0);
            auto tint = float(abs(value));
            sprite2->GetTintComponent()->SetColor(LcColor4(1.0f - tint, tint, 0.0f, 1.0f));
        };

        auto onKeyboardHandler = [&keys](int key, LcKeyState keyEvent, IApplication* app)
        {
            keys[key] = (keyEvent == LcKeyState::Down) ? 1 : 0;

            if (key == 'Q') app->RequestQuit();
        };

        auto app = GetApp();
        app->SetRenderSystem(GetRenderSystem());
        app->SetInitHandler(onInitHandler);
        app->SetUpdateHandler(onUpdateHandler);
        app->SetKeyboardHandler(onKeyboardHandler);
        app->SetWindowSize(1024, 768);
        app->Init(hInstance);
        app->Run();
    }
    catch (const std::exception& ex)
    {
        DebugMsg("\n# SpritesSample error: %s\n\n", ex.what());
    }

    return 0;
}
