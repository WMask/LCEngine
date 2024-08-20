/**
* ColoredSpriteRenderVulkan.h
* 15.08.2024
* (c) Denis Romakhov
*/

#pragma once

#include "Core/LCTypes.h"
#include "RenderSystem/RenderSystem.h"
#include <vulkan/vulkan.h>


/**
* Colored sprite render */
class LcColoredSpriteRenderVulkan : public IVisual2DRender
{
public:
	/**
	* Constructor */
	LcColoredSpriteRenderVulkan(class IRenderDeviceVulkan& inRender, const LcAppContext& context);
	/**
	* Destructor */
	~LcColoredSpriteRenderVulkan();


public:// IVisual2DRender interface implementation
	//
	virtual void Setup(const IVisual* visual, const LcAppContext& context) override;
	//
	virtual void Render(const IVisual* visual, const LcAppContext& context) override;
	//
	virtual bool Supports(const TVFeaturesList& features) const override;


protected:
	//
	IRenderDeviceVulkan& render;
	//
	VkPipelineLayout pipelineLayout;
	//
	VkPipeline graphicsPipeline;

};
