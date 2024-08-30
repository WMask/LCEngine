/**
* AnimatedSpriteRenderVulkan.h
* 30.08.2024
* (c) Denis Romakhov
*/

#pragma once

#include "Core/LCTypes.h"
#include "RenderSystem/RenderSystemVulkan/RenderSystemVulkan.h"


/**
* Animated sprite render, frame animation */
class LcAnimatedSpriteRenderVulkan : public IVisual2DRender
{
public:
	/**
	* Constructor */
	LcAnimatedSpriteRenderVulkan(IRenderDeviceVulkan& inRender, const LcAppContext& context);
	/**
	* Destructor */
	~LcAnimatedSpriteRenderVulkan();


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
