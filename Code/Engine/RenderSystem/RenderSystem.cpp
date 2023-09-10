/**
* RenderSystem.cpp
* 28.01.2023
* (c) Denis Romakhov
*/

#include "pch.h"
#include "RenderSystem/RenderSystem.h"
#include "RenderSystem/WidgetRender.h"
#include "GUI/GuiManager.h"
#include "World/WorldInterface.h"
#include "World/Sprites.h"
#include "World/Camera.h"
#include "GUI/Widgets.h"
#include "Core/LCUtils.h"
#include "Core/LCException.h"


void LcRenderSystemBase::LoadShaders(const char* folderPath)
{
    using namespace std::filesystem;

    LC_TRY

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

    LC_CATCH{ LC_THROW("LcRenderSystemBase::LoadShaders()") }
}

void LcRenderSystemBase::Create(TWeakWorld world, void* windowHandle, LcWinMode mode, bool inVSync)
{
    worldPtr = world;
    vSync = inVSync;
}

void LcRenderSystemBase::Update(float deltaSeconds)
{
    LC_TRY

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

    LC_CATCH{ LC_THROW("LcRenderSystemBase::Update()") }
}

void LcRenderSystemBase::Render()
{
    LC_TRY

    if (auto world = worldPtr.lock())
    {
        const auto& sprites = world->GetSprites();
        const auto& widgets = world->GetWidgets();

        for (const auto& sprite : sprites)
        {
            if (sprite->IsVisible()) RenderSprite(sprite.get());
        }

        PreRenderWidgets(EWRMode::Textures);

        for (const auto& widget : widgets)
        {
            if (widget->IsVisible()) RenderWidget(widget.get());
        }

        PostRenderWidgets(EWRMode::Textures);

        PreRenderWidgets(EWRMode::Text);

        for (const auto& widget : widgets)
        {
            if (widget->IsVisible()) RenderWidget(widget.get());
        }

        PostRenderWidgets(EWRMode::Text);
    }

    LC_CATCH{ LC_THROW("LcRenderSystemBase::Render()") }
}
