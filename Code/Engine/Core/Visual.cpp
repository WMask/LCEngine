/**
* Visual.cpp
* 31.08.2023
* (c) Denis Romakhov
*/

#pragma once

#include "pch.h"
#include "Visual.h"
#include "World/WorldInterface.h"
#include "Core/LCUtils.h"


LcFontWeight ToWeight(const std::string& weight)
{
	std::string weightName = ToLower(weight.c_str());

	if (weightName == "light")
		return LcFontWeight::Light;
	else if (weightName == "bold")
		return LcFontWeight::Bold;

	return LcFontWeight::Normal;
}

LcTextAlignment ToAlignment(const std::string& alignment)
{
	std::string alignmentName = ToLower(alignment.c_str());

	if (alignmentName == "left")
		return LcTextAlignment::Left;
	else if (alignmentName == "right")
		return LcTextAlignment::Right;
	else if (alignmentName == "justified")
		return LcTextAlignment::Justified;

	return LcTextAlignment::Center;
}

class LcVisualTintComponent : public IVisualTintComponent
{
public:
	LcVisualTintComponent(const LcVisualTintComponent& colors) : tint(colors.tint)
	{
		SetColor(tint);
	}
	//
	LcVisualTintComponent(LcColor4 inTint) : tint(inTint)
	{
		SetColor(tint);
	}
	//
	LcVisualTintComponent(LcColor3 inTint) : tint(inTint.x, inTint.y, inTint.z, 1.0f)
	{
		SetColor(tint);
	}


public: // IVisualTintComponent interface implementation
	//
	void SetColor(LcColor4 inTint) { data[0] = data[1] = data[2] = data[3] = inTint; }
	//
	const void* GetData() const { return data; }


public: // IVisualComponent interface implementation
	//
	virtual EVCType GetType() const override { return LcComponents::Tint; }


protected:
	LcColor4 tint;
	LcColor4 data[4];
};


class LcVisualColorsComponent : public IVisualColorsComponent
{
public:
	LcVisualColorsComponent() : leftTop{}, rightTop{}, rightBottom{}, leftBottom{} {}
	//
	LcVisualColorsComponent(const LcVisualColorsComponent& colors) :
		leftTop{ colors.leftTop }, rightTop{ colors.rightTop }, rightBottom{ colors.rightBottom }, leftBottom{ colors.leftBottom }
	{
	}
	//
	LcVisualColorsComponent(LcColor4 inLeftTop, LcColor4 inRightTop, LcColor4 inRightBottom, LcColor4 inLeftBottom) :
		leftTop(inLeftTop), rightTop(inRightTop), rightBottom(inRightBottom), leftBottom(inLeftBottom)
	{
	}
	//
	LcVisualColorsComponent(LcColor3 inLeftTop, LcColor3 inRightTop, LcColor3 inRightBottom, LcColor3 inLeftBottom) :
		leftTop(To4(inLeftTop)), rightTop(To4(inRightTop)), rightBottom(To4(inRightBottom)), leftBottom(To4(inLeftBottom))
	{
	}


public: // IVisualColorsComponent interface implementation
	//
	const void* GetData() const { return &leftTop; }


public: // IVisualComponent interface implementation
	//
	virtual EVCType GetType() const override { return LcComponents::VertexColor; }


protected:
	LcColor4 leftTop;
	LcColor4 rightTop;
	LcColor4 rightBottom;
	LcColor4 leftBottom;
};


class LcVisualTextureComponent : public IVisualTextureComponent
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


public: // IVisualComponent interface implementation
	//
	virtual void SetTextureSize(LcVector2 newSize) override { texSize = newSize; }
	//
	virtual LcVector2 GetTextureSize() const override { return texSize; }
	//
	virtual std::string GetTexturePath() const override { return texture; }


public: // IVisualComponent interface implementation
	//
	virtual EVCType GetType() const override { return LcComponents::Texture; }


protected:
	std::string texture;	// texture file path
	LcBytes data;			// texture data
	LcVector2 texSize;		// texture size in pixels
};


void IVisual::AddTintComponent(const LcAppContext& context, LcColor4 tint)
{
	AddComponent(std::make_shared<LcVisualTintComponent>(tint), context);
}

void IVisual::AddTintComponent(const LcAppContext& context, LcColor3 tint)
{
	AddComponent(std::make_shared<LcVisualTintComponent>(tint), context);
}

void IVisual::AddColorsComponent(const LcAppContext& context, LcColor4 inLeftTop, LcColor4 inRightTop, LcColor4 inRightBottom, LcColor4 inLeftBottom)
{
	AddComponent(std::make_shared<LcVisualColorsComponent>(inLeftTop, inRightTop, inRightBottom, inLeftBottom), context);
}

