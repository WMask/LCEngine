/**
* DescriptorSetsVulkan.h
* 20.08.2024
* (c) Denis Romakhov
*/

#pragma once

#include <deque>
#include <vulkan/vulkan.h>

#include "Module.h"
#include "RenderSystem/RenderSystem.h"
#include "World/WorldInterface.h"
#include "RenderSystem/RenderSystemVulkan/UtilsVulkan.h"

#pragma warning(disable : 4251)
#pragma warning(disable : 5046)


/** Uniform descriptor layout types */
enum class LcDSLayoutType : int
{
	Textures,
	ColoredSprite,
	AnimatedSprite,
	TexturedVisual,
	TiledVisual,
	Max
};

static const uint32_t LcDSLayoutTypeSize = static_cast<int>(LcDSLayoutType::Max);


/**
* Uniform buffer */
struct LcUniformBufferObject
{
	alignas(16) LcMatrix4 mView;
	alignas(16) LcMatrix4 mProj;
	LcColor3 globalTint;
};


/**
* Vulkan descriptor sets */
class LcDescriptorSetsVulkan
{
public:
	using TDescriptors = std::vector<VkDescriptorSet>;
	using TDescriptorsConstPtr = const TDescriptors*;
	using TTextureUpdatedDelegate = LcDelegate<const char* /* path */, int /* frame */, VkDescriptorSet /* texture image */>;
	//
	TTextureUpdatedDelegate onTextureUpdated;


public:
	//
	LcDescriptorSetsVulkan(class IRenderDeviceVulkan& render);
	//
	void Create();
	//
	void Destroy(VkDevice device);
	//
	void LookAt(uint32_t frame, LcVector3 cameraPos, LcVector3 cameraTarget, bool updateUniforms = true);
	//
	void LookAt(uint32_t frame, LcVector3 cameraPos, bool updateUniforms = true) { LookAt(frame, cameraPos, { cameraPos.x, cameraPos.y, 1.0f }, updateUniforms); }
	//
	void SetOrtho(uint32_t frame, float widthPixels, float heightPixels, float nearPlane = 1.0f, float farPlane = -1.0f);
	//
	void SetOrtho(uint32_t frame, int widthPixels, int heightPixels) { SetOrtho(frame, static_cast<float>(widthPixels), static_cast<float>(heightPixels)); }
	//
	void SetOrtho(uint32_t frame, unsigned int widthPixels, unsigned int heightPixels) { SetOrtho(frame, static_cast<float>(widthPixels), static_cast<float>(heightPixels)); }
	//
	void SetGlobalTint(uint32_t frame, LcColor3 tint);
	//
	void UpdateTextures(uint32_t frame);
	//
	const VkDescriptorSet& GetDescriptorSet(uint32_t frame, LcDSLayoutType type) const;
	//
	inline const VkDescriptorSetLayout* GetLayout(LcDSLayoutType type) const { return &descriptorLayouts[static_cast<int>(type)].layout; }
	//
	inline const LcMatrix4& GetViewMatrix() const { return buffer.mView; }
	//
	inline const LcMatrix4& GetProjMatrix() const { return buffer.mProj; }
	//
	inline uint32_t GetMaxPushConstSize() const { return maxPushConstSize; }


protected:
	//
	void CreateTextureLayout();
	//
	void CreateForColoredSprite(uint32_t frame, const VkDescriptorSetLayoutBinding& uboLayoutBinding);
	//
	void CreateForTexturedVisual(uint32_t frame, const VkDescriptorSetLayoutBinding& uboLayoutBinding, const VkDescriptorSetLayoutBinding& samplerLayoutBinding);
	//
	void CreateForTiledVisual(uint32_t frame, const VkDescriptorSetLayoutBinding& uboLayoutBinding, const VkDescriptorSetLayoutBinding& samplerLayoutBinding);


protected:
	//
	struct LcDescriptorLayout
	{
		VkDescriptorSetLayout layout;
		//
		std::array<VkDescriptorPool, MAX_FRAMES_IN_FLIGHT> pool;
		//
		std::array<std::vector<VkDescriptorSet>, MAX_FRAMES_IN_FLIGHT> sets;
	};


protected:
	//
	class IRenderDeviceVulkan& render;
	//
	LcUniformBufferObject buffer;
	//
	std::array<LcDescriptorLayout, LcDSLayoutTypeSize> descriptorLayouts;
	//
	std::array<VkBuffer, MAX_FRAMES_IN_FLIGHT> uniformBuffers;
	//
	std::array<VkDeviceMemory, MAX_FRAMES_IN_FLIGHT> uniformBuffersMemory;
	//
	std::array<void*, MAX_FRAMES_IN_FLIGHT> uniformBuffersMapped;
	//
	uint32_t maxPushConstSize;

};
