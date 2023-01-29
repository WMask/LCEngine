/**
* SpriteRender.h
* 28.01.2023
* (c) Denis Romakhov
*/

#pragma once


/**
* Render system type */
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
	//
	SPRITE_DATA() : rotZ(0.0f) {}
	//
	SPRITE_DATA(Eigen::Vector3f inPos, LCFSize inSize, const SPRITE_COLORS& inColors, float inRotZ = 0.0f)
		: pos(inPos), size(inSize), colors(inColors), rotZ(inRotZ)
	{
	}
};


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
	virtual void Render(const SPRITE_DATA& sprite) = 0;
	/**
	* Return sprite type */
	virtual ESpriteType GetType() const = 0;

};
