/**
* HelloWorld.cpp
* 28.01.2023
* (c) Denis Romakhov
*/

#include "framework.h"
#include "HelloWorld.h"
#include "Application/Application.h"
#include "Core/LCUtils.h"
#include "World/World.h"


int APIENTRY wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);

    try
    {
        auto& World = LCWorld::GetInstance();
        auto App = IApplication::GetPlatformApp();
        TWeakApp WeakApp(App);

        LCFSize size(200, 200);
        Eigen::Vector3f pos(200, 200, 0);
        SPRITE_COLORS colors(LCColor4(1, 0, 0, 1), LCColor4(0, 0, 0, 1), LCColor4(1, 0, 1, 1), LCColor4(0, 1, 0, 1));
        auto Sprite = World.AddSprite(SPRITE_DATA(ESpriteType::Colored, pos, size, colors));

        BYTE keys[256];
        memset(keys, 0, sizeof(keys));

        auto OnUpdateHandler = [WeakApp, Sprite, &keys](float DeltaSeconds) {
            DebugMsg("FPS: %.3f\n", (1.0f / DeltaSeconds));

            if (auto App = WeakApp.lock())
            {
                if (keys[VK_LEFT]) Sprite->pos.x() -= 200.0f * DeltaSeconds;
                if (keys[VK_RIGHT]) Sprite->pos.x() += 200.0f * DeltaSeconds;

                if (keys[VK_UP]) Sprite->rotZ -= 2.0f * DeltaSeconds;
                if (keys[VK_DOWN]) Sprite->rotZ += 2.0f * DeltaSeconds;
            }
        };

        auto OnKeyboardHandler = [WeakApp, &keys](int key, EInputKeyEvent keyEvent) {
            keys[key] = (keyEvent == EInputKeyEvent::Down) ? 1 : 0;

            if (auto App = WeakApp.lock())
            {
                if (key == 'Q') App->RequestQuit();
            }
        };

        App->SetUpdateHandler(OnUpdateHandler);
        App->SetKeyboardHandler(OnKeyboardHandler);
        App->SetRenderSystemType(ERenderSystemType::DX10);
        App->SetWindowSize(LCSize(1024, 768));
        App->LoadShaders("../../../Shaders/HLSL/");
        App->Init(hInstance, lpCmdLine);
        App->Run();
    }
    catch (const std::exception& ex)
    {
        DebugMsg("\n# HelloWorld error: %s\n\n", ex.what());
    }

    return 0;
}
