/**
* World.h
* 29.01.2023
* (c) Denis Romakhov
*/

#pragma once

#include "Module.h"

#pragma warning(disable : 4251)


/**
* Game world manager */
class WORLD_API LcWorld : public IWorld
{
public: // IWorld interface implementation
	/**
	* Add sprite */
	virtual LcSpriteData* AddSprite(const LcSpriteData& sprite) override;
	/**
	* Remove sprite */
	virtual void RemoveSprite(LcSpriteData* sprite) override;
	/**
	* Get sprites */
	virtual SPRITE_LIST& GetSprites() override { return sprites; }


protected:
	LcWorld();
	~LcWorld();
	LcWorld(const LcWorld&);
	LcWorld& operator=(const LcWorld&);
	friend WORLD_API IWorld& GetWorld();


protected:
	std::deque<LcSpriteData> sprites;

};
