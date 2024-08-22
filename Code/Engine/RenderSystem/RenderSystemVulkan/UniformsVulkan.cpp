/**
* UniformsVulkan.cpp
* 20.08.2024
* (c) Denis Romakhov
*/

#include "RenderSystem/RenderSystemVulkan/UniformsVulkan.h"
#include "RenderSystem/RenderSystemVulkan/RenderSystemVulkan.h"
#include "World/World.h"
#include "World/Camera.h"
#include "Core/LCException.h"
#include "Core/glm/ext/matrix_transform.hpp"
#include <array>


LcUniformsVulkan::LcUniformsVulkan(IRenderDeviceVulkan& inRender)
	: render(inRender)
	, descriptorSetLayout(VK_NULL_HANDLE)
	, descriptorPool(VK_NULL_HANDLE)
	, frames(0)
{
	buffer.mView = IdentityMatrix();
	buffer.mProj = IdentityMatrix();
	buffer.globalTint = LcDefaults::White3;
}

void LcUniformsVulkan::Destroy(VkDevice device)
{
	size_t sz = std::min<size_t>(frames, uniformBuffers.size());

	for (size_t i = 0; i < sz; i++)
	{
		vkDestroyBuffer(device, uniformBuffers[i], nullptr);
		vkFreeMemory(device, uniformBuffersMemory[i], nullptr);
	}

	uniformBuffers.clear();
	uniformBuffersMemory.clear();

	if (descriptorPool)
	{
		vkDestroyDescriptorPool(device, descriptorPool, nullptr);
		descriptorPool = VK_NULL_HANDLE;
	}

	if (descriptorSetLayout)
	{
		vkDestroyDescriptorSetLayout(device, descriptorSetLayout, nullptr);
		descriptorSetLayout = VK_NULL_HANDLE;
	}
}

