/**
* HelloWorld.cpp
* 28.01.2023
* (c) Denis Romakhov
*/

#include "framework.h"
#include "HelloWorld.h"
#include "Application/Windows/Module.h"
#include "RenderSystem/RenderSystemDX10/Module.h"
#include "Core/LCUtils.h"
#include "World/World.h"


int APIENTRY wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);

    try
    {
        auto& world = LcWorld::GetInstance();
        auto app = GetApp();
        TWeakApp weakApp(app);

        LcSizef size(200, 200);
        LcVector3 pos(200, 200, 0);
        LcSpriteColors colors(LcColor4(1, 0, 0, 1), LcColor4(0, 0, 0, 1), LcColor4(1, 0, 1, 1), LcColor4(0, 1, 0, 1));
        auto sprite = world.AddSprite(LcSpriteData(LcSpriteType::Colored, pos, size, colors));

        BYTE keys[256];
        memset(keys, 0, sizeof(keys));

        auto onUpdateHandler = [weakApp, sprite, &keys](float DeltaSeconds) {
            DebugMsg("FPS: %.3f\n", (1.0f / DeltaSeconds));

            if (auto app = weakApp.lock())
            {
                if (keys[VK_LEFT]) sprite->pos.x() -= 200.0f * DeltaSeconds;
                if (keys[VK_RIGHT]) sprite->pos.x() += 200.0f * DeltaSeconds;

                if (keys[VK_UP]) sprite->rotZ -= 2.0f * DeltaSeconds;
                if (keys[VK_DOWN]) sprite->rotZ += 2.0f * DeltaSeconds;
            }
        };

        auto onKeyboardHandler = [weakApp, &keys](int key, LcKeyState keyEvent) {
            keys[key] = (keyEvent == LcKeyState::Down) ? 1 : 0;

            if (auto app = weakApp.lock())
            {
                if (key == 'Q') app->RequestQuit();
            }
        };

        auto render = GetRenderSystem();
        render->LoadShaders("../../../Shaders/HLSL/");
        app->SetRenderSystem(render);
        app->SetUpdateHandler(onUpdateHandler);
        app->SetKeyboardHandler(onKeyboardHandler);
        app->SetWindowSize(LcSize(1024, 768));
        app->Init(hInstance, lpCmdLine);
        app->Run();
    }
    catch (const std::exception& ex)
    {
        DebugMsg("\n# HelloWorld error: %s\n\n", ex.what());
    }

    return 0;
}
