/**
* SpriteRender.h
* 28.01.2023
* (c) Denis Romakhov
*/

#pragma once

#include "World/Sprites.h"


/**
* Sprite render interface */
class ISpriteRender
{
public:
	/**
	* Setup render state */
	virtual void Setup() = 0;
	/**
	* Render sprite */
	virtual void Render(const LcSpriteData& sprite) = 0;
	/**
	* Return sprite type */
	virtual LcSpriteType GetType() const = 0;

};
