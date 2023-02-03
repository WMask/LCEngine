/**
* WorldInterface.h
* 03.02.2023
* (c) Denis Romakhov
*/

#pragma once

#include "Module.h"
#include "Sprites.h"

#include <deque>


/**
* Game world interface */
class WORLD_API IWorld
{
public:
	typedef std::deque<LcSpriteData> SPRITE_LIST;


public:
	/**
	* Add sprite */
	virtual LcSpriteData* AddSprite(const LcSpriteData& sprite) = 0;
	/**
	* Remove sprite */
	virtual void RemoveSprite(LcSpriteData* sprite) = 0;
	/**
	* Get sprites */
	virtual SPRITE_LIST& GetSprites() = 0;

};
