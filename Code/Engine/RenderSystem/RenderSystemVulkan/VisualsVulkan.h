/**
* VisualsVulkan.h
* 29.08.2024
* (c) Denis Romakhov
*/

#pragma once

#include <vulkan/vulkan.h>
#include <string>
#include <map>

#include "GUI/Widgets.h"
#include "World/Sprites.h"
#include "Core/LCTypesEx.h"


/**
* Vulkan Sprite implementation */
class LcSpriteVulkan : public LcSprite
{
public:
	LcSpriteVulkan() : spriteTex(nullptr) {}
	//
	VkDescriptorSet spriteTex;


public: // IVisualBase interface implementation
	//
	virtual void AddComponent(TVComponentPtr comp, const LcAppContext& context) override;

};


/**
* Vulkan Widget implementation */
class LcWidgetVulkan : public LcWidget
{
public:
	LcWidgetVulkan() : spriteTex(nullptr) {}
	//
	VkDescriptorSet spriteTex;


public: // IVisualBase interface implementation
	//
	virtual void AddComponent(TVComponentPtr comp, const LcAppContext& context) override;

};
