/**
* RenderSystem.cpp
* 28.01.2023
* (c) Denis Romakhov
*/

#include "pch.h"
#include "RenderSystem/RenderSystem.h"
#include "World/WorldInterface.h"
#include "World/Sprites.h"
#include "World/Widgets.h"
#include "World/Camera.h"
#include "Core/LCUtils.h"


void LcRenderSystemBase::LoadShaders(const char* folderPath)
{
    using namespace std::filesystem;

    for (auto& entry : directory_iterator(folderPath))
    {
        if (entry.is_regular_file())
        {
            auto name = entry.path().filename().string();
            auto content = ReadTextFile(entry.path().string().c_str());
            if (!name.empty() && !content.empty())
            {
                shaders[name] = content;
            }
        }
    }
}

void LcRenderSystemBase::Create(TWeakWorld worldPtr, void* windowHandle, bool windowed)
{
    world = worldPtr;
}

void LcRenderSystemBase::Shutdown()
{
}

void LcRenderSystemBase::Update(float deltaSeconds)
{
    if (auto worldPtr = world.lock())
    {
        const auto& sprites = worldPtr->GetSprites();
        const auto& widgets = worldPtr->GetWidgets();

        for (const auto& sprite : sprites)
        {
            if (sprite->IsVisible()) sprite->Update(deltaSeconds);
        }

        for (const auto& widget : widgets)
        {
            if (widget->IsVisible()) widget->Update(deltaSeconds);
        }

        auto newPos = worldPtr->GetCamera().GetPosition();
        auto newTarget = worldPtr->GetCamera().GetTarget();
        if (newPos != cameraPos || newTarget != cameraTarget)
        {
            UpdateCamera(deltaSeconds, newPos, newTarget);

            cameraPos = newPos;
            cameraTarget = newTarget;
        }
    }
}

void LcRenderSystemBase::Render()
{
    if (auto weakWorld = world.lock())
    {
        const auto& sprites = weakWorld->GetSprites();
        const auto& widgets = weakWorld->GetWidgets();

        for (const auto& sprite : sprites)
        {
            if (sprite->IsVisible()) RenderSprite(sprite.get());
        }

        if (!widgets.empty())
        {
            PreRenderWidgets();

            for (const auto& widget : widgets)
            {
                if (widget->IsVisible()) RenderWidget(widget.get());
            }

            PostRenderWidgets();
        }
    }
}
