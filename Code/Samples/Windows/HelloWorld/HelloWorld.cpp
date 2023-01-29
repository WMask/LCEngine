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

        LCFSize size(100, 100);
        Eigen::Vector3f pos(100, 100, 0);
        SPRITE_COLORS colors(LCColor4(1, 0, 0, 1), LCColor4(0, 0, 0, 1), LCColor4(1, 0, 1, 1), LCColor4(0, 1, 0, 1));
        auto Sprite = World.AddSprite(SPRITE_DATA(ESpriteType::Colored, pos, size, colors));

        auto OnUpdateHandler = [WeakApp](float DeltaSeconds) {
            DebugMsg("FPS: %.3f\n", (1.0f / DeltaSeconds));
        };

        auto OnKeyboardHandler = [WeakApp, Sprite](int key, EInputKeyEvent keyEvent) {
            if (auto App = WeakApp.lock())
            {
                if (key == 'Q') App->RequestQuit();

                if (key == VK_LEFT) Sprite->pos.x() -= 10.0f;
                if (key == VK_RIGHT) Sprite->pos.x() += 10.0f;

                if (key == VK_UP) Sprite->rotZ -= 0.1f;
                if (key == VK_DOWN) Sprite->rotZ += 0.1f;
            }
        };

        App->SetUpdateHandler(OnUpdateHandler);
        App->SetKeyboardHandler(OnKeyboardHandler);
        App->SetRenderSystemType(ERenderSystemType::DX10);
        App->Init(hInstance, lpCmdLine);
        App->Run();
    }
    catch (const std::exception& ex)
    {
        DebugMsg("\n# HelloWorld error: %s\n\n", ex.what());
    }

    return 0;
}
