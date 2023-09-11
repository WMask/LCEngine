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


/**
* Sprite interface */
class WORLD_API ISprite : public IVisualBase
{
public:
	virtual ~ISprite() {}
	//
	virtual const TVFeaturesList& GetFeaturesList() const = 0;
	//
	void AddTintComponent(LcColor4 tint);
	//
	void AddTintComponent(LcColor3 tint);
	//
	void AddColorsComponent(LcColor4 inLeftTop, LcColor4 inRightTop, LcColor4 inRightBottom, LcColor4 inLeftBottom);
	//
	void AddColorsComponent(LcColor3 inLeftTop, LcColor3 inRightTop, LcColor3 inRightBottom, LcColor3 inLeftBottom);
	//
	void AddTextureComponent(const std::string& inTexture);
	//
	void AddTextureComponent(const LcBytes& inData);
	//
	void AddCustomUVComponent(LcVector2 inLeftTop, LcVector2 inRightTop, LcVector2 inRightBottom, LcVector2 inLeftBottom);
	//
	void AddAnimationComponent(LcVector2 inFrameSize, unsigned short inNumFrames, float inFramesPerSecond);
	//
	void AddTiledSpriteComponent(const std::string& tiledJsonPath, const LcLayersList& inLayerNames = LcLayersList{});
	//
	void AddTiledSpriteComponent(const std::string& tiledJsonPath, LcObjectHandler inObjectHandler, const LcLayersList& inLayerNames = LcLayersList{});
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
