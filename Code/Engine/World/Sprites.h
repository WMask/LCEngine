/**
* Sprites.h
* 29.01.2023
* (c) Denis Romakhov
*/

#pragma once

#include "Module.h"
#include "Visual.h"

#include <deque>

#pragma warning(disable : 4251)
#pragma warning(disable : 4275)


/**
* Sprite data */
struct LcSpriteData
{
	LcVector3 pos;	// [0,0] - leftTop, x - right, y - down (negative)
	LcSizef size;	// sprite size in pixels
	float rotZ;
	bool visible;
	//
	LcSpriteData() : pos(LcDefaults::ZeroVec3), size(), rotZ(0.0f), visible(true) {}
	//
	LcSpriteData(LcVector3 inPos, LcSizef inSize, float inRotZ = 0.0f, bool inVisible = true)
		: pos(inPos), size(inSize), rotZ(inRotZ), visible(inVisible)
	{
	}
};


/**
* Sprite interface */
class ISprite : public IVisual
{
public:
	ISprite() {}
	~ISprite() {}
};


/**
* Sprite tint component */
struct LcSpriteTintComponent : public IVisualComponent
{
	LcColor4 tint;
	//
	LcColor4 data[4];
	//
	LcSpriteTintComponent(const LcSpriteTintComponent& colors) : tint(colors.tint)
	{
		data[0] = data[1] = data[2] = data[3] = tint;
	}
	//
	LcSpriteTintComponent(LcColor4 inTint) : tint(inTint)
	{
		data[0] = data[1] = data[2] = data[3] = tint;
	}
	//
	const void* GetData() const { return data; }
	// IVisualComponent interface implementation
	virtual EVCType GetType() const override { return EVCType::Tint; }

};


/**
* Sprite colors component */
struct LcSpriteColorsComponent : public IVisualComponent
{
	LcColor4 leftTop;
	LcColor4 rightTop;
	LcColor4 rightBottom;
	LcColor4 leftBottom;
	//
	LcSpriteColorsComponent() : leftTop{}, rightTop{}, rightBottom{}, leftBottom{} {}
	//
	LcSpriteColorsComponent(const LcSpriteColorsComponent& colors) :
		leftTop{ colors.leftTop }, rightTop{ colors.rightTop }, rightBottom{ colors.rightBottom }, leftBottom{ colors.leftBottom }
	{
	}
	//
	LcSpriteColorsComponent(LcColor4 inLeftTop, LcColor4 inRightTop, LcColor4 inRightBottom, LcColor4 inLeftBottom) :
		leftTop(inLeftTop), rightTop(inRightTop), rightBottom(inRightBottom), leftBottom(inLeftBottom)
	{
	}
	const void* GetData() const { return &leftTop; }
	// IVisualComponent interface implementation
	virtual EVCType GetType() const override { return EVCType::VertexColor; }

};


/**
* Sprite texture component */
struct LcSpriteTextureComponent : public IVisualComponent
{
	std::string texture;// texture file path
	LcBytes data;		// texture data
	LcVector2 texPos;	// sprite frame offset
	LcVector2 texSize;	// texture size in pixels
	//
	LcSpriteTextureComponent() : texPos(LcDefaults::ZeroVec2), texSize(LcDefaults::ZeroVec2) {}
	//
	LcSpriteTextureComponent(const LcSpriteTextureComponent& texture) :
		texture(texture.texture), data(texture.data), texPos(texture.texPos), texSize(texture.texSize) {}
	//
	LcSpriteTextureComponent(const std::string& inTexture, LcVector2 inTexPos) :
		texture(inTexture), texPos(inTexPos), texSize(LcDefaults::ZeroVec2)
	{
	}
	//
	LcSpriteTextureComponent(const LcBytes& inData, LcVector2 inTexPos) :
		data(inData), texPos(inTexPos), texSize(LcDefaults::ZeroVec2)
	{
	}
	// IVisualComponent interface implementation
	virtual EVCType GetType() const override { return EVCType::Texture; }

};


/**
* Default Sprite implementation */
class WORLD_API LcSprite : public ISprite
{
public:
	LcSprite(LcSpriteData inSprite) : sprite(inSprite) {}
	//
	~LcSprite() {}
	//
	LcSpriteData sprite;


public: // IVisual interface implementation
	virtual void AddComponent(TVComponentPtr comp) override;
	//
	virtual TVComponentPtr GetComponent(EVCType type) const override;
	//
	virtual bool HasComponent(EVCType type) const override;
	//
	virtual const TVFeaturesList& GetFeaturesList() const override { return features; }
	//
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


protected:
	std::deque<TVComponentPtr> components;
	//
	TVFeaturesList features;

};
