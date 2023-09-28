/**
* HelloWorld.cpp
* 28.01.2023
* (c) Denis Romakhov
*/

#include "framework.h"
#include "HelloWorld.h"
#include "Application/Windows/Module.h"
#include "RenderSystem/RenderSystemDX10/Module.h"
#include "World/SpriteInterface.h"
#include "World/WorldInterface.h"
#include "Core/LCUtils.h"


int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);

    try
    {
        auto onInitHandler = [](const LcAppContext& context)
        {
            auto& spriteHelper = context.world->GetSpriteHelper();

            if (auto sprite = context.world->AddSprite(250, 400, 200, 200))
            {
                spriteHelper.AddColorsComponent(LcColor3(1, 0, 0), LcColor3(1, 0, 1), LcColor3(0, 0, 0), LcColor3(0, 1, 0));
                sprite->SetTag(1);
            }
        };

        auto onUpdateHandler = [](float deltaSeconds, const LcAppContext& context)
        {
            DebugMsg("FPS: %.3f\n", (1.0f / deltaSeconds));

            auto sprite = context.world->GetObjectByTag<ISprite>(1);

            // move sprite
            auto& keys = context.input->GetActiveInputDevice()->GetState();
            if (keys[VK_LEFT] || keys[LcJKeys::Left]) sprite->AddPos(LcVector3(-200 * deltaSeconds, 0, 0));
            if (keys[VK_RIGHT] || keys[LcJKeys::Right]) sprite->AddPos(LcVector3(200 * deltaSeconds, 0, 0));

            if (keys[VK_UP] || keys[LcJKeys::Up]) sprite->AddRotZ(-2 * deltaSeconds);
            if (keys[VK_DOWN] || keys[LcJKeys::Down]) sprite->AddRotZ(2 * deltaSeconds);
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
        DebugMsg("\n# HelloWorld error: %s\n\n", ex.what());
    }

    return 0;
}
