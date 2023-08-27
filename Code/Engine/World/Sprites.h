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
	LcVector3 pos;	// [0,0] - leftBottom, x - right, y - up
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
* Sprite tint component */
struct LcSpriteTintComponent : public IVisualComponent
{
	LcColor4 tint;
	//
	LcColor4 data[4];
	//
	LcSpriteTintComponent(const LcSpriteTintComponent& colors) : tint(colors.tint)
	{
		SetColor(tint);
	}
	//
	LcSpriteTintComponent(LcColor4 inTint) : tint(inTint)
	{
		SetColor(tint);
	}
	//
	LcSpriteTintComponent(LcColor3 inTint) : tint(inTint.x, inTint.y, inTint.z, 1.0f)
	{
		SetColor(tint);
	}
	//
	void SetColor(LcColor4 inTint) { data[0] = data[1] = data[2] = data[3] = inTint; }
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
	//
	LcSpriteColorsComponent(LcColor3 inLeftTop, LcColor3 inRightTop, LcColor3 inRightBottom, LcColor3 inLeftBottom) :
		leftTop(To4(inLeftTop)), rightTop(To4(inRightTop)), rightBottom(To4(inRightBottom)), leftBottom(To4(inLeftBottom))
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
	std::string texture;	// texture file path
	LcBytes data;			// texture data
	LcVector2 framePos;		// sprite frame offset in pixels
	LcVector2 texSize;		// texture size in pixels
	//
	LcSpriteTextureComponent() : framePos(LcDefaults::ZeroVec2), texSize(LcDefaults::ZeroVec2) {}
	//
	LcSpriteTextureComponent(const LcSpriteTextureComponent& texture) :
		texture(texture.texture), data(texture.data), framePos(texture.framePos), texSize(texture.texSize) {}
	//
	LcSpriteTextureComponent(const std::string& inTexture, LcVector2 inFramePos) :
		texture(inTexture), framePos(inFramePos), texSize(LcDefaults::ZeroVec2)
	{
	}
	//
	LcSpriteTextureComponent(const LcBytes& inData, LcVector2 inFramePos) :
		data(inData), framePos(inFramePos), texSize(LcDefaults::ZeroVec2)
	{
	}
	// IVisualComponent interface implementation
	virtual EVCType GetType() const override { return EVCType::Texture; }

};


/**
* Sprite texture component */
struct WORLD_API LcSpriteAnimationComponent : public IVisualComponent
{
	LcVector2 frameSize;		// sprite frame offset in pixels
	unsigned short numFrames;	// frames count
	unsigned short curFrame;	// current frame index
	float framesPerSecond;		// frames per second
	double lastFrameSeconds;	// last game time
	//
	LcSpriteAnimationComponent() :
		frameSize(LcDefaults::ZeroVec2), numFrames(0), curFrame(0), framesPerSecond(0.0f), lastFrameSeconds(0.0f) {}
	//
	LcSpriteAnimationComponent(const LcSpriteAnimationComponent& anim) :
		frameSize(anim.frameSize), numFrames(anim.numFrames), curFrame(anim.curFrame),
		framesPerSecond(anim.framesPerSecond), lastFrameSeconds(anim.lastFrameSeconds) {}
	//
	LcSpriteAnimationComponent(LcVector2 inFrameSize, unsigned short inNumFrames, float inFramesPerSecond) :
		frameSize(inFrameSize), numFrames(inNumFrames), curFrame(0), framesPerSecond(inFramesPerSecond), lastFrameSeconds(0.0f)
	{
	}
	// x - frame width, y - frame height, z - offsetX, w - offsetY
	LcVector4 GetAnimData() const;


public: // IVisualComponent interface implementation
	virtual void Update(float deltaSeconds) override;
	virtual EVCType GetType() const override { return EVCType::FrameAnimation; }

};


/**
* Sprite interface */
class ISprite : public IVisual
{
public:
	ISprite() {}
	//
	~ISprite() {}
	//
	void AddTintComponent(LcColor4 tint)
	{
		AddComponent(std::make_shared<LcSpriteTintComponent>(tint));
	}
	//
	void AddTintComponent(LcColor3 tint)
	{
		AddComponent(std::make_shared<LcSpriteTintComponent>(tint));
	}
	//
	void AddColorsComponent(LcColor4 inLeftTop, LcColor4 inRightTop, LcColor4 inRightBottom, LcColor4 inLeftBottom)
	{
		AddComponent(std::make_shared<LcSpriteColorsComponent>(inLeftTop, inRightTop, inRightBottom, inLeftBottom));
	}
	//
	void AddColorsComponent(LcColor3 inLeftTop, LcColor3 inRightTop, LcColor3 inRightBottom, LcColor3 inLeftBottom)
	{
		AddComponent(std::make_shared<LcSpriteColorsComponent>(inLeftTop, inRightTop, inRightBottom, inLeftBottom));
	}
	//
	void AddTextureComponent(const std::string& inTexture, LcVector2 inTexPos = LcDefaults::ZeroVec2)
	{
		AddComponent(std::make_shared<LcSpriteTextureComponent>(inTexture, inTexPos));
	}
	//
	void AddTextureComponent(const LcBytes& inData, LcVector2 inTexPos = LcDefaults::ZeroVec2)
	{
		AddComponent(std::make_shared<LcSpriteTextureComponent>(inData, inTexPos));
	}
	//
	void AddAnimationComponent(LcVector2 inFrameSize, unsigned short inNumFrames, float inFramesPerSecond)
	{
		AddComponent(std::make_shared<LcSpriteAnimationComponent>(inFrameSize, inNumFrames, inFramesPerSecond));
	}
	//
	LcSpriteTintComponent* GetTintComponent() const { return (LcSpriteTintComponent*)GetComponent(EVCType::Tint).get(); }
	//
	LcSpriteColorsComponent* GetColorsComponent() const { return (LcSpriteColorsComponent*)GetComponent(EVCType::VertexColor).get(); }
	//
	LcSpriteTextureComponent* GetTextureComponent() const { return (LcSpriteTextureComponent*)GetComponent(EVCType::Texture).get(); }
	//
	LcSpriteAnimationComponent* GetAnimationComponent() const { return (LcSpriteAnimationComponent*)GetComponent(EVCType::FrameAnimation).get(); }

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
	virtual void Update(float deltaSeconds);
	//
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
