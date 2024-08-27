/**
* ConstantBuffersVulkan.cpp
* 20.08.2024
* (c) Denis Romakhov
*/

#include "RenderSystem/RenderSystemVulkan/ConstantBuffersVulkan.h"
#include "RenderSystem/RenderSystemVulkan/RenderSystemVulkan.h"
#include "RenderSystem/RenderSystemVulkan/UtilsVulkan.h"
#include "World/World.h"
#include "World/Camera.h"
#include "Core/LCException.h"
#include "Core/glm/ext/matrix_transform.hpp"
#include <array>


LcConstantBuffersVulkan::LcConstantBuffersVulkan(IRenderDeviceVulkan& inRender)
	: render(inRender)
{
	buffer.mView = IdentityMatrix();
	buffer.mProj = IdentityMatrix();
	buffer.globalTint = LcDefaults::White3;
}

void LcConstantBuffersVulkan::Destroy(VkDevice device)
{
	size_t sz = std::min<size_t>(MAX_FRAMES_IN_FLIGHT, uniformBuffers.size());

	for (size_t i = 0; i < sz; i++)
	{
		vkDestroyBuffer(device, uniformBuffers[i], nullptr);
		vkFreeMemory(device, uniformBuffersMemory[i], nullptr);
	}

	uniformBuffers.clear();
	uniformBuffersMemory.clear();

	for (int i = 0; i < LcDSLayoutTypeSize; i++)
	{
		vkDestroyDescriptorPool(device, descriptorLayouts[i].pool, nullptr);
		vkDestroyDescriptorSetLayout(device, descriptorLayouts[i].layout, nullptr);
	}
}

