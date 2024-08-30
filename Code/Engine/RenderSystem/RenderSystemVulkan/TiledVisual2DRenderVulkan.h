/**
* TiledVisual2DRenderVulkan.h
* 29.08.2024
* (c) Denis Romakhov
*/

#pragma once

#include "Core/LCTypes.h"
#include "RenderSystem/RenderSystemVulkan/RenderSystemVulkan.h"


struct LC_VULKAN_TILES_BUFFER
{
	~LC_VULKAN_TILES_BUFFER();
	//
	uint32_t vertexCount;
	//
	VkBuffer buffer;
	//
	VkDeviceMemory bufferMemory;
};

using LcVulkanTileBuffersList = std::map<const IVisual*, LC_VULKAN_TILES_BUFFER>;


/**
* Textured visual render */
class LcTiledVisual2DRenderVulkan : public IVisual2DRender
{
public:
	//
	LcTiledVisual2DRenderVulkan(IRenderDeviceVulkan& render, const LcAppContext& context);
	//
	~LcTiledVisual2DRenderVulkan();
	//
	void RemoveTiles(const IVisual* visual) { vertexBuffers.erase(visual); }
	//
	void RemoveTiles() { vertexBuffers.clear(); }
	//
	void ClearCache(IWorld* world);
	//
	inline int GetNumTiles() const { return (int)vertexBuffers.size(); }
	//
	static VkDevice deviceInstance;


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
	LcVulkanTileBuffersList vertexBuffers;
	//
	VkPipelineLayout pipelineLayout;
	//
	VkPipeline graphicsPipeline;

};
