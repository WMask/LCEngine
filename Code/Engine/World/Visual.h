/**
* Visual.h
* 03.02.2023
* (c) Denis Romakhov
*/

#pragma once

#include <set>
#include <deque>
#include <memory>
#include <functional>

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
	Tiled,
	// Widgets
	Text,
	Button,
	Checkbox,
	ClickHandler,
	CheckHandler,
	// Particles
	Particles
};


namespace LcCreatables
{
	constexpr int Sprite = 0;
	constexpr int Widget = 1;
}


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

/** Text alignment */
enum class LcTextAlignment { Left, Center, Right, Justified };

/** Visual feature list */
typedef std::set<EVCType> TVFeaturesList;


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
	virtual void Init(const LcAppContext& context) = 0;
	/**
	* Destroy visual */
	virtual void Destroy(const LcAppContext& context) = 0;
	/**
	* Update visual */
	virtual void Update(float deltaSeconds, const LcAppContext& context) = 0;
	/**
	* Add component */
	virtual void AddComponent(TVComponentPtr comp, const LcAppContext& context) = 0;
	/**
	* Remove component */
	virtual void RemoveComponent(class IVisualComponent* comp, const LcAppContext& context) = 0;
	/**
	* Get component */
	virtual TVComponentPtr GetComponent(EVCType type) const = 0;
	/**
	* Get features list */
	virtual const TVFeaturesList& GetFeaturesList() const = 0;
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
	* Visual position in pixels. Z not changed */
	virtual void SetPos(LcVector2 pos) = 0;
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
	* Set visual tag */
	virtual void SetTag(VisualTag tag) = 0;
	/**
	* Get visual tag */
	virtual VisualTag GetTag() const = 0;
	/**
	* Get visual id (LcCreatables) */
	virtual int GetTypeId() const = 0;
	/**
	* Mouse button event */
	virtual void OnMouseButton(LcMouseBtn btn, LcKeyState state, int x, int y, const LcAppContext& context) = 0;
	/**
	* Mouse move event */
	virtual void OnMouseMove(LcVector3 pos, const LcAppContext& context) = 0;
	/**
	* Mouse enter event */
	virtual void OnMouseEnter(const LcAppContext& context) = 0;
	/**
	* Mouse leave event */
	virtual void OnMouseLeave(const LcAppContext& context) = 0;

};

/** Visual feature list */
typedef std::function<void(class IVisualComponent&, const LcAppContext&)> TLifespanHandler;


/**
* Visual component interface */
class WORLD_API IVisualComponent
{
public:
	/**
	* Constructor */
	IVisualComponent() : owner(nullptr), lifespan(-1.0f) {}
	/**
	* Virtual destructor */
	virtual ~IVisualComponent() {}
	/**
	* Init component */
	virtual void Init(const LcAppContext& context) {}
	/**
	* Destroy component */
	virtual void Destroy(const LcAppContext& context) {}
	/**
	* Update component */
	virtual void Update(float deltaSeconds, const LcAppContext& context);
	/**
	* Set lifespan */
	virtual void SetLifespan(float seconds, TLifespanHandler onRemoved);
	/**
	* Set lifespan */
	virtual void SetLifespan(float seconds);
	/**
	* Set owner */
	virtual void SetOwner(IVisual* inOwner) { owner = inOwner; }
	/**
	* Get owner */
	inline IVisual* GetOwner() const { return owner; }
	/**
	* Get type */
	virtual EVCType GetType() const = 0;


protected:
	friend class IVisual;
	class IVisual* owner;
	//
	TLifespanHandler lifespanHandler;
	//
	float lifespan;

};


/**
* Visual base interface */
class WORLD_API IVisualBase : public IVisual
{
public:
	IVisualBase() : tag(-1) {}


public:// IVisual interface implementation
	//
	virtual void Init(const LcAppContext& context) override {}
	//
	virtual void Destroy(const LcAppContext& context) override;
	//
	virtual void Update(float deltaSeconds, const LcAppContext& context) override;
	//
	virtual void AddComponent(TVComponentPtr comp, const LcAppContext& context) override;
	//
	virtual void RemoveComponent(class IVisualComponent* comp, const LcAppContext& context) override;
	//
	virtual TVComponentPtr GetComponent(EVCType type) const override;
	//
	virtual bool HasComponent(EVCType type) const override;
	//
	virtual void SetTag(VisualTag inTag) override { tag = inTag; }
	//
	virtual VisualTag GetTag() const override { return tag; }


protected:
	std::deque<TVComponentPtr> components;
	//
	VisualTag tag;

};


/**
* Visual texture component */
class LcVisualTextureComponent : public IVisualComponent
{
public:
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
	//
	inline void SetTextureSize(LcVector2 newSize) { texSize = newSize; }
	//
	inline LcVector2 GetTextureSize() const { return texSize; }
	//
	inline std::string GetTexturePath() const { return texture; }


public:// IVisualComponent interface implementation
	//
	virtual EVCType GetType() const override { return EVCType::Texture; }


protected:
	std::string texture;	// texture file path
	LcBytes data;			// texture data
	LcVector2 texSize;		// texture size in pixels
};
