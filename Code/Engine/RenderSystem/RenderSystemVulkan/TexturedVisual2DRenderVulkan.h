/**
* TexturedVisual2DRenderVulkan.h
* 24.08.2024
* (c) Denis Romakhov
*/

#pragma once

#include "Core/LCTypes.h"
#include "RenderSystem/RenderSystemVulkan/RenderSystemVulkan.h"


/**
* Textured visual render */
class LcTexturedVisual2DRenderVulkan : public IVisual2DRender
{
public:
	/**
	* Constructor */
	LcTexturedVisual2DRenderVulkan(class IRenderDeviceVulkan& inRender, const LcAppContext& context);
	/**
	* Destructor */
	~LcTexturedVisual2DRenderVulkan();


public:// IVisual2DRender interface implementation
	//
	virtual void Setup(const IVisual* visual, const LcAppContext& context) override;
	//
	virtual void Render(const IVisual* visual, const LcAppContext& context) override;
	//
	virtual bool Supports(const TVFeaturesList& features) const override;


protected:
	//
	class IRenderDeviceVulkan& render;
	//
	VkPipelineLayout pipelineLayout;
	//
	VkPipeline graphicsPipeline;

};
