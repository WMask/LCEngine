/**
* Visual.h
* 03.02.2023
* (c) Denis Romakhov
*/

#pragma once

#include <set>
#include <deque>
#include <memory>

#include "Module.h"
#include "Core/LCTypesEx.h"

#pragma warning(disable : 4251)
#pragma warning(disable : 4275)


/** Visual component pointer */
typedef std::shared_ptr<class IVisualComponent> TVComponentPtr;

/** Visual component type */
enum class EVCType : int
{
	// Sprites
	Tint,
	VertexColor,
	Texture,
	CustomUV,
	FrameAnimation,
	// Widgets
	Text,
	Button,
	ClickHandler
};

/** Text font */
struct ITextFont
{
public:
	virtual ~ITextFont() {}
	//
	virtual std::wstring GetFontName() const = 0;
};

/** Font weight */
enum class LcFontWeight { Light, Normal, Bold };


/**
* Visual interface */
class IVisual
{
public:
	/**
	* Virtual destructor */
	virtual ~IVisual() {}
	/**
	* Init visual */
	virtual void Init(class IWorld* world) {}
	/**
	* Destroy visual */
	virtual void Destroy(class IWorld* world) {}
	/**
	* Update visual */
	virtual void Update(float deltaSeconds) {}
	/**
	* Add component */
	virtual void AddComponent(TVComponentPtr comp) = 0;
	/**
	* Get component */
	virtual TVComponentPtr GetComponent(EVCType type) const = 0;
	/**
	* Check for component type */
	virtual bool HasComponent(EVCType type) const = 0;
	/**
	* Visual size in pixels */
	virtual void SetSize(LcSizef size) = 0;
	/**
	* Visual size in pixels */
	virtual LcSizef GetSize() const = 0;
	/**
	* Visual position in pixels */
	virtual void SetPos(LcVector3 pos) = 0;
	/**
	* Visual position in pixels */
	virtual void AddPos(LcVector3 pos) = 0;
	/**
	* Visual position in pixels */
	virtual LcVector3 GetPos() const = 0;
	/**
	* Visual rotation Z axis */
	virtual void SetRotZ(float rotZ) = 0;
	/**
	* Visual rotation Z axis */
	virtual void AddRotZ(float rotZ) = 0;
	/**
	* Visual rotation Z axis */
	virtual float GetRotZ() const = 0;
	/**
	* Visual state */
	virtual void SetVisible(bool visible) = 0;
	/**
	* Visual state */
	virtual bool IsVisible() const = 0;
	/**
	* Mouse button event */
	virtual void OnMouseButton(LcMouseBtn btn, LcKeyState state, int x, int y) = 0;
	/**
	* Mouse move event */
	virtual void OnMouseMove(LcVector3 pos) = 0;
	/**
	* Mouse enter event */
	virtual void OnMouseEnter() = 0;
	/**
	* Mouse leave event */
	virtual void OnMouseLeave() = 0;

};


/** Visual feature list */
typedef std::set<EVCType> TVFeaturesList;


/**
* Visual component interface */
class IVisualComponent
{
public:
	/**
	* Constructor */
	IVisualComponent() : owner(nullptr) {}
	/**
	* Virtual destructor */
	virtual ~IVisualComponent() {}
	/**
	* Init component */
	virtual void Init(class IWorld& world) {}
	/**
	* Destroy component */
	virtual void Destroy(class IWorld& world) {}
	/**
	* Update component */
	virtual void Update(float deltaSeconds) {}
	/**
	* Get type */
	virtual EVCType GetType() const = 0;
	/**
	* Set owner */
	virtual void SetOwner(IVisual* inOwner) { owner = inOwner; }
	/**
	* Get owner */
	inline IVisual* GetOwner() const { return owner; }


protected:
	class IVisual* owner;
	friend class IVisual;

};


/**
* Visual texture component */
struct LcVisualTextureComponent : public IVisualComponent
{
	std::string texture;	// texture file path
	LcBytes data;			// texture data
	LcVector2 texSize;		// texture size in pixels
	//
	LcVisualTextureComponent() : texSize(LcDefaults::ZeroVec2) {}
	//
	LcVisualTextureComponent(const LcVisualTextureComponent& texture) :
		texture(texture.texture), data(texture.data), texSize(texture.texSize) {}
	//
	LcVisualTextureComponent(const std::string& inTexture) : texture(inTexture), texSize(LcDefaults::ZeroVec2)
	{
	}
	//
	LcVisualTextureComponent(const LcBytes& inData) : data(inData), texSize(LcDefaults::ZeroVec2)
	{
	}
	// IVisualComponent interface implementation
	virtual EVCType GetType() const override { return EVCType::Texture; }

};


/**
* Visual base interface */
class WORLD_API IVisualBase : public IVisual
{
public:// IVisual interface implementation
	//
	virtual void Destroy(class IWorld* world) override;
	//
	virtual void AddComponent(TVComponentPtr comp) override;
	//
	virtual TVComponentPtr GetComponent(EVCType type) const override;
	//
	virtual bool HasComponent(EVCType type) const override;


protected:
	std::deque<TVComponentPtr> components;
	//
	class IWorld* world;

};
