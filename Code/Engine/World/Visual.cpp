/**
* Visual.cpp
* 31.08.2023
* (c) Denis Romakhov
*/

#pragma once

#include "pch.h"
#include "World/Visual.h"

void IVisualBase::Destroy(class IWorld* worldPtr)
{
	world = worldPtr;
	if (world)
	{
		for (auto& comp : components) comp->Destroy(*world);
	}
}

void IVisualBase::AddComponent(TVComponentPtr comp)
{
	if (!comp) throw std::exception("IVisualBase::AddComponent(): Invalud component");

	comp->SetOwner(this);
	comp->Init(*world);
	components.push_back(comp);
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