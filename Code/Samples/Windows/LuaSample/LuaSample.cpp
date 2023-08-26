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
#include "Core/LCUtils.h"


int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);

    try
    {
        LcLuaScriptSystem lua;

        auto onInitHandler = [&lua](IApplication* app)
        {
            GetApplicationLuaModule(*app).AddTo(lua);
        };

        auto onKeyboardHandler = [&lua](int key, LcKeyState keyEvent, IApplication* app)
        {
            if (key == 'Q' && (keyEvent == LcKeyState::Down))
            {
                lua.RunScript("print(\"Quit request!\"); requestQuit()");
                OutputDebugStringA("\n");
            }
            if (key == 'P' && (keyEvent == LcKeyState::Down))
            {
                // Override luaB_print in lbaselib.c to print in Output window
                const char* script = "print(\"Hello from Lua!\"); return 77.7";
                auto result = lua.RunScriptEx(script);
                OutputDebugStringA("\n");
            }
        };

        auto app = GetApp();
        app->SetInitHandler(onInitHandler);
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
