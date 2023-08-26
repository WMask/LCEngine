/**
* Sprites.cpp
* 26.08.2023
* (c) Denis Romakhov
*/

#pragma once

#include "pch.h"
#include "Sprites.h"


void LcSprite::AddComponent(TVComponentPtr comp)
{
	components.push_back(comp);
	features.insert(comp->GetType());
}

TVComponentPtr LcSprite::GetComponent(EVCType type) const
{
	auto result = std::find_if(components.begin(), components.end(), [type](auto& comp) {
		return comp->GetType() == type;
	});
	return (result == components.end()) ? TVComponentPtr() : *result;
}

bool LcSprite::HasComponent(EVCType type) const
{
	auto result = std::find_if(components.begin(), components.end(), [type](auto& comp) {
		return comp->GetType() == type;
	});
	return result != components.end();
}
