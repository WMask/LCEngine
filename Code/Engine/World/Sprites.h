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
	LcColor4 leftTop;
	LcColor4 rightTop;
	LcColor4 leftBottom;
	LcColor4 rightBottom;
	//
	SPRITE_COLORS() {}
	//
	SPRITE_COLORS(LcColor4 inLeftTop, LcColor4 inRightTop, LcColor4 inLeftBottom, LcColor4 inRightBottom)
		: leftTop(inLeftTop), rightTop(inRightTop), leftBottom(inLeftBottom), rightBottom(inRightBottom)
	{
	}
	//
	SPRITE_COLORS(LcColor4 tint) : leftTop(tint), rightTop(tint), leftBottom(tint), rightBottom(tint)
	{
	}
};


/**
* Sprite data */
struct SPRITE_DATA
{
	SPRITE_COLORS colors;
	LcVector3 pos;
	LcSizef size;
	float rotZ;
	bool visible;
	ESpriteType type;
	//
	SPRITE_DATA() : rotZ(0.0f), visible(true) {}
	//
	SPRITE_DATA(ESpriteType inType, LcVector3 inPos, LcSizef inSize, const SPRITE_COLORS& inColors, float inRotZ = 0.0f, bool inVisible = true)
		: type(inType), pos(inPos), size(inSize), colors(inColors), rotZ(inRotZ), visible(inVisible)
	{
	}
};
