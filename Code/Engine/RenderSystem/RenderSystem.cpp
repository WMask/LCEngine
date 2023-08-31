/**
* RenderSystem.cpp
* 28.01.2023
* (c) Denis Romakhov
*/

#include "pch.h"
#include "RenderSystem/RenderSystem.h"
#include "GUI/GuiManager.h"
#include "World/WorldInterface.h"
#include "World/Sprites.h"
#include "World/Camera.h"
#include "GUI/Widgets.h"
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

void LcRenderSystemBase::Update(float deltaSeconds)
{
    if (auto world = worldPtr.lock())
    {
        const auto& sprites = world->GetSprites();

        for (const auto& sprite : sprites)
        {
            if (sprite->IsVisible()) sprite->Update(deltaSeconds);
        }

        auto newPos = world->GetCamera().GetPosition();
        auto newTarget = world->GetCamera().GetTarget();
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
    if (auto world = worldPtr.lock())
    {
        const auto& sprites = world->GetSprites();
        const auto& widgets = world->GetWidgets();

        for (const auto& sprite : sprites)
        {
            if (sprite->IsVisible()) RenderSprite(sprite.get());
        }

        PreRenderWidgets();

        for (const auto& widget : widgets)
        {
            if (widget->IsVisible()) RenderWidget(widget.get());
        }

        PostRenderWidgets();
    }
}
