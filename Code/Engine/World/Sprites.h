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
	TexturedColored
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
	std::string texture;
	LcVector2 texPos;
	LcVector2 texSize;
	LcSpriteType type;
	//
	LcSpriteData() : pos(LcDefaults::ZeroVec3), size(), rotZ(0.0f), visible(true), texPos(LcDefaults::ZeroVec2), texSize(LcDefaults::ZeroVec2), type(LcSpriteType::Colored) {}
	//
	LcSpriteData(LcSpriteType inType, LcVector3 inPos, LcSizef inSize, const LcSpriteColors& inColors, float inRotZ = 0.0f, bool inVisible = true)
		: type(inType), pos(inPos), size(inSize), colors(inColors), rotZ(inRotZ), visible(inVisible), texPos(LcDefaults::ZeroVec2), texSize(LcDefaults::ZeroVec2)
	{
	}
	//
	LcSpriteData(LcSpriteType inType, LcVector3 inPos, LcSizef inSize, LcVector2 inTexPos, const std::string& inTexture, float inRotZ = 0.0f, bool inVisible = true)
		: type(inType), pos(inPos), size(inSize), rotZ(inRotZ), visible(inVisible), texture(inTexture), texPos(inTexPos), texSize(LcDefaults::ZeroVec2)
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
	* Sprite texture */
	virtual void SetTexture(const std::string& texture) = 0;
	/**
	* Sprite texture */
	virtual const std::string& GetTexture() const = 0;
	/**
	* Sprite frame pos */
	virtual void SetTexPos(LcVector2 texPos) = 0;
	/**
	* Sprite frame pos */
	virtual LcVector2 GetTexPos() const = 0;
	/**
	* Texture size */
	virtual LcVector2 GetTexSize() const = 0;
	/**
	* Sprite type */
	virtual LcSpriteType GetType() const = 0;

};


/**
* Default Sprite implementation */
class LcSprite : public ISprite
{
public:
	LcSprite(LcSpriteData inSprite) : sprite(inSprite) {}
	//
	LcSpriteData sprite;


public: // ISprite interface implementation
	virtual ~LcSprite() override {}
	//
	virtual void SetColors(const LcSpriteColors& inColors) override { sprite.colors = inColors; }
	//
	virtual const LcSpriteColors& GetColors() const override { return sprite.colors; }
	//
	virtual void SetTexture(const std::string& inTexture) override { sprite.texture = inTexture; }
	//
	virtual const std::string& GetTexture() const override { return sprite.texture; }
	//
	virtual void SetTexPos(LcVector2 inTexPos) override { sprite.texPos = inTexPos; }
	//
	virtual LcVector2 GetTexPos() const override { return sprite.texPos; }
	//
	virtual LcVector2 GetTexSize() const override { return sprite.texSize; }
	//
	virtual LcSpriteType GetType() const override { return sprite.type; }


public: // IVisual interface implementation
	virtual void SetSize(LcSizef inSize) override { sprite.size = inSize; }
	//
	virtual LcSizef GetSize() const override { return sprite.size; }
	//
	virtual void SetPos(LcVector3 inPos) override { sprite.pos = inPos; }
	//
	virtual void AddPos(LcVector3 inPos) override { sprite.pos = sprite.pos + inPos; }
	//
	virtual LcVector3 GetPos() const override { return sprite.pos; }
	//
	virtual void SetRotZ(float inRotZ) override { sprite.rotZ = inRotZ; }
	//
	virtual void AddRotZ(float inRotZ) override { sprite.rotZ += inRotZ; }
	//
	virtual float GetRotZ() const override { return sprite.rotZ; }
	//
	virtual void SetVisible(bool inVisible) override { sprite.visible = inVisible; }
	//
	virtual bool IsVisible() const override { return sprite.visible; }
	//
	virtual void OnMouseButton(LcMouseBtn btn, LcKeyState state, int x, int y) {}
	//
	virtual void OnMouseMove(int x, int y) {}

};
