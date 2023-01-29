/**
* RenderSystem.cpp
* 28.01.2023
* (c) Denis Romakhov
*/

#include "pch.h"
#include "RenderSystem/RenderSystem.h"
#include "World/World.h"


IRenderSystem::~IRenderSystem()
{
}

void IRenderSystem::Render()
{
	const auto& sprites = LCWorld::GetInstance().GetSprites();

	for (const auto& sprite : sprites)
	{
		if (sprite.visible) RenderSprite(sprite);
	}
}
