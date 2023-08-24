/**
* Sprites.h
* 29.01.2023
* (c) Denis Romakhov
*/

#pragma once

#include "Visual.h"


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
	LcSpriteColors() : leftTop{}, rightTop{}, leftBottom{}, rightBottom{} {}
	//
	LcSpriteColors(const LcSpriteColors& colors) :
		leftTop{ colors.leftTop }, rightTop{ colors.rightTop }, leftBottom{ colors.leftBottom }, rightBottom{ colors.rightBottom }
	{
	}
	//
	LcSpriteColors(LcColor4 inLeftTop, LcColor4 inRightTop, LcColor4 inLeftBottom, LcColor4 inRightBottom) :
		leftTop(inLeftTop), rightTop(inRightTop), leftBottom(inLeftBottom), rightBottom(inRightBottom)
	{
	}
	//
	LcSpriteColors(LcColor4 tint) : leftTop(tint), rightTop(tint), leftBottom(tint), rightBottom(tint)
	{
	}
	//
	LcSpriteColors& operator=(const LcSpriteColors& colors)
	{
		leftTop = colors.leftTop;
		leftTop = colors.leftTop;
		leftTop = colors.leftTop;
		leftTop = colors.leftTop;
		return *this;
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
	LcSpriteData() : pos(LcDefaults::ZeroVec3), size(), rotZ(0.0f), visible(true), type(LcSpriteType::Colored) {}
	//
	LcSpriteData(LcSpriteType inType, LcVector3 inPos, LcSizef inSize, const LcSpriteColors& inColors, float inRotZ = 0.0f, bool inVisible = true)
		: type(inType), pos(inPos), size(inSize), colors(inColors), rotZ(inRotZ), visible(inVisible)
	{
	}
};


/**
* Sprite interface */
class ISprite : public IVisual
{
public:
	/**
	* Sprite colors */
	virtual void SetColors(LcSpriteColors colors) = 0;
	/**
	* Sprite colors */
	virtual LcSpriteColors GetColors() const = 0;
	/**
	* Sprite type */
	virtual LcSpriteType GetType() const = 0;

};
