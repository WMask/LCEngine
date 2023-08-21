/**
* RenderSystem.cpp
* 28.01.2023
* (c) Denis Romakhov
*/

#include "pch.h"
#include "RenderSystem/RenderSystem.h"
#include "World/WorldInterface.h"
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

void LcRenderSystemBase::Create(TWorldWeakPtr worldPtr, void* windowHandle, LcSize viewportSize, bool windowed)
{
    world = worldPtr;
}

void LcRenderSystemBase::Shutdown()
{
}

void LcRenderSystemBase::Render()
{
    if (auto weakWorld = world.lock())
    {
        const auto& sprites = weakWorld->GetSprites();

        for (const auto& sprite : sprites)
        {
            if (sprite->IsVisible()) RenderSprite(sprite.get());
        }
    }
}
