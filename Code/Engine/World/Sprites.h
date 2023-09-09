/**
* Sprites.h
* 29.01.2023
* (c) Denis Romakhov
*/

#pragma once

#include "Module.h"
#include "World/Visual.h"

#include <functional>


/**
* Sprite data */
struct LcSpriteData
{
	LcVector3 pos;	// [0,0] - leftBottom, x - right, y - up
	LcSizef size;	// sprite size in pixels
	float rotZ;
	bool visible;
	//
	LcSpriteData() : pos(LcDefaults::ZeroVec3), size(LcDefaults::ZeroSize), rotZ(0.0f), visible(true) {}
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
* Sprite custom UV component */
struct LcSpriteCustomUVComponent : public IVisualComponent
{
	LcVector4 leftTop;
	LcVector4 rightTop;
	LcVector4 rightBottom;
	LcVector4 leftBottom;
	//
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
	const void* GetData() const { return &leftTop; }
	// IVisualComponent interface implementation
	virtual EVCType GetType() const override { return EVCType::CustomUV; }

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


struct LC_TILES_DATA
{
	LcVector3 pos[4];	// position
	LcVector2 uv[4];	// uv coordinates
};

namespace LcTiles
{
	namespace Layers
	{
		static const std::string All = "All";
		static const std::string Default = "Default";
		static const std::string Collision = "Collision";
		static const std::string Objects = "Objects";
	}
	namespace Type
	{
		static const std::string TileLayer = "tilelayer";
		static const std::string ObjectGroup = "objectgroup";
	}
}

typedef std::deque<std::string> LcLayersList;
typedef std::deque<std::pair<std::string, LcAny>> LcObjectProps;
typedef std::function<void(
	const std::string&,		// layer name
	const std::string&,		// object name
	const std::string&,		// object type
	const LcObjectProps&,	// object properties
	LcVector2,				// object position
	LcSizef					// object size
)>
LcObjectHandler;

// void objectHandler(const std::string& layer, const std::string& name, const std::string& type,
//		const LcObjectProps& props, LcVector2 pos, LcSizef size)

/**
* Tiled sprite component */
struct WORLD_API LcTiledSpriteComponent : public IVisualComponent
{
	std::vector<LC_TILES_DATA> tiles;
	std::string tiledJsonPath;
	LcObjectHandler objectHandler;
	LcLayersList layerNames;
	LcVector2 scale;
	//
	LcTiledSpriteComponent() : scale(LcDefaults::OneVec2) {}
	//
	LcTiledSpriteComponent(const LcTiledSpriteComponent& sprite) = default;
	//
	LcTiledSpriteComponent(const std::string& inTiledJsonPath, const LcLayersList& inLayerNames = LcLayersList{}) :
		tiledJsonPath(inTiledJsonPath), layerNames(inLayerNames), scale(LcDefaults::OneVec2) {}
	//
	LcTiledSpriteComponent(const std::string& inTiledJsonPath, LcObjectHandler inObjectHandler,
		const LcLayersList& inLayerNames = LcLayersList{}) : tiledJsonPath(inTiledJsonPath), layerNames(inLayerNames),
		objectHandler(inObjectHandler), scale(LcDefaults::OneVec2) {}


public:// IVisualComponent interface implementation
	//
	virtual void Init(class IWorld& world) override;
	//
	virtual EVCType GetType() const override { return EVCType::Tiled; }

};


/**
* Sprite interface */
class ISprite : public IVisualBase
{
public:
	virtual ~ISprite() {}
	//
	virtual const TVFeaturesList& GetFeaturesList() const = 0;
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
	void AddTextureComponent(const std::string& inTexture)
	{
		AddComponent(std::make_shared<LcVisualTextureComponent>(inTexture));
	}
	//
	void AddTextureComponent(const LcBytes& inData)
	{
		AddComponent(std::make_shared<LcVisualTextureComponent>(inData));
	}
	//
	void AddCustomUVComponent(LcVector2 inLeftTop, LcVector2 inRightTop, LcVector2 inRightBottom, LcVector2 inLeftBottom)
	{
		AddComponent(std::make_shared<LcSpriteCustomUVComponent>(inLeftTop, inRightTop, inRightBottom, inLeftBottom));
	}
	//
	void AddAnimationComponent(LcVector2 inFrameSize, unsigned short inNumFrames, float inFramesPerSecond)
	{
		AddComponent(std::make_shared<LcSpriteAnimationComponent>(inFrameSize, inNumFrames, inFramesPerSecond));
	}
	//
	void AddTiledSpriteComponent(const std::string& tiledJsonPath, const LcLayersList& inLayerNames = LcLayersList{})
	{
		AddComponent(std::make_shared<LcTiledSpriteComponent>(tiledJsonPath, inLayerNames));
	}
	//
	void AddTiledSpriteComponent(const std::string& tiledJsonPath, LcObjectHandler inObjectHandler, const LcLayersList& inLayerNames = LcLayersList{})
	{
		AddComponent(std::make_shared<LcTiledSpriteComponent>(tiledJsonPath, inObjectHandler, inLayerNames));
	}
	//
	LcSpriteTintComponent* GetTintComponent() const { return (LcSpriteTintComponent*)GetComponent(EVCType::Tint).get(); }
	//
	LcSpriteColorsComponent* GetColorsComponent() const { return (LcSpriteColorsComponent*)GetComponent(EVCType::VertexColor).get(); }
	//
	LcVisualTextureComponent* GetTextureComponent() const { return (LcVisualTextureComponent*)GetComponent(EVCType::Texture).get(); }
	//
	LcSpriteCustomUVComponent* GetCustomUVComponent() const { return (LcSpriteCustomUVComponent*)GetComponent(EVCType::CustomUV).get(); }
	//
	LcSpriteAnimationComponent* GetAnimationComponent() const { return (LcSpriteAnimationComponent*)GetComponent(EVCType::FrameAnimation).get(); }
	//
	LcTiledSpriteComponent* GetTiledComponent() const { return (LcTiledSpriteComponent*)GetComponent(EVCType::Tiled).get(); }

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


public:// ISprite interface implementation
	virtual const TVFeaturesList& GetFeaturesList() const override { return features; }


public:// IVisual interface implementation
	virtual void Update(float deltaSeconds);
	//
	virtual void AddComponent(TVComponentPtr comp) override;
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
	virtual void OnMouseMove(LcVector3 pos) override {}
	//
	virtual void OnMouseEnter() override {}
	//
	virtual void OnMouseLeave() override {}


protected:
	TVFeaturesList features;

};
