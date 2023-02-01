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
enum class LcSpriteType
{
	Colored,
	Textured
};


/**
* Sprite colors */
struct LcSpriteColors
{
	LcColor4 leftTop;
	LcColor4 rightTop;
	LcColor4 leftBottom;
	LcColor4 rightBottom;
	//
	LcSpriteColors() {}
	//
	LcSpriteColors(LcColor4 inLeftTop, LcColor4 inRightTop, LcColor4 inLeftBottom, LcColor4 inRightBottom)
		: leftTop(inLeftTop), rightTop(inRightTop), leftBottom(inLeftBottom), rightBottom(inRightBottom)
	{
	}
	//
	LcSpriteColors(LcColor4 tint) : leftTop(tint), rightTop(tint), leftBottom(tint), rightBottom(tint)
	{
	}
};


/**
* Sprite data */
struct LcSpriteData
{
	LcSpriteColors colors;
	LcVector3 pos;
	LcSizef size;
	float rotZ;
	bool visible;
	LcSpriteType type;
	//
	LcSpriteData() : rotZ(0.0f), visible(true) {}
	//
	LcSpriteData(LcSpriteType inType, LcVector3 inPos, LcSizef inSize, const LcSpriteColors& inColors, float inRotZ = 0.0f, bool inVisible = true)
		: type(inType), pos(inPos), size(inSize), colors(inColors), rotZ(inRotZ), visible(inVisible)
	{
	}
};
