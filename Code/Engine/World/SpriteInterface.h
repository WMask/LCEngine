/**
* SpriteInterface.h
* 10.09.2023
* (c) Denis Romakhov
*/

#pragma once

#include "Module.h"
#include "World/Visual.h"

#include <functional>


struct LcSpriteTintComponent;
struct LcSpriteColorsComponent;
struct LcSpriteCustomUVComponent;
struct LcSpriteAnimationComponent;
struct LcTiledSpriteComponent;


/**
* Sprite interface */
class ISprite : public IVisualBase
{
public:
	virtual const TVFeaturesList& GetFeaturesList() const = 0;


public:
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
	void AddAnimationComponent(LcVector2 inFrameSize, unsigned short inNumFrames, float inFramesPerSecond) const;
	/**
	* Add tiled component to the last added sprite */
	void AddTiledComponent(const std::string& tiledJsonPath, const LcLayersList& inLayerNames = LcLayersList{}) const;
	/**
	* Add tiled component to the last added sprite */
	void AddTiledComponent(const std::string& tiledJsonPath,
		LcObjectHandler inObjectHandler, const LcLayersList& inLayerNames = LcLayersList{}) const;


protected:
	const LcAppContext& context;

};
