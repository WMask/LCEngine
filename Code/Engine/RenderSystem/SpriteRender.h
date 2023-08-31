/**
* SpriteRender.h
* 28.01.2023
* (c) Denis Romakhov
*/

#pragma once

#include "World/Sprites.h"


/**
* Sprite renderer interface */
class ISpriteRender
{
public:
	/**
	* Setup render state */
	virtual void Setup() = 0;
	/**
	* Render sprite */
	virtual void Render(const ISprite* sprite) = 0;
	/**
	* Checks support for the feature */
	virtual bool Supports(const TVFeaturesList& features) const = 0;

};
