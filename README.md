LC Engine
====================================

Welcome to the LCEngine source code!

**Features:**
- DirectX 10 render system
- DirectWrite text rendering
- DirectInput joysticks support
- XAudio2 sound system
- Ogg / Vorbis audio streaming
- Lua script system
- Box2D physics
- JSON file parsing
- Tiled 2D maps support

**Installation**
----------------
**Windows**

Prerequisites: **Visual Studio 2022**

1. Download **LCEngine** from GitHub.
2. Put **Lua** 5.4.6 sources and libs to **Code/Engine/Lua/src**.
3. Put **Box2D** source to **Code/Engine/Box2D/src**. Build it.
4. Put **nlohmann**'s Json sources to **Code/Engine/Json**.
5. Put **libogg** library to **Code/Engine/Audio/libogg**. Build it.
6. Put **libvorbis** library to **Code/Engine/Audio/libvorbis**. Build it.
7. Open **Projects/Windows/LCEngine.sln**. Build all projects multiple times to create all libs.
8. Select HelloWorld project as startup. Start.

**Hello World**
---------------
Windows samples are in **Code/Samples/Windows** folder.
```
#include "Application/Windows/Module.h"
#include "RenderSystem/RenderSystemDX10/Module.h"
#include "World/SpriteInterface.h"
#include "World/WorldInterface.h"

int WinMain(...)
{
	auto onInitHandler = [](IApplication* app)
	{
		auto& spriteHelper = app->GetWorld()->GetSpriteHelper();

		if (app->GetWorld()->AddSprite2D(250, 400, 200, 200))
		{
			spriteHelper.AddColorsComponent(LcColor3(1, 0, 0), LcColor3(1, 0, 1), LcColor3(0, 0, 0), LcColor3(0, 1, 0));
		}
	};

	auto onUpdateHandler = [](float deltaSeconds, IApplication* app)
	{
		auto sprite = app->GetWorld()->GetSprites()[0];
		auto& keys = app->GetInputSystem()->GetActiveInputDevice()->GetState();

		if (keys[VK_LEFT] || keys[LcJKeys::Left]) sprite->AddPos(LcVector3(-200 * deltaSeconds, 0, 0));
		if (keys[VK_RIGHT] || keys[LcJKeys::Right]) sprite->AddPos(LcVector3(200 * deltaSeconds, 0, 0));

		if (keys[VK_UP] || keys[LcJKeys::Up]) sprite->AddRotZ(-2 * deltaSeconds);
		if (keys[VK_DOWN] || keys[LcJKeys::Down]) sprite->AddRotZ(2 * deltaSeconds);
	};

	auto onKeysHandler = [](int key, LcKeyState keyEvent, IApplication* app)
	{
		if (key == 'Q' || key == LcJKeys::Menu) app->RequestQuit();
	};

	auto app = GetApp();
	app->SetRenderSystem(GetRenderSystem());
	app->SetInitHandler(onInitHandler);
	app->SetUpdateHandler(onUpdateHandler);
	app->GetInputSystem()->SetKeysHandler(onKeysHandler);
	app->SetWindowSize(1024, 768);
	app->Init(hInstance);
	app->Run();
  
	return 0;
}
```
