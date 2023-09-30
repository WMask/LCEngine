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
#include "LCTypesEx.h"

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
class CORE_API IVisual
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
	* Set visual tag. Default: -1 */
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


public:
	/**
	* Add tint component to the last added visual */
	void AddTintComponent(LcColor4 tint, const LcAppContext& context);
	/**
	* Add tint component to the last added visual */
	void AddTintComponent(LcColor3 tint, const LcAppContext& context);
	/**
	* Add colors component to the last added visual */
	void AddColorsComponent(LcColor4 inLeftTop, LcColor4 inRightTop, LcColor4 inRightBottom, LcColor4 inLeftBottom, const LcAppContext& context);
	/**
	* Add colors component to the last added visual */
	void AddColorsComponent(LcColor3 inLeftTop, LcColor3 inRightTop, LcColor3 inRightBottom, LcColor3 inLeftBottom, const LcAppContext& context);
	/**
	* Add texture component to the last added visual */
	void AddTextureComponent(const std::string& inTexture, const LcAppContext& context);
	/**
	* Add texture component to the last added visual */
	void AddTextureComponent(const LcBytes& inData, const LcAppContext& context);


public:
	class IVisualTintComponent* GetTintComponent() const { return (class IVisualTintComponent*)GetComponent(EVCType::Tint).get(); }
	//
	class IVisualColorsComponent* GetColorsComponent() const { return (class IVisualColorsComponent*)GetComponent(EVCType::VertexColor).get(); }
	//
	class IVisualTextureComponent* GetTextureComponent() const { return (class IVisualTextureComponent*)GetComponent(EVCType::Texture).get(); }

};


/** Visual feature list */
typedef std::function<void(class IVisualComponent&, const LcAppContext&)> TLifespanHandler;


/**
* Visual component interface */
class CORE_API IVisualComponent
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
class CORE_API IVisualBase : public IVisual
{
public:
	IVisualBase() : tag(-1) {}


public: // IVisual interface implementation
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


/** Visual helper */
class CORE_API LcVisualHelper
{
public:
	LcVisualHelper(const LcAppContext& inContext) : context(inContext) {}


public:
	/**
	* Add tint component to the last added visual */
	void AddTintComponent(LcColor4 tint) const;
	/**
	* Add tint component to the last added visual */
	void AddTintComponent(LcColor3 tint) const;
	/**
	* Add colors component to the last added visual */
	void AddColorsComponent(LcColor4 inLeftTop, LcColor4 inRightTop, LcColor4 inRightBottom, LcColor4 inLeftBottom) const;
	/**
	* Add colors component to the last added visual */
	void AddColorsComponent(LcColor3 inLeftTop, LcColor3 inRightTop, LcColor3 inRightBottom, LcColor3 inLeftBottom) const;
	/**
	* Add texture component to the last added visual */
	void AddTextureComponent(const std::string& inTexture) const;
	/**
	* Add texture component to the last added visual */
	void AddTextureComponent(const LcBytes& inData) const;
	/**
	* Set tag to the last added visual */
	void SetTag(VisualTag tag) const;


protected:
	const LcAppContext& context;

};


/** Visual tint component */
class IVisualTintComponent : public IVisualComponent
{
public:
	//
	virtual void SetColor(LcColor4 inTint) = 0;
	// return visual color data LcColor4[4]
	virtual const void* GetData() const = 0;
};


/** Visual colors component */
class IVisualColorsComponent : public IVisualComponent
{
public:
	// return visual color data LcColor4[4]
	virtual const void* GetData() const = 0;
};


/**
* Visual texture component */
class IVisualTextureComponent : public IVisualComponent
{
public:
	//
	virtual void SetTextureSize(LcVector2 newSize) = 0;
	//
	virtual LcVector2 GetTextureSize() const = 0;
	//
	virtual std::string GetTexturePath() const = 0;
};