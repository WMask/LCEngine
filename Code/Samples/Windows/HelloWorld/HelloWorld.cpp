/**
* HelloWorld.cpp
* 28.01.2023
* (c) Denis Romakhov
*/

#include "framework.h"
#include "HelloWorld.h"
#include "Application/AppConfig.h"
#include "Application/Application.h"
#include "Application/Windows/Module.h"
#include "RenderSystem/RenderSystemDX10/Module.h"
#include "World/WorldInterface.h"
#include "World/Sprites.h"
#include "Core/LCUtils.h"


int APIENTRY wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);

    try
    {
        auto onInitHandler = [](IApplication* app)
        {
            if (auto sprite1 = app->GetWorld()->AddSprite(LcSpriteData(LcVector3{ 200, -200, 0 }, LcSizef{ 200, 200 })))
            {
                auto colorsComponent = std::make_shared<LcSpriteColorsComponent>(
                    LcColor4(1, 0, 0, 1), LcColor4(1, 0, 1, 1), LcColor4(0, 0, 0, 1), LcColor4(0, 1, 0, 1));
                sprite1->AddComponent(colorsComponent);
            }

            if (auto sprite2 = app->GetWorld()->AddSprite(LcSpriteData(LcVector3{ 200, -500, 0 }, LcSizef{ 200, 200 })))
            {
                sprite2->AddComponent(std::make_shared<LcSpriteTintComponent>(LcColor4(0.7, 0.7, 0.7, 1)));
            }
        };

        KEYS keys;
        auto onUpdateHandler = [&keys](float deltaSeconds, IApplication* app)
        {
            DebugMsg("FPS: %.3f\n", (1.0f / deltaSeconds));

            auto sprite = app->GetWorld()->GetSprites()[0];

            if (keys[VK_LEFT]) sprite->AddPos(LcVector3(-200 * deltaSeconds, 0, 0));
            if (keys[VK_RIGHT]) sprite->AddPos(LcVector3(200 * deltaSeconds, 0, 0));

            if (keys[VK_UP]) sprite->AddRotZ(-2 * deltaSeconds);
            if (keys[VK_DOWN]) sprite->AddRotZ(2 * deltaSeconds);
        };

        auto onKeyboardHandler = [&keys](int key, LcKeyState keyEvent, IApplication* app)
        {
            keys[key] = (keyEvent == LcKeyState::Down) ? 1 : 0;

            if (key == 'Q') app->RequestQuit();
        };

        auto cfg = LoadConfig();
        int winWidth = cfg["appWinWidth"].iValue;
        int winHeight = cfg["appWinHeight"].iValue;

        auto app = GetApp();
        auto world = GetWorld();
        app->SetRenderSystem(GetRenderSystem());
        app->SetInitHandler(onInitHandler);
        app->SetUpdateHandler(onUpdateHandler);
        app->SetKeyboardHandler(onKeyboardHandler);
        app->SetWindowSize(winWidth, winHeight);
        app->Init(hInstance, world);
        app->Run();
    }
    catch (const std::exception& ex)
    {
        DebugMsg("\n# HelloWorld error: %s\n\n", ex.what());
    }

    return 0;
}