void LcUniformsVulkan::Create(unsigned int framesInFlight)
{
	auto device = render.GetVulkanDevice();

	LC_TRY

	VkDeviceSize bufferSize = sizeof(LcUniformBufferObject);

	frames = framesInFlight;
	uniformBuffers.resize(frames);
	uniformBuffersMemory.resize(frames);
	uniformBuffersMapped.resize(frames);

	for (unsigned int i = 0; i < frames; i++)
	{
		CreateBuffer(bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
			uniformBuffers[i], uniformBuffersMemory[i]
		);

		vkMapMemory(render.GetVulkanDevice(), uniformBuffersMemory[i], 0, bufferSize, 0, &uniformBuffersMapped[i]);
	}

	VkDescriptorSetLayoutBinding samplerLayoutBinding{};
	samplerLayoutBinding.binding = 0;
	samplerLayoutBinding.descriptorCount = 1;
	samplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_SAMPLER;
	samplerLayoutBinding.pImmutableSamplers = nullptr;
	samplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

	VkDescriptorSetLayoutBinding uboLayoutBinding{};
	uboLayoutBinding.binding = 1;
	uboLayoutBinding.descriptorCount = 1;
	uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	uboLayoutBinding.pImmutableSamplers = nullptr;
	uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

	std::array<VkDescriptorSetLayoutBinding, 2> bindings = { samplerLayoutBinding, uboLayoutBinding };

	VkDescriptorSetLayoutCreateInfo layoutInfo{};
	layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
	layoutInfo.pBindings = bindings.data();

	VkResult result = vkCreateDescriptorSetLayout(device, &layoutInfo, nullptr, &descriptorSetLayout);
	if (result != VK_SUCCESS)
	{
		throw LcException("Failed to create descriptor set layout");
	}

	std::vector<VkDescriptorSetLayout> layouts(frames, descriptorSetLayout);

	std::array<VkDescriptorPoolSize, 2> poolSizes{};

	poolSizes[0].type = VK_DESCRIPTOR_TYPE_SAMPLER;
	poolSizes[0].descriptorCount = frames;
	poolSizes[1].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	poolSizes[1].descriptorCount = frames;

	VkDescriptorPoolCreateInfo poolInfo{};
	poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
	poolInfo.pPoolSizes = poolSizes.data();
	poolInfo.maxSets = frames;

	result = vkCreateDescriptorPool(device, &poolInfo, nullptr, &descriptorPool);
	if (result != VK_SUCCESS)
	{
		throw LcException("Failed to create descriptor pool");
	}

	VkDescriptorSetAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocInfo.descriptorPool = descriptorPool;
	allocInfo.descriptorSetCount = frames;
	allocInfo.pSetLayouts = layouts.data();

	descriptorSets.resize(frames);
	result = vkAllocateDescriptorSets(device, &allocInfo, descriptorSets.data());
	if (result != VK_SUCCESS)
	{
		throw LcException("Failed to allocate descriptor sets");
	}

	for (size_t i = 0; i < frames; i++)
	{
		VkDescriptorBufferInfo bufferInfo{};
		bufferInfo.buffer = uniformBuffers[i];
		bufferInfo.offset = 0;
		bufferInfo.range = sizeof(LcUniformBufferObject);

		VkDescriptorImageInfo samplerInfo{};
		samplerInfo.sampler = render.GetTextureSampler();

		std::array<VkWriteDescriptorSet, 2> descriptorWrites{};

		descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWrites[0].dstSet = descriptorSets[i];
		descriptorWrites[0].dstBinding = 0;
		descriptorWrites[0].dstArrayElement = 0;
		descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_SAMPLER;
		descriptorWrites[0].descriptorCount = 1;
		descriptorWrites[0].pImageInfo = &samplerInfo;

		descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWrites[1].dstSet = descriptorSets[i];
		descriptorWrites[1].dstBinding = 1;
		descriptorWrites[1].dstArrayElement = 0;
		descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		descriptorWrites[1].descriptorCount = 1;
		descriptorWrites[1].pBufferInfo = &bufferInfo;

		vkUpdateDescriptorSets(device, static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
	}

	LC_CATCH{ LC_THROW("LcUniformsVulkan::Create()") }
}

void LcUniformsVulkan::LookAt(LcVector3 cameraPos, LcVector3 cameraTarget, bool updateUniforms)
{
	buffer.mView = LookAtMatrix(cameraPos, cameraTarget, false);

	auto imageIndex = render.GetCurrentImage();
	if (imageIndex < uniformBuffersMapped.size() && updateUniforms)
	{
		memcpy(uniformBuffersMapped[imageIndex], &buffer, sizeof(LcUniformBufferObject));
	}
}

void LcUniformsVulkan::SetOrtho(float widthPixels, float heightPixels, float nearPlane, float farPlane)
{
	buffer.mProj = OrthoMatrix(widthPixels, heightPixels, nearPlane, farPlane, false, false);

	auto imageIndex = render.GetCurrentImage();
	if (imageIndex < uniformBuffersMapped.size())
	{
		memcpy(uniformBuffersMapped[imageIndex], &buffer, sizeof(LcUniformBufferObject));
	}
}

void LcUniformsVulkan::SetGlobalTint(LcColor3 tint)
{
	buffer.globalTint = tint;

	auto imageIndex = render.GetCurrentImage();
	if (imageIndex < uniformBuffersMapped.size())
	{
		memcpy(uniformBuffersMapped[imageIndex], &buffer, sizeof(LcUniformBufferObject));
	}
}

const VkDescriptorSet* LcUniformsVulkan::GetCurrentDescriptorSet() const
{
	auto imageIndex = render.GetCurrentImage();
	return (imageIndex < descriptorSets.size()) ? &descriptorSets[imageIndex] : nullptr;
}

void LcUniformsVulkan::CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory)
{
	auto device = render.GetVulkanDevice();

	LC_TRY

	VkBufferCreateInfo bufferInfo{};
	bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferInfo.size = size;
	bufferInfo.usage = usage;
	bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	VkResult result = vkCreateBuffer(device, &bufferInfo, nullptr, &buffer);
	if (result != VK_SUCCESS)
	{
		throw LcException("Failed to create buffer");
	}

	VkMemoryRequirements memRequirements;
	vkGetBufferMemoryRequirements(device, buffer, &memRequirements);

	VkMemoryAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memRequirements.size;
	allocInfo.memoryTypeIndex = FindMemoryType(memRequirements.memoryTypeBits, properties);

	result = vkAllocateMemory(device, &allocInfo, nullptr, &bufferMemory);
	if (result != VK_SUCCESS)
	{
		throw LcException("Failed to allocate buffer memory");
	}

	vkBindBufferMemory(device, buffer, bufferMemory, 0);

	LC_CATCH{ LC_THROW("LcUniformsVulkan::CreateBuffer()") }
}

uint32_t LcUniformsVulkan::FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties)
{
	VkPhysicalDeviceMemoryProperties memProperties;
	vkGetPhysicalDeviceMemoryProperties(render.GetPhysicalDevice(), &memProperties);

	for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++)
	{
		if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties)
		{
			return i;
		}
	}

	throw LcException("Failed to find suitable memory type");
}
