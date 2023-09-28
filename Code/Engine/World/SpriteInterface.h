/**
* SpriteInterface.h
* 10.09.2023
* (c) Denis Romakhov
*/

#pragma once

#include "Module.h"
#include "World/Visual.h"

#include <functional>


/** Sprite tint component */
class ISpriteTintComponent : public IVisualComponent
{
public:
	//
	virtual void SetColor(LcColor4 inTint) = 0;
	// return sprite color data LcColor4[4]
	virtual const void* GetData() const = 0;
};


/** Sprite colors component */
class ISpriteColorsComponent : public IVisualComponent
{
public:
	// return sprite color data LcColor4[4]
	virtual const void* GetData() const = 0;
};


/** Sprite custom UV component */
class ISpriteCustomUVComponent : public IVisualComponent
{
public:
	// return sprite UV data LcVector4[4]
	virtual const void* GetData() const = 0;
};


/** Sprite animation component */
class ISpriteAnimationComponent : public IVisualComponent
{
public:
	// x - frame width, y - frame height, z - offsetX, w - offsetY
	virtual LcVector4 GetAnimData() const = 0;
};


struct LC_TILES_DATA
{
	LcVector3 pos[4];	// position
	LcVector2 uv[4];	// uv coordinates
};

/** Sprite tiled component */
class ITiledSpriteComponent : public IVisualComponent
{
public:
	//
	virtual LcVector2 GetTilesScale() const = 0;
	// tiles vertex data
	virtual const std::vector<LC_TILES_DATA>& GetTilesData() const = 0;
};


/**
* @brief Sprite basic particles component.
* Particles spawned in random places of sprite area and move around start position.
*/
class IBasicParticlesComponent : public IVisualComponent
{
public:
	//
	virtual unsigned short GetNumParticles() const = 0;
	//
	virtual unsigned short GetNumFrames() const = 0;
	// size of each particle frame on texture in pixels
	virtual LcSizef GetFrameSize() const = 0;
	// lifetime in seconds
	virtual float GetParticleLifetime() const = 0;
	// speed multiplier
	virtual float GetParticleSpeed() const = 0;
	// distance around start position
	virtual float GetParticleMovementRadius() const = 0;
};


/**
* Sprite interface */
class ISprite : public IVisualBase
{
public:
	virtual const TVFeaturesList& GetFeaturesList() const = 0;
	//
	static int GetStaticId() { return LcCreatables::Sprite; }


public: // IVisual interface implementation
	//
	virtual int GetTypeId() const override { return LcCreatables::Sprite; }


public:
	ISpriteTintComponent* GetTintComponent() const { return (ISpriteTintComponent*)GetComponent(EVCType::Tint).get(); }
	//
	ISpriteColorsComponent* GetColorsComponent() const { return (ISpriteColorsComponent*)GetComponent(EVCType::VertexColor).get(); }
	//
	ISpriteCustomUVComponent* GetCustomUVComponent() const { return (ISpriteCustomUVComponent*)GetComponent(EVCType::CustomUV).get(); }
	//
	ISpriteAnimationComponent* GetAnimationComponent() const { return (ISpriteAnimationComponent*)GetComponent(EVCType::FrameAnimation).get(); }
	//
	ITiledSpriteComponent* GetTiledComponent() const { return (ITiledSpriteComponent*)GetComponent(EVCType::Tiled).get(); }
	//
	IBasicParticlesComponent* GetParticlesComponent() const { return (IBasicParticlesComponent*)GetComponent(EVCType::Particles).get(); }
	//
	LcVisualTextureComponent* GetTextureComponent() const { return (LcVisualTextureComponent*)GetComponent(EVCType::Texture).get(); }

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

/** Tiled sprite object handler */
typedef std::function<void(
	const std::string&,		// layer name
	const std::string&,		// object name
	const std::string&,		// object type
	const LcObjectProps&,	// object properties
	LcVector2,				// object position
	LcSizef					// object size
)>
LcObjectHandler;


/** Sprite helper */
class WORLD_API LcSpriteHelper
{
public:
	LcSpriteHelper(const LcAppContext& inContext) : context(inContext) {}


public:
	/**
	* Add tint component to the last added sprite */
	void AddTintComponent(LcColor4 tint) const;
	/**
	* Add tint component to the last added sprite */
	void AddTintComponent(LcColor3 tint) const;
	/**
	* Add colors component to the last added sprite */
	void AddColorsComponent(LcColor4 inLeftTop, LcColor4 inRightTop, LcColor4 inRightBottom, LcColor4 inLeftBottom) const;
	/**
	* Add colors component to the last added sprite */
	void AddColorsComponent(LcColor3 inLeftTop, LcColor3 inRightTop, LcColor3 inRightBottom, LcColor3 inLeftBottom) const;
	/**
	* Add texture component to the last added sprite or widget */
	void AddTextureComponent(const std::string& inTexture) const;
	/**
	* Add texture component to the last added sprite or widget */
	void AddTextureComponent(const LcBytes& inData) const;
	/**
	* Add custom UV component to the last added sprite */
	void AddCustomUVComponent(LcVector2 inLeftTop, LcVector2 inRightTop, LcVector2 inRightBottom, LcVector2 inLeftBottom) const;
	/**
	* Add frame animation component to the last added sprite */
	void AddAnimationComponent(LcSizef inFrameSize, unsigned short inNumFrames, float inFramesPerSecond) const;
	/**
	* Add tiled component to the last added sprite */
	void AddTiledComponent(const std::string& tiledJsonPath, const LcLayersList& inLayerNames = LcLayersList{}) const;
	/**
	* Add basic particles component to the last added sprite */
	void AddParticlesComponent(unsigned short inNumParticles, unsigned short inNumFrames, LcSizef inFrameSize,
		float inParticleLifetime, float inParticleSpeed, float inParticleMovementRadius) const;
	/**
	* Add tiled component to the last added sprite */
	void AddTiledComponent(const std::string& tiledJsonPath,
		LcObjectHandler inObjectHandler, const LcLayersList& inLayerNames = LcLayersList{}) const;


protected:
	const LcAppContext& context;

};
