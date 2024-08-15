/**
* ColoredSpriteRenderVulkan.h
* 15.08.2024
* (c) Denis Romakhov
*/

#pragma once

#include "Core/LCTypes.h"
#include "RenderSystem/RenderSystemVulkan/RenderSystemVulkan.h"


/**
* Colored sprite render */
class LcColoredSpriteRenderVulkan : public IVisual2DRender
{
public:
	/**
	* Constructor */
	LcColoredSpriteRenderVulkan(const LcAppContext& context);
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
	VkDevice device;
	//
	VkDescriptorSetLayout descriptorSetLayout;
	//
	VkPipelineLayout pipelineLayout;
	//
	VkPipeline graphicsPipeline;

};
