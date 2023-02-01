/**
* World.h
* 29.01.2023
* (c) Denis Romakhov
*/

#pragma once


#include "WorldModule.h"
#include "Sprites.h"

#include <deque>

#pragma warning(disable : 4251)


/**
* Game world */
class WORLD_API LcWorld
{
public:
	typedef std::deque<LcSpriteData> SPRITE_LIST;


public:
	/**
	* Get singleton */
	static LcWorld& GetInstance();
	/**
	* Add sprite */
	LcSpriteData* AddSprite(const LcSpriteData& sprite);
	/**
	* Remove sprite */
	void RemoveSprite(LcSpriteData* sprite);
	/**
	* Get sprites */
	SPRITE_LIST& GetSprites() { return sprites; }


protected:
	LcWorld();
	LcWorld(const LcWorld&);
	LcWorld& operator=(const LcWorld&);
	~LcWorld();


protected:
	std::deque<LcSpriteData> sprites;

};
