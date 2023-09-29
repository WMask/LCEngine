/**
* SpriteInterface.h
* 10.09.2023
* (c) Denis Romakhov
*/

#pragma once

#include "Module.h"
#include "World/Visual.h"

#include <functional>


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


/** Particle settings */
struct LcBasicParticleSettings
{
	LcBasicParticleSettings()
		: frameSize(LcDefaults::ZeroSize)
		, numFrames(0)
		, lifetime(-1.0f)
		, fadeInRate(-1.0f)
		, fadeOutRate(-1.0f)
		, speed(1.0f)
		, movementRadius(16.0f)
	{}
	// size of each particle frame on texture in pixels
	LcSizef frameSize;
	// frames count on texture
	unsigned short numFrames;
	// lifetime in seconds (-1.0 == forever)
	float lifetime;
	// fadeIn = particleLifetime * fadeInRate (-1.0 == no fade in)
	float fadeInRate;
	// fadeOut = particleLifetime * fadeOutRate (-1.0 == no fade out)
	float fadeOutRate;
	// speed multiplier
	float speed;
	// distance around start position in pixels
	float movementRadius;
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
	virtual const LcBasicParticleSettings& GetSettings() const = 0;

};


/**
* Sprite interface */
class ISprite : public IVisualBase
{
public: // IVisual interface implementation
	//
	virtual int GetTypeId() const override { return LcCreatables::Sprite; }


public:
	//
	static int GetStaticId() { return LcCreatables::Sprite; }
	//
	virtual const TVFeaturesList& GetFeaturesList() const = 0;


public:
	//
	ISpriteCustomUVComponent* GetCustomUVComponent() const { return (ISpriteCustomUVComponent*)GetComponent(EVCType::CustomUV).get(); }
	//
	ISpriteAnimationComponent* GetAnimationComponent() const { return (ISpriteAnimationComponent*)GetComponent(EVCType::FrameAnimation).get(); }
	//
	ITiledSpriteComponent* GetTiledComponent() const { return (ITiledSpriteComponent*)GetComponent(EVCType::Tiled).get(); }
	//
	IBasicParticlesComponent* GetParticlesComponent() const { return (IBasicParticlesComponent*)GetComponent(EVCType::Particles).get(); }

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
typedef std::deque<std::pair<std::string, LcAny>> LcTiledProps;

/** Tiled sprite object handler */
typedef std::function<void(
	const std::string&,		// layer name
	const std::string&,		// object name
	const std::string&,		// object type
	const LcTiledProps&,	// object properties
	LcVector2,				// object position
	LcSizef					// object size
)>
LcTiledObjectHandler;


/** Sprite helper */
class WORLD_API LcSpriteHelper : public LcVisualHelper
{
public:
	LcSpriteHelper(const LcAppContext& inContext) : LcVisualHelper(inContext) {}


public:
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
	* Add tiled component to the last added sprite */
	void AddTiledComponent(const std::string& tiledJsonPath,
		LcTiledObjectHandler inObjectHandler, const LcLayersList& inLayerNames = LcLayersList{}) const;
	/**
	* Add basic particles component to the last added sprite */
	void AddParticlesComponent(unsigned short inNumParticles, const LcBasicParticleSettings& inSettings) const;
};
