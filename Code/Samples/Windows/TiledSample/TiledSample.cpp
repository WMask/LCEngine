/**
* TiledSample.cpp
* 06.09.2023
* (c) Denis Romakhov
*/

#include "framework.h"
#include "TiledSample.h"
#include "Application/Windows/Module.h"
#include "RenderSystem/RenderSystemDX10/Module.h"
#include "World/SpriteInterface.h"
#include "World/WorldInterface.h"
#include "Box2D/Module.h"
#include "Core/LCUtils.h"


int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);

    try
    {
        auto onInitHandler = [](IApplication* app)
        {
            auto physWorld = app->GetPhysicsWorld();
            auto world = app->GetWorld();

            auto& spriteHelper = world->GetSpriteHelper();
            if (world->AddSprite(512, 384, LcLayers::Z1, 1024, 768))
            {
                auto objectHandler = [physWorld](const std::string& layer, const std::string& name,
                    const std::string& type, const LcObjectProps& props, LcVector2 pos, LcSizef size)
                {
                    if (layer == LcTiles::Layers::Collision)
                    {
                        physWorld->AddStaticBox(pos, size);
                    }
                };
                spriteHelper.AddTiledComponent("../../Assets/Map1.tmj", objectHandler);
            }

            if (auto hero = world->AddSprite2D(100, 600, 64, 64))
            {
                spriteHelper.AddTextureComponent("../../Assets/anim.png");
                spriteHelper.AddAnimationComponent(LcSizef(128, 128), 10, 12);

                auto body = physWorld->AddDynamicBox(LcVector2(100, 600), LcSizef(35, 45), 5);
                body->SetUserData(hero);
            }
        };

        auto onUpdateHandler = [](float deltaSeconds, IApplication* app)
        {
            DebugMsg("FPS: %.3f\n", (1.0f / deltaSeconds));

            auto physWorld = app->GetPhysicsWorld();
            physWorld->Update(deltaSeconds);

            auto body = physWorld->GetDynamicBodies()[0];
            auto vel = body->GetVelocity();

            if (auto hero = body->GetUserObject<ISprite>())
            {
                hero->SetPos(To3(body->GetPos()));
            }

            auto& keys = app->GetInputSystem()->GetState();
            if (keys[VK_LEFT]) body->SetVelocity(LcVector2(-1.2f, vel.y));
            if (keys[VK_RIGHT]) body->SetVelocity(LcVector2(1.2f, vel.y));
        };

        auto onKeysHandler = [](int key, LcKeyState keyEvent, IApplication* app)
        {
            auto physWorld = app->GetPhysicsWorld();
            auto body = physWorld->GetDynamicBodies()[0];

            if (key == 'Q') app->RequestQuit();

            bool canJump = !body->IsFalling() && (abs(body->GetVelocity().y) <= 0.1f);
            if (key == VK_UP && (keyEvent == LcKeyState::Down) && canJump)
            {
                body->ApplyImpulse(LcVector2(0.0f, -4.0f));
            }
        };

        auto app = GetApp();
        app->SetRenderSystem(GetRenderSystem());
        app->SetPhysicsWorld(GetPhysicsWorld());
        app->SetInitHandler(onInitHandler);
        app->SetUpdateHandler(onUpdateHandler);
        app->GetInputSystem()->SetKeysHandler(onKeysHandler);
        app->SetWindowSize(1024, 768);
        app->Init(hInstance);
        app->Run();
    }
    catch (const std::exception& ex)
    {
        DebugMsg("\n# TiledSample error: %s\n\n", ex.what());
    }

    return 0;
}
