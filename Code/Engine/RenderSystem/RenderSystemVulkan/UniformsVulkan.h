/**
* UniformsVulkan.h
* 20.08.2024
* (c) Denis Romakhov
*/

#pragma once

#include <deque>
#include <vulkan/vulkan.h>

#include "Module.h"
#include "RenderSystem/RenderSystem.h"
#include "World/WorldInterface.h"

#pragma warning(disable : 4251)
#pragma warning(disable : 5046)

/**
* Uniform buffer */
struct LcUniformBufferObject
{
	alignas(16) LcMatrix4 mView;
	alignas(16) LcMatrix4 mProj;
	LcColor3 globalTint;
};


/**
* Vulkan uniforms */
class LcUniformsVulkan
{
public:
	//
	LcUniformsVulkan(class IRenderDeviceVulkan& inRender);
	//
	void Create(unsigned int framesInFlight);
	//
	void Destroy(VkDevice device);
	//
	void LookAt(LcVector3 cameraPos, LcVector3 cameraTarget, bool updateUniforms = true);
	//
	void LookAt(LcVector3 cameraPos, bool updateUniforms = true) { LookAt(cameraPos, { cameraPos.x, cameraPos.y, 1.0f }, updateUniforms); }
	//
	void SetOrtho(float widthPixels, float heightPixels, float nearPlane = -1.0f, float farPlane = 1.0f);
	//
	void SetOrtho(int widthPixels, int heightPixels) { SetOrtho(static_cast<float>(widthPixels), static_cast<float>(heightPixels)); }
	//
	void SetGlobalTint(LcColor3 tint);
	//
	const VkDescriptorSet* GetCurrentDescriptorSet() const;
	//
	inline const VkDescriptorSetLayout& GetDescriptorSetLayout() const { return descriptorSetLayout; }
	//
	inline const LcMatrix4& GetViewMatrix() const { return buffer.mView; }
	//
	inline const LcMatrix4& GetProjMatrix() const { return buffer.mProj; }


protected:
	//
	void CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);
	//
	uint32_t FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);


protected:
	//
	class IRenderDeviceVulkan& render;
	//
	LcUniformBufferObject buffer;
	//
	VkDescriptorSetLayout descriptorSetLayout;

	VkDescriptorPool descriptorPool;
	//
	std::vector<VkDescriptorSet> descriptorSets;
	//
	std::vector<VkBuffer> uniformBuffers;
	//
	std::vector<VkDeviceMemory> uniformBuffersMemory;
	//
	std::vector<void*> uniformBuffersMapped;
	//
	unsigned int frames;

};
