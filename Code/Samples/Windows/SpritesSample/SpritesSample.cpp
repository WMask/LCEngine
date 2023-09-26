/**
* SpritesSample.cpp
* 27.08.2023
* (c) Denis Romakhov
*/

#include "framework.h"
#include "SpritesSample.h"
#include "Application/Windows/Module.h"
#include "RenderSystem/RenderSystemDX10/Module.h"
#include "World/WorldInterface.h"
#include "World/SpriteInterface.h"
#include "Core/LCUtils.h"


int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);

    try
    {
        auto onInitHandler = [](const LcAppContext& context)
        {
            auto& spriteHelper = context.world->GetSpriteHelper();

            if (context.world->AddSprite2D(200, 550, 250, 250))
            {
                spriteHelper.AddColorsComponent(LcColor3(1, 0, 0), LcColor3(1, 0, 1), LcColor3(0, 0, 0), LcColor3(0, 1, 0));
            }

            if (context.world->AddSprite2D(200, 200, 300, 300))
            {
                spriteHelper.AddTintComponent(LcColor3(0.7f, 0.7f, 0.7f));
            }

            if (context.world->AddSprite(550, 200, LcLayers::Z1, 300, 300))
            {
                spriteHelper.AddTextureComponent("../../Assets/tree.png");
                spriteHelper.AddColorsComponent(LcColor3(0, 1, 0), LcColor3(0, 1, 0), LcColor3(1, 0, 0), LcColor3(1, 0, 0));
            }

            if (context.world->AddSprite2D(460, 315, 100, 100))
            {
                spriteHelper.AddTextureComponent("../../Assets/anim.png");
                spriteHelper.AddAnimationComponent(LcSizef(128, 128), 10, 12);
            }
        };

        auto onUpdateHandler = [](float deltaSeconds, const LcAppContext& context)
        {
            DebugMsg("FPS: %.3f\n", (1.0f / deltaSeconds));

            auto sprite1 = context.world->GetSprites()[0];
            auto sprite2 = context.world->GetSprites()[1];

            // change tint
            auto value = sin(double(GetTickCount64()) / 1000.0);
            auto tint = float(abs(value));
            sprite2->GetTintComponent()->SetColor(LcColor4(1.0f - tint, tint, 0.0f, 1.0f));

            // move sprite
            auto& keys = context.input->GetActiveInputDevice()->GetState();
            if (keys[VK_LEFT] || keys[LcJKeys::Left]) sprite1->AddPos(LcVector3(-200 * deltaSeconds, 0, 0));
            if (keys[VK_RIGHT] || keys[LcJKeys::Right]) sprite1->AddPos(LcVector3(200 * deltaSeconds, 0, 0));

            if (keys[VK_UP] || keys[LcJKeys::Up]) sprite1->AddRotZ(-2 * deltaSeconds);
            if (keys[VK_DOWN] || keys[LcJKeys::Down]) sprite1->AddRotZ(2 * deltaSeconds);
        };

        auto onKeysHandler = [](int key, LcKeyState keyEvent, const LcAppContext& context)
        {
            if (key == 'Q' || key == LcJKeys::Menu) context.app->RequestQuit();
        };

        auto app = GetApp();
        app->SetRenderSystem(GetRenderSystem());
        app->SetInitHandler(onInitHandler);
        app->SetUpdateHandler(onUpdateHandler);
        app->GetInputSystem()->SetKeysHandler(onKeysHandler);
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
