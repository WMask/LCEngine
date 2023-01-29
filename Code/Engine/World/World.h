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
class WORLD_API LCWorld
{
public:
	typedef std::deque<SPRITE_DATA> SPRITE_LIST;


public:
	/**
	* Get singleton */
	static LCWorld& GetInstance();
	/**
	* Add sprite */
	SPRITE_DATA* AddSprite(const SPRITE_DATA& sprite);
	/**
	* Remove sprite */
	void RemoveSprite(SPRITE_DATA* sprite);
	/**
	* Get sprites */
	SPRITE_LIST& GetSprites() { return sprites; }


protected:
	LCWorld();
	LCWorld(const LCWorld&);
	LCWorld& operator=(const LCWorld&);
	~LCWorld();


protected:
	std::deque<SPRITE_DATA> sprites;

};
