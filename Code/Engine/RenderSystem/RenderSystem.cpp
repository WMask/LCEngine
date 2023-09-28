/**
* RenderSystem.cpp
* 28.01.2023
* (c) Denis Romakhov
*/

#include "pch.h"
#include "RenderSystem/RenderSystem.h"
#include "World/WorldInterface.h"
#include "World/SpriteInterface.h"
#include "GUI/WidgetInterface.h"
#include "GUI/GuiManager.h"
#include "World/Camera.h"
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

    // update visuals
    const auto& visuals = context.world->GetVisuals();
    for (const auto& visual : visuals)
    {
        if (visual->GetTypeId() == LcCreatables::Widget)
        {
            auto widget = static_cast<IWidget*>(visual.get());
            if (HasInvisibleParent(widget)) continue;

            if (visual->IsVisible()) visual->Update(deltaSeconds, context);
        }
        else
        {
            if (visual->IsVisible()) visual->Update(deltaSeconds, context);
        }
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

    const auto& visuals = context.world->GetVisuals();

    for (const auto& visual : visuals)
    {
        if (visual->GetTypeId() == LcCreatables::Widget)
        {
            auto widget = static_cast<IWidget*>(visual.get());
            if (HasInvisibleParent(widget)) continue;

            if (visual->IsVisible()) Render(visual.get(), context);
        }
        else
        {
            if (visual->IsVisible()) Render(visual.get(), context);
        }
    }

    LC_CATCH{ LC_THROW("LcRenderSystemBase::Render()") }
}
