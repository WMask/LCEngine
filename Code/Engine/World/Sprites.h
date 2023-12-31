/**
* Sprites.h
* 29.01.2023
* (c) Denis Romakhov
*/

#pragma once

#include "Module.h"
#include "SpriteInterface.h"
#include "World/WorldInterface.h"


class LcSpriteCustomUVComponent : public ISpriteCustomUVComponent
{
public:
	LcSpriteCustomUVComponent() : leftTop{}, rightTop{}, rightBottom{}, leftBottom{} {}
	//
	LcSpriteCustomUVComponent(const LcSpriteCustomUVComponent& colors) :
		leftTop{ colors.leftTop }, rightTop{ colors.rightTop }, rightBottom{ colors.rightBottom }, leftBottom{ colors.leftBottom }
	{
	}
	//
	LcSpriteCustomUVComponent(LcVector2 inLeftTop, LcVector2 inRightTop, LcVector2 inRightBottom, LcVector2 inLeftBottom) :
		leftTop(To4(inLeftTop)), rightTop(To4(inRightTop)), rightBottom(To4(inRightBottom)), leftBottom(To4(inLeftBottom))
	{
	}


public: // ISpriteCustomUVComponent interface implementation
	//
	virtual const void* GetData() const override { return &leftTop; }


public: // IVisualComponent interface implementation
	//
	virtual EVCType GetType() const override { return LcComponents::CustomUV; }


protected:
	LcVector4 leftTop;
	LcVector4 rightTop;
	LcVector4 rightBottom;
	LcVector4 leftBottom;
};


class WORLD_API LcSpriteAnimationComponent : public ISpriteAnimationComponent
{
public:
	LcSpriteAnimationComponent() :
		frameSize(LcDefaults::ZeroVec2), numFrames(0), curFrame(0), framesPerSecond(0.0f), lastFrameSeconds(0.0f) {}
	//
	LcSpriteAnimationComponent(const LcSpriteAnimationComponent& anim) :
		frameSize(anim.frameSize), numFrames(anim.numFrames), curFrame(anim.curFrame),
		framesPerSecond(anim.framesPerSecond), lastFrameSeconds(anim.lastFrameSeconds) {}
	//
	LcSpriteAnimationComponent(LcSizef inFrameSize, unsigned short inNumFrames, float inFramesPerSecond) :
		frameSize(inFrameSize), numFrames(inNumFrames), curFrame(0), framesPerSecond(inFramesPerSecond), lastFrameSeconds(0.0f)
	{
	}


public:
	// x - frame width, y - frame height, z - offsetX, w - offsetY
	virtual LcVector4 GetAnimData() const override;


public: // IVisualComponent interface implementation
	//
	virtual void Update(float deltaSeconds, const LcAppContext& context) override;
	//
	virtual EVCType GetType() const override { return LcComponents::FrameAnimation; }


protected:
	LcSizef frameSize;			// sprite frame offset in pixels
	unsigned short numFrames;	// frames count
	unsigned short curFrame;	// current frame index
	float framesPerSecond;		// frames per second
	double lastFrameSeconds;	// last game time
};


class WORLD_API LcTiledSpriteComponent : public ITiledSpriteComponent
{
public:
	//
	LcTiledSpriteComponent() : scale(LcDefaults::OneVec2) {}
	//
	LcTiledSpriteComponent(const LcTiledSpriteComponent& sprite) = default;
	//
	LcTiledSpriteComponent(const std::string& inTiledJsonPath, const LcLayersList& inLayerNames = LcLayersList{}) :
		tiledJsonPath(inTiledJsonPath), layerNames(inLayerNames), scale(LcDefaults::OneVec2) {}
	//
	LcTiledSpriteComponent(const std::string& inTiledJsonPath, LcTiledObjectHandler inObjectHandler,
		const LcLayersList& inLayerNames = LcLayersList{}) : tiledJsonPath(inTiledJsonPath), layerNames(inLayerNames),
		objectHandler(inObjectHandler), scale(LcDefaults::OneVec2) {}


public: // ITiledSpriteComponent interface implementation
	//
	virtual LcVector2 GetTilesScale() const override { return scale; }
	//
	virtual const std::vector<LC_TILES_DATA>& GetTilesData() const override { return tiles; }


public: // IVisualComponent interface implementation
	//
	virtual void Init(const LcAppContext& context) override;
	//
	virtual EVCType GetType() const override { return LcComponents::Tiled; }


protected:
	std::vector<LC_TILES_DATA> tiles;
	std::string tiledJsonPath;
	LcTiledObjectHandler objectHandler;
	LcLayersList layerNames;
	LcVector2 scale;
};


class LcBasicParticlesComponent : public IBasicParticlesComponent
{
public:
	//
	LcBasicParticlesComponent() = default;
	//
	LcBasicParticlesComponent(const LcBasicParticlesComponent& sprite) = default;
	//
	LcBasicParticlesComponent(unsigned short inNumParticles, const LcBasicParticleSettings& inSettings) :
		numParticles(inNumParticles), settings(inSettings) {}


public: // IBasicParticlesComponent interface implementation
	//
	virtual unsigned short GetNumParticles() const override { return numParticles; }
	//
	virtual const LcBasicParticleSettings& GetSettings() const override { return settings; }


public: // IVisualComponent interface implementation
	//
	virtual EVCType GetType() const override { return LcComponents::Particles; }


protected:
	LcBasicParticleSettings settings;
	//
	unsigned short numParticles;
};


/**
* Default Sprite implementation */
class WORLD_API LcSprite : public ISprite
{
public:
	LcSprite() : pos(LcDefaults::ZeroVec3), size(LcDefaults::ZeroSize), rotZ(0.0f), visible(true) {}
	//
	~LcSprite() {}


public: // ISprite interface implementation
	//
	virtual const TVFeaturesList& GetFeaturesList() const override { return features; }


public: // IVisual interface implementation
	//
	virtual void AddComponent(TVComponentPtr comp, const LcAppContext& context) override;
	//
	virtual void SetSize(LcSizef inSize) override { size = inSize; }
	//
	virtual LcSizef GetSize() const override { return size; }
	//
	virtual void SetPos(LcVector3 inPos) override { pos = inPos; }
	//
	virtual void SetPos(LcVector2 inPos) override { pos = LcVector3{ inPos.x, inPos.y, pos.z }; }
	//
	virtual void AddPos(LcVector3 inPos) override { pos = pos + inPos; }
	//
	virtual LcVector3 GetPos() const override { return pos; }
	//
	virtual void SetRotZ(float inRotZ) override { rotZ = inRotZ; }
	//
	virtual void AddRotZ(float inRotZ) override { rotZ += inRotZ; }
	//
	virtual float GetRotZ() const override { return rotZ; }
	//
	virtual void SetVisible(bool inVisible) override { visible = inVisible; }
	//
	virtual bool IsVisible() const override { return visible; }
	//
	virtual void OnMouseButton(int btn, LcKeyState state, int x, int y, const LcAppContext& context) override {}
	//
	virtual void OnMouseMove(LcVector3 pos, const LcAppContext& context) override {}
	//
	virtual void OnMouseEnter(const LcAppContext& context) override {}
	//
	virtual void OnMouseLeave(const LcAppContext& context) override {}


protected:
	TVFeaturesList features;
	// [0,0] - leftBottom, x - right, y - up
	LcVector3 pos;
	// sprite size in pixels
	LcSizef size;
	// sprite rotation in degrees
	float rotZ;
	//
	bool visible;

};
