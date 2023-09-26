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
#include "World/SpriteInterface.h"
#include "World/Camera.h"
#include "GUI/WidgetInterface.h"
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

void LcRenderSystemBase::Create(void* windowHandle, LcWinMode mode, bool inVSync, bool inAllowFullscreen, const LcAppContext& context)
{
    vSync = inVSync;
    allowFullscreen = inAllowFullscreen;
}

void LcRenderSystemBase::Update(float deltaSeconds, const LcAppContext& context)
{
    LC_TRY

    // widgets updated in LcGuiManager

    // update sprites
    const auto& sprites = context.world->GetSprites();

    for (const auto& sprite : sprites)
    {
        if (sprite->IsVisible()) sprite->Update(deltaSeconds, context);
    }

    // update camera
    auto newPos = context.world->GetCamera().GetPosition();
    auto newTarget = context.world->GetCamera().GetTarget();
    if (newPos != cameraPos || newTarget != cameraTarget)
    {
        UpdateCamera(deltaSeconds, newPos, newTarget);

        cameraPos = newPos;
        cameraTarget = newTarget;
    }

    LC_CATCH{ LC_THROW("LcRenderSystemBase::Update()") }
}

void LcRenderSystemBase::Render(const LcAppContext& context)
{
    LC_TRY

    const auto& sprites = context.world->GetSprites();
    const auto& widgets = context.world->GetWidgets();

    for (const auto& widget : widgets)
    {
        if (widget->IsVisible()) RenderWidget(widget.get(), context);
    }

    for (const auto& sprite : sprites)
    {
        if (sprite->IsVisible()) RenderSprite(sprite.get(), context);
    }

    LC_CATCH{ LC_THROW("LcRenderSystemBase::Render()") }
}
