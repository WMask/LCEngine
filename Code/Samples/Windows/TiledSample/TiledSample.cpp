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
        auto onInitHandler = [](const LcAppContext& context)
        {
            auto physWorld = context.physics;

            auto& spriteHelper = context.world->GetSpriteHelper();
            if (context.world->AddSprite(512, 384, LcLayers::Z1, 1024, 768))
            {
                auto objectHandler = [physWorld](const std::string& layer, const std::string& name,
                    const std::string& type, const LcTiledProps& props, LcVector2 pos, LcSizef size)
                {
                    if (layer == LcTiles::Layers::Collision)
                    {
                        physWorld->AddStaticBox(pos, size);
                    }
                };
                spriteHelper.AddTiledComponent("../../Assets/Map1.tmj", objectHandler);
            }

            if (auto hero = context.world->AddSprite(100, 600, 64, 64))
            {
                spriteHelper.AddTextureComponent("../../Assets/anim.png");
                spriteHelper.AddAnimationComponent(LcSizef{ 128, 128 }, 10, 12);

                auto body = physWorld->AddDynamicBox(LcVector2{ 100, 600 }, LcSizef{ 35, 45 }, 5);
                body->SetUserData(hero);
            }
        };

        auto onUpdateHandler = [](float deltaSeconds, const LcAppContext& context)
        {
            DebugMsg("FPS: %.3f\n", (1.0f / deltaSeconds));

            auto body = context.physics->GetDynamicBodies()[0];
            if (auto hero = body->GetUserObject<ISprite>())
            {
                hero->SetPos(body->GetPos());
            }

            auto vel = body->GetVelocity();
            const auto& actions = context.input->GetActiveInputDevice();
            if (actions->Pressed("Left")) body->SetVelocity(LcVector2{ -1.2f, vel.y });
            if (actions->Pressed("Right")) body->SetVelocity(LcVector2{ 1.2f, vel.y });
        };

        auto onActionHandler = [](const LcAction& action, const LcAppContext& context)
        {
            auto body = context.physics->GetDynamicBodies()[0];

            if (action.Pressed("Quit")) context.app->RequestQuit();

            bool canJump = !body->IsFalling() && (abs(body->GetVelocity().y) <= 0.1f);
            if (action.Pressed("Up") && canJump)
            {
                body->ApplyImpulse(LcVector2{ 0.0f, -4.0f });
            }
        };

        LcAppConfig cfg;
        LoadConfig(cfg, "../../Assets/Config.txt");

        auto app = GetApp();
        app->SetConfig(cfg);
        app->SetRenderSystem(GetRenderSystem());
        app->SetPhysicsWorld(GetPhysicsWorld());
        app->SetInitHandler(onInitHandler);
        app->SetUpdateHandler(onUpdateHandler);
        app->GetInputSystem()->SetActionHandler(onActionHandler);
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
