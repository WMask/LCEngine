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
	Textured,
	ColoredTextured
};


/**
* Sprite colors */
struct LcSpriteColors
{
	LcColor4 leftTop;
	LcColor4 rightTop;
	LcColor4 rightBottom;
	LcColor4 leftBottom;
	//
	LcSpriteColors() : leftTop{}, rightTop{}, rightBottom{}, leftBottom{} {}
	//
	LcSpriteColors(const LcSpriteColors& colors) :
		leftTop{ colors.leftTop }, rightTop{ colors.rightTop }, rightBottom{ colors.rightBottom }, leftBottom{ colors.leftBottom }
	{
	}
	//
	LcSpriteColors(LcColor4 inLeftTop, LcColor4 inRightTop, LcColor4 inRightBottom, LcColor4 inLeftBottom) :
		leftTop(inLeftTop), rightTop(inRightTop), rightBottom(inRightBottom), leftBottom(inLeftBottom)
	{
	}
	//
	LcSpriteColors(LcColor4 tint) : leftTop(tint), rightTop(tint), rightBottom(tint), leftBottom(tint)
	{
	}
	//
	LcSpriteColors& operator=(const LcSpriteColors& colors)
	{
		leftTop = colors.leftTop;
		rightTop = colors.rightTop;
		rightBottom = colors.rightBottom;
		leftBottom = colors.leftBottom;
		return *this;
	}
};


/**
* Sprite data */
struct LcSpriteData
{
	LcSpriteColors colors;
	LcVector3 pos; // [0,0] - leftTop, x - right, y - down (negative)
	LcSizef size;
	float rotZ;
	bool visible;
	std::string texPath;
	LcVector2 texPos;
	LcVector2 texSize;
	LcBytes texData;
	LcSpriteType type;
	//
	LcSpriteData() : pos(LcDefaults::ZeroVec3), size(), rotZ(0.0f), visible(true), texPos(LcDefaults::ZeroVec2), texSize(LcDefaults::ZeroVec2), type(LcSpriteType::Colored) {}
	//
	LcSpriteData(LcSpriteType inType, LcVector3 inPos, LcSizef inSize, const LcSpriteColors& inColors, float inRotZ = 0.0f, bool inVisible = true)
		: type(inType), pos(inPos), size(inSize), colors(inColors), rotZ(inRotZ), visible(inVisible), texPos(LcDefaults::ZeroVec2), texSize(LcDefaults::ZeroVec2)
	{
	}
	//
	LcSpriteData(LcSpriteType inType, LcVector3 inPos, LcSizef inSize, LcVector2 inTexPos, const std::string& inTexPath, float inRotZ = 0.0f, bool inVisible = true)
		: type(inType), pos(inPos), size(inSize), rotZ(inRotZ), visible(inVisible), texPath(inTexPath), texPos(inTexPos), texSize(LcDefaults::ZeroVec2)
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
	virtual void SetColors(const LcSpriteColors& colors) = 0;
	/**
	* Sprite colors */
	virtual const LcSpriteColors& GetColors() const = 0;
	/**
	* Sprite type */
	virtual LcSpriteType GetType() const = 0;

};
