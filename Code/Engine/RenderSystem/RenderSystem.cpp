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

void IRenderSystem::Create(void* Handle, LCSize viewportSize, bool windowed)
{
	LCGUIManager::GetInstance().Init(app.GetUseNoesis());
}

void IRenderSystem::Shutdown()
{
	LCGUIManager::GetInstance().Shutdown();
}

void IRenderSystem::Render()
{
	const auto& sprites = LCWorld::GetInstance().GetSprites();

	for (const auto& sprite : sprites)
	{
		if (sprite.visible) RenderSprite(sprite);
	}
}
