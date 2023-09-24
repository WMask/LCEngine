/**
* Visual.cpp
* 31.08.2023
* (c) Denis Romakhov
*/

#pragma once

#include "pch.h"
#include "World/Visual.h"


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