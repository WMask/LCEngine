/**
* HelloWorld.cpp
* 28.01.2023
* (c) Denis Romakhov
*/

#include "framework.h"
#include "HelloWorld.h"
#include "Application/Application.h"
#include "Core/LCUtils.h"


int APIENTRY wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);

    try
    {
        auto App = IApplication::GetPlatformApp();
        TWeakApp WeakApp(App);

        auto OnUpdateHandler = [WeakApp](float DeltaSeconds) {
            DebugMsg("FPS: %.3f\n", (1.0f / DeltaSeconds));
        };

        auto OnKeyboardHandler = [WeakApp](int key, EInputKeyEvent keyEvent) {
            if (auto App = WeakApp.lock())
            {
                if (key == 'Q') App->RequestQuit();
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
