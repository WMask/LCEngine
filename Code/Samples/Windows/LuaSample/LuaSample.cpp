/**
* LuaSample.cpp
* 24.08.2023
* (c) Denis Romakhov
*/

#include "framework.h"
#include "LuaSample.h"
#include "Application/Application.h"
#include "Application/Windows/Module.h"
#include "Lua/ApplicationLuaModule.h"
#include "Lua/LuaScriptSystem.h"
#include "Lua/ApplicationLuaModule.h"
#include "Core/LCUtils.h"


int APIENTRY wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);

    try
    {
        auto app = GetApp();
        TWeakApp weakApp(app);

        LcLuaScriptSystem lua;
        GetApplicationLuaModule(app)->Add(lua);

        auto onKeyboardHandler = [weakApp, &lua](int key, LcKeyState keyEvent) {
            if (auto app = weakApp.lock())
            {
                if (key == 'Q') app->RequestQuit();
                if (key == 'L' && (keyEvent == LcKeyState::Down))
                {
                    // Override luaB_print in lbaselib.c to print in Output window
                    const char* script = "print(\"Hello from Lua!\"); requestQuit(); return 77.7";
                    auto result = lua.RunScriptEx(script); 
                    OutputDebugStringA("\n");
                }
            }
        };

        app->SetKeyboardHandler(onKeyboardHandler);
        app->SetWindowSize(800, 600);
        app->Init(hInstance);
        app->Run();
    }
    catch (const std::exception& ex)
    {
        DebugMsg("\n# LuaSample error: %s\n\n", ex.what());
    }

    return 0;
}
