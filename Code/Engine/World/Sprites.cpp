/**
* Sprites.cpp
* 26.08.2023
* (c) Denis Romakhov
*/

#pragma once

#include "pch.h"
#include "World/Sprites.h"


void LcSpriteAnimationComponent::Update(float deltaSeconds)
{
	double curGameTime = (double)GetTickCount64();
	double frameDelta = curGameTime - lastFrameSeconds;
	float frameLength = 1.0f / framesPerSecond;
	if ((frameDelta / 1000.0) > frameLength)
	{
		lastFrameSeconds = curGameTime;
		curFrame++;
		if (curFrame >= numFrames) curFrame = 0;
	}
}

LcVector4 LcSpriteAnimationComponent::GetAnimData() const
{
	if (auto sprite = (LcSprite*)owner)
	{
		if (auto texComp = sprite->GetTextureComponent())
		{
			auto framesPerRow = unsigned short(texComp->texSize.x / frameSize.x);
			auto column = curFrame % framesPerRow;
			auto row = curFrame / framesPerRow;
			float frameWidth = frameSize.x / texComp->texSize.x;
			float frameHeight = frameSize.y / texComp->texSize.y;

			return LcVector4(
				frameWidth,
				frameHeight,
				frameWidth * column,
				frameHeight * row);
		}
	}

	return LcDefaults::ZeroVec4;
}

void LcSprite::Update(float deltaSeconds)
{
	if (auto anim = GetAnimationComponent())
	{
		anim->Update(deltaSeconds);
	}
}

void LcSprite::AddComponent(TVComponentPtr comp)
{
	IVisualBase::AddComponent(comp);

	features.insert(comp->GetType());
}
