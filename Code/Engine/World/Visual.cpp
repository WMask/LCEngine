/**
* Visual.cpp
* 31.08.2023
* (c) Denis Romakhov
*/

#pragma once

#include "pch.h"
#include "World/Visual.h"


void IVisualBase::AddComponent(TVComponentPtr comp)
{
	if (!comp) throw std::exception("IVisualBase::AddComponent(): Invalud component");

	comp->SetOwner(this);
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
