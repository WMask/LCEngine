LC Engine
====================================

Welcome to the LCEngine source code!

**Languages:**
- C++17
- HLSL
- Lua

**Features:**
- DirectX 10 render system
- DirectWrite text rendering
- DirectInput gamepads support
- XAudio2 sound system
- Lua script system
- Box2D physics
- JSON file parsing
- Tiled 2D maps support

**Installation**
----------------
**Windows**

Prerequisites: Visual Studio 2022

1. Download LCEngine from GitHub.
2. Put Lua 5.4.6 sources and libs to **Code/Engine/Lua/src**.
3. Put nlohmann's Json sources to **Code/Engine/Json**.
4. Put Box2D source to **Code/Engine/Box2D**. Build it. Remove all Box2D files and folders except of: **build** and **include**.
5. Open **Projects/Windows/LCEngine.sln**. Build project.
6. Windows samples are in **Code/Samples/Windows** folder.

**Hello World**
---------------
Windows samples are in **Code/Samples/Windows** folder.
```
#include "Application/Windows/Module.h"
#include "RenderSystem/RenderSystemDX10/Module.h"
#include "World/Spriteinterface.h"
#include "World/WorldInterface.h"

int WinMain(...)
{
    auto onInitHandler = [](IApplication* app)
    {
        if (app->GetWorld()->AddSprite2D(250, 400, 200, 200))
        {
            app->GetWorld()->GetSpriteHelper().AddTintComponent(LcColor3(0, 1, 0));
        }
    };
  
    KEYS keys;
    auto onUpdateHandler = [&keys](float deltaSeconds, IApplication* app)
    {
        auto sprite = app->GetWorld()->GetSprites()[0];
        if (keys[VK_LEFT]) sprite->AddPos(LcVector3(-200 * deltaSeconds, 0, 0));
        if (keys[VK_RIGHT]) sprite->AddPos(LcVector3(200 * deltaSeconds, 0, 0));
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
  
    return 0;
}
```