void IVisual::AddColorsComponent(const LcAppContext& context, LcColor3 inLeftTop, LcColor3 inRightTop, LcColor3 inRightBottom, LcColor3 inLeftBottom)
{
	AddComponent(std::make_shared<LcVisualColorsComponent>(inLeftTop, inRightTop, inRightBottom, inLeftBottom), context);
}

void IVisual::AddTextureComponent(const LcAppContext& context, const std::string& inTexture)
{
	AddComponent(std::make_shared<LcVisualTextureComponent>(inTexture), context);
}

void IVisual::AddTextureComponent(const LcAppContext& context, const LcBytes& inData)
{
	AddComponent(std::make_shared<LcVisualTextureComponent>(inData), context);
}


void IVisualComponent::Update(float deltaSeconds, const LcAppContext& context)
{
	if (lifespan > 0.0f)
	{
		if ((lifespan - deltaSeconds) < 0.0f)
		{
			lifespan = -1.0f;
			if (lifespanHandler) lifespanHandler(*this, context);
			if (owner) owner->RemoveComponent(this, context);
		}
		else
		{
			lifespan -= deltaSeconds;
		}
	}
}

void IVisualComponent::SetLifespan(float seconds, TLifespanHandler onRemoved)
{
	lifespan = seconds;
	lifespanHandler = onRemoved;
}

void IVisualComponent::SetLifespan(float seconds)
{
	lifespan = seconds;
}


void IVisualBase::Destroy(const LcAppContext& context)
{
	for (auto& comp : components) comp->Destroy(context);
}

void IVisualBase::Update(float deltaSeconds, const LcAppContext& context)
{
	for (auto& comp : components) comp->Update(deltaSeconds, context);
}

void IVisualBase::AddComponent(TVComponentPtr comp, const LcAppContext& context)
{
	if (!comp) throw std::exception("IVisualBase::AddComponent(): Invalid component");

	comp->SetOwner(this);
	components.push_back(comp);
	components.back()->Init(context);
}

void IVisualBase::RemoveComponent(class IVisualComponent* comp, const LcAppContext& context)
{
	auto it = std::find_if(components.begin(), components.end(), [comp](const TVComponentPtr& ptr) {
		return comp == ptr.get();
	});

	if (it != components.end())
	{
		(*it)->Destroy(context);
		components.erase(it);
	}
}

TVComponentPtr IVisualBase::GetComponent(EVCType type) const
{
	auto result = std::find_if(components.begin(), components.end(), [type](auto& comp) {
		return comp->GetType() == type;
	});
	return (result == components.end()) ? TVComponentPtr() : *result;
}

bool IVisualBase::HasComponent(EVCType type) const
{
	auto result = std::find_if(components.begin(), components.end(), [type](auto& comp) {
		return comp->GetType() == type;
	});
	return result != components.end();
}


void LcVisualHelper::SetTag(ObjectTag tag) const
{
	if (auto visual = context.world->GetLastAddedVisual())
	{
		visual->SetTag(tag);
	}
}

void LcVisualHelper::AddTintComponent(LcColor4 tint) const
{
	if (auto visual = context.world->GetLastAddedVisual())
	{
		visual->AddTintComponent(context, tint);
	}
}

void LcVisualHelper::AddTintComponent(LcColor3 tint) const
{
	if (auto visual = context.world->GetLastAddedVisual())
	{
		visual->AddTintComponent(context, tint);
	}
}

void LcVisualHelper::AddColorsComponent(LcColor4 inLeftTop, LcColor4 inRightTop, LcColor4 inRightBottom, LcColor4 inLeftBottom) const
{
	if (auto visual = context.world->GetLastAddedVisual())
	{
		visual->AddColorsComponent(context, inLeftTop, inRightTop, inRightBottom, inLeftBottom);
	}
}

void LcVisualHelper::AddColorsComponent(LcColor3 inLeftTop, LcColor3 inRightTop, LcColor3 inRightBottom, LcColor3 inLeftBottom) const
{
	if (auto visual = context.world->GetLastAddedVisual())
	{
		visual->AddColorsComponent(context, inLeftTop, inRightTop, inRightBottom, inLeftBottom);
	}
}

void LcVisualHelper::AddTextureComponent(const std::string& inTexture) const
{
	if (auto visual = context.world->GetLastAddedVisual())
	{
		visual->AddTextureComponent(context, inTexture);
	}
}

void LcVisualHelper::AddTextureComponent(const LcBytes& inData) const
{
	if (auto visual = context.world->GetLastAddedVisual())
	{
		visual->AddTextureComponent(context, inData);
	}
}
