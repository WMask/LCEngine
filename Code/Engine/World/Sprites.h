/**
* Sprites.h
* 29.01.2023
* (c) Denis Romakhov
*/

#pragma once

#include "WorldModule.h"
#include "Core/LCTypes.h"


/**
* Sprite type */
enum class ESpriteType
{
	Colored,
	Textured
};


/**
* Sprite colors */
struct SPRITE_COLORS
{
	LCColor4 leftTop;
	LCColor4 rightTop;
	LCColor4 leftBottom;
	LCColor4 rightBottom;
	//
	SPRITE_COLORS() {}
	//
	SPRITE_COLORS(LCColor4 inLeftTop, LCColor4 inRightTop, LCColor4 inLeftBottom, LCColor4 inRightBottom)
		: leftTop(inLeftTop), rightTop(inRightTop), leftBottom(inLeftBottom), rightBottom(inRightBottom)
	{
	}
	//
	SPRITE_COLORS(LCColor4 tint) : leftTop(tint), rightTop(tint), leftBottom(tint), rightBottom(tint)
	{
	}
};


/**
* Sprite data */
struct SPRITE_DATA
{
	SPRITE_COLORS colors;
	Eigen::Vector3f pos;
	LCFSize size;
	float rotZ;
	bool visible;
	ESpriteType type;
	//
	SPRITE_DATA() : rotZ(0.0f), visible(true) {}
	//
	SPRITE_DATA(ESpriteType inType, Eigen::Vector3f inPos, LCFSize inSize, const SPRITE_COLORS& inColors, float inRotZ = 0.0f, bool inVisible = true)
		: type(inType), pos(inPos), size(inSize), colors(inColors), rotZ(inRotZ), visible(inVisible)
	{
	}
};
