/**
* RenderSystem.cpp
* 28.01.2023
* (c) Denis Romakhov
*/

#include "pch.h"
#include "RenderSystem/RenderSystem.h"
#include "Application/Application.h"
#include "GUI/GUIManager.h"
#include "World/World.h"


IRenderSystem::IRenderSystem(IApplication& inApp) : app(inApp)
{
}

IRenderSystem::~IRenderSystem()
{
}

void IRenderSystem::Create(void* Handle, LcSize viewportSize, bool windowed)
{
	LcGUIManager::GetInstance().Init(viewportSize, app.GetUseNoesis());
}

void IRenderSystem::Shutdown()
{
	LcGUIManager::GetInstance().Shutdown();
}

void IRenderSystem::Render()
{
	const auto& sprites = LcWorld::GetInstance().GetSprites();

	for (const auto& sprite : sprites)
	{
		if (sprite.visible) RenderSprite(sprite);
	}
}
