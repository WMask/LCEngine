/**
* RenderSystem.cpp
* 28.01.2023
* (c) Denis Romakhov
*/

#include "pch.h"
#include "RenderSystem/RenderSystem.h"
#include "World/Module.h"
#include "Core/LCUtils.h"


IRenderSystem::~IRenderSystem()
{
}

void IRenderSystem::LoadShaders(const char* folderPath)
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

void IRenderSystem::Create(void* Handle, LcSize viewportSize, bool windowed)
{
}

void IRenderSystem::Shutdown()
{
}

void IRenderSystem::Render()
{
	const auto& sprites = GetWorld()->GetSprites();

	for (const auto& sprite : sprites)
	{
		if (sprite->IsVisible()) RenderSprite(sprite.get());
	}
}