void LcConstantBuffersVulkan::Create()
{
	auto device = render.GetVulkanDevice();

	LC_TRY

	VkDeviceSize bufferSize = sizeof(LcUniformBufferObject);

	uniformBuffers.resize(MAX_FRAMES_IN_FLIGHT);
	uniformBuffersMemory.resize(MAX_FRAMES_IN_FLIGHT);
	uniformBuffersMapped.resize(MAX_FRAMES_IN_FLIGHT);

	for (unsigned int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
	{
		CreateBuffer(device, render.GetPhysicalDevice(), bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
			uniformBuffers[i], uniformBuffersMemory[i]
		);

		vkMapMemory(render.GetVulkanDevice(), uniformBuffersMemory[i], 0, bufferSize, 0, &uniformBuffersMapped[i]);
	}

	VkDescriptorSetLayoutBinding uboLayoutBinding{};
	uboLayoutBinding.binding = 0;
	uboLayoutBinding.descriptorCount = 1;
	uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	uboLayoutBinding.pImmutableSamplers = nullptr;
	uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

	VkDescriptorSetLayoutBinding samplerLayoutBinding{};
	samplerLayoutBinding.binding = 1;
	samplerLayoutBinding.descriptorCount = 1;
	samplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_SAMPLER;
	samplerLayoutBinding.pImmutableSamplers = nullptr;
	samplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

	VkDescriptorSetLayoutBinding textureLayoutBinding{};
	textureLayoutBinding.binding = 2;
	textureLayoutBinding.descriptorCount = 1;
	textureLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
	textureLayoutBinding.pImmutableSamplers = nullptr;
	textureLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

	CreateForColoredSprite(uboLayoutBinding);
	CreateForTexturedVisual(uboLayoutBinding, samplerLayoutBinding, textureLayoutBinding);

	LC_CATCH{ LC_THROW("LcConstantBuffersVulkan::Create()") }
}

void LcConstantBuffersVulkan::CreateForColoredSprite(const VkDescriptorSetLayoutBinding& uboLayoutBinding)
{
	auto device = render.GetVulkanDevice();

	LC_TRY

	LcDescriptorLayout& layout = descriptorLayouts[static_cast<int>(LcDSLayoutType::ColoredSprite)];

	VkDescriptorSetLayoutCreateInfo layoutInfo{};
	layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	layoutInfo.bindingCount = 1;
	layoutInfo.pBindings = &uboLayoutBinding;

	VkResult result = vkCreateDescriptorSetLayout(device, &layoutInfo, nullptr, &layout.layout);
	if (result != VK_SUCCESS)
	{
		throw LcException("Failed to create descriptor set layout");
	}

	std::vector<VkDescriptorSetLayout> layouts(MAX_FRAMES_IN_FLIGHT, layout.layout);

	VkDescriptorPoolSize poolSize;
	poolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	poolSize.descriptorCount = MAX_FRAMES_IN_FLIGHT;

	VkDescriptorPoolCreateInfo poolInfo{};
	poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	poolInfo.poolSizeCount = 1;
	poolInfo.pPoolSizes = &poolSize;
	poolInfo.maxSets = MAX_FRAMES_IN_FLIGHT;

	result = vkCreateDescriptorPool(device, &poolInfo, nullptr, &layout.pool);
	if (result != VK_SUCCESS)
	{
		throw LcException("Failed to create descriptor pool");
	}

	VkDescriptorSetAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocInfo.descriptorPool = layout.pool;
	allocInfo.descriptorSetCount = MAX_FRAMES_IN_FLIGHT;
	allocInfo.pSetLayouts = layouts.data();

	layout.sets.resize(MAX_FRAMES_IN_FLIGHT);
	result = vkAllocateDescriptorSets(device, &allocInfo, layout.sets.data());
	if (result != VK_SUCCESS)
	{
		throw LcException("Failed to allocate descriptor sets");
	}

	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
	{
		VkDescriptorBufferInfo bufferInfo{};
		bufferInfo.buffer = uniformBuffers[i];
		bufferInfo.offset = 0;
		bufferInfo.range = sizeof(LcUniformBufferObject);

		VkWriteDescriptorSet descriptorWrite{};
		descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWrite.dstSet = layout.sets[i];
		descriptorWrite.dstBinding = 0;
		descriptorWrite.dstArrayElement = 0;
		descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		descriptorWrite.descriptorCount = 1;
		descriptorWrite.pBufferInfo = &bufferInfo;

		vkUpdateDescriptorSets(device, 1, &descriptorWrite, 0, nullptr);
	}

	LC_CATCH{ LC_THROW("LcConstantBuffersVulkan::CreateForColoredSprite()") }
}

void LcConstantBuffersVulkan::CreateForTexturedVisual(
	const VkDescriptorSetLayoutBinding& uboLayoutBinding,
	const VkDescriptorSetLayoutBinding& samplerLayoutBinding,
	const VkDescriptorSetLayoutBinding& textureLayoutBinding)
{
	auto device = render.GetVulkanDevice();
	const uint32_t framesCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);

	LC_TRY

	LcDescriptorLayout& layout = descriptorLayouts[static_cast<int>(LcDSLayoutType::TexturedVisual)];

	std::array<VkDescriptorSetLayoutBinding, 3> bindings = { uboLayoutBinding, samplerLayoutBinding, textureLayoutBinding };

	VkDescriptorSetLayoutCreateInfo layoutInfo{};
	layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
	layoutInfo.pBindings = bindings.data();

	VkResult result = vkCreateDescriptorSetLayout(device, &layoutInfo, nullptr, &layout.layout);
	if (result != VK_SUCCESS)
	{
		throw LcException("Failed to create descriptor set layout");
	}

	std::vector<VkDescriptorSetLayout> layouts(MAX_FRAMES_IN_FLIGHT, layout.layout);

	std::array<VkDescriptorPoolSize, 3> poolSizes{};

	poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	poolSizes[0].descriptorCount = framesCount;
	poolSizes[1].type = VK_DESCRIPTOR_TYPE_SAMPLER;
	poolSizes[1].descriptorCount = framesCount;
	poolSizes[2].type = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
	poolSizes[2].descriptorCount = framesCount;

	VkDescriptorPoolCreateInfo poolInfo{};
	poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
	poolInfo.pPoolSizes = poolSizes.data();
	poolInfo.maxSets = framesCount;

	result = vkCreateDescriptorPool(device, &poolInfo, nullptr, &layout.pool);
	if (result != VK_SUCCESS)
	{
		throw LcException("Failed to create descriptor pool");
	}

	VkDescriptorSetAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocInfo.descriptorPool = layout.pool;
	allocInfo.descriptorSetCount = framesCount;
	allocInfo.pSetLayouts = layouts.data();

	layout.sets.resize(framesCount);
	result = vkAllocateDescriptorSets(device, &allocInfo, layout.sets.data());
	if (result != VK_SUCCESS)
	{
		throw LcException("Failed to allocate descriptor sets");
	}

	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
	{
		VkDescriptorBufferInfo bufferInfo{};
		bufferInfo.buffer = uniformBuffers[i];
		bufferInfo.offset = 0;
		bufferInfo.range = sizeof(LcUniformBufferObject);

		VkDescriptorImageInfo samplerInfo{};
		samplerInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		samplerInfo.sampler = render.GetTextureSampler();

		std::array<VkWriteDescriptorSet, 2> descriptorWrites{};

		descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWrites[0].dstSet = layout.sets[i];
		descriptorWrites[0].dstBinding = 0;
		descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		descriptorWrites[0].descriptorCount = 1;
		descriptorWrites[0].pBufferInfo = &bufferInfo;

		descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWrites[1].dstSet = layout.sets[i];
		descriptorWrites[1].dstBinding = 1;
		descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_SAMPLER;
		descriptorWrites[1].descriptorCount = 1;
		descriptorWrites[1].pImageInfo = &samplerInfo;

		vkUpdateDescriptorSets(device, static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
	}

	LC_CATCH{ LC_THROW("LcConstantBuffersVulkan::CreateForTexturedVisual()") }
}

void LcConstantBuffersVulkan::LookAt(LcVector3 cameraPos, LcVector3 cameraTarget, bool updateUniforms)
{
	buffer.mView = LookAtMatrix(cameraPos, cameraTarget, false);

	auto currentFrame = render.GetCurrentFrame();
	if (currentFrame < uniformBuffersMapped.size() && updateUniforms)
	{
		memcpy(uniformBuffersMapped[currentFrame], &buffer, sizeof(LcUniformBufferObject));
	}
}

void LcConstantBuffersVulkan::SetOrtho(float widthPixels, float heightPixels, float nearPlane, float farPlane)
{
	buffer.mProj = OrthoMatrix(widthPixels, heightPixels, nearPlane, farPlane, false, false);

	auto currentFrame = render.GetCurrentFrame();
	if (currentFrame < uniformBuffersMapped.size())
	{
		memcpy(uniformBuffersMapped[currentFrame], &buffer, sizeof(LcUniformBufferObject));
	}
}

void LcConstantBuffersVulkan::SetGlobalTint(LcColor3 tint)
{
	buffer.globalTint = tint;

	auto currentFrame = render.GetCurrentFrame();
	if (currentFrame < uniformBuffersMapped.size())
	{
		memcpy(uniformBuffersMapped[currentFrame], &buffer, sizeof(LcUniformBufferObject));
	}
}

void LcConstantBuffersVulkan::SetTextureFor(LcDSLayoutType type, VkImageView imageView)
{
	auto device = render.GetVulkanDevice();
	auto currentFrame = render.GetCurrentFrame();
	LcDescriptorLayout& layout = descriptorLayouts[static_cast<int>(type)];

	switch (type)
	{
	case LcDSLayoutType::TexturedVisual:
		{
			VkDescriptorImageInfo imageInfo{};
			imageInfo.imageView = imageView;
			imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

			VkWriteDescriptorSet descriptorWrite{};
			descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptorWrite.dstSet = layout.sets[currentFrame];
			descriptorWrite.dstBinding = 2;
			descriptorWrite.descriptorCount = 1;
			descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
			descriptorWrite.pImageInfo = &imageInfo;

			vkUpdateDescriptorSets(device, 1, &descriptorWrite, 0, nullptr);
		}
		return;
	}

	throw LcException("LcConstantBuffersVulkan::SetTextureFor(): Invalid layout type");
}

const VkDescriptorSet* LcConstantBuffersVulkan::GetDescriptorSetFor(LcDSLayoutType type) const
{
	auto currentFrame = render.GetCurrentFrame();
	auto& layout = descriptorLayouts[static_cast<int>(type)];
	return (currentFrame < layout.sets.size()) ? &layout.sets[currentFrame] : nullptr;
}
