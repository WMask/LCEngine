/**
* DescriptorSetsVulkan.cpp
* 20.08.2024
* (c) Denis Romakhov
*/

#include "RenderSystem/RenderSystemVulkan/DescriptorSetsVulkan.h"
#include "RenderSystem/RenderSystemVulkan/RenderSystemVulkan.h"
#include "World/World.h"
#include "World/Camera.h"
#include "Core/LCException.h"
#include "Core/glm/ext/matrix_transform.hpp"


LcDescriptorSetsVulkan::LcDescriptorSetsVulkan(IRenderDeviceVulkan& inRender)
	: render(inRender)
	, maxPushConstSize(0)
	, uniformBuffers{}
	, uniformBuffersMemory{}
	, uniformBuffersMapped{}
{
	buffer.mView = IdentityMatrix();
	buffer.mProj = IdentityMatrix();
	buffer.globalTint = LcDefaults::White3;
}

void LcDescriptorSetsVulkan::Destroy(VkDevice device)
{
	for (uint32_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
	{
		vkDestroyBuffer(device, uniformBuffers[i], nullptr);
		vkFreeMemory(device, uniformBuffersMemory[i], nullptr);

		vkDestroyDescriptorSetLayout(device, descriptorLayouts[i].layout, nullptr);

		for (uint32_t j = 0; j < LcDSLayoutTypeSize; j++)
		{
			vkDestroyDescriptorPool(device, descriptorLayouts[j].pool[i], nullptr);
		}
	}
}

void LcDescriptorSetsVulkan::Create()
{
	auto device = render.GetVulkanDevice();
	auto physicalDevice = render.GetPhysicalDevice();

	LC_TRY

	VkPhysicalDeviceProperties properties{};
	vkGetPhysicalDeviceProperties(physicalDevice, &properties);
	maxPushConstSize = properties.limits.maxPushConstantsSize;

	VkDeviceSize bufferSize = sizeof(LcUniformBufferObject);

	for (uint32_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
	{
		CreateBuffer(device, render.GetPhysicalDevice(), bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
			uniformBuffers[i], uniformBuffersMemory[i]
		);

		vkMapMemory(render.GetVulkanDevice(), uniformBuffersMemory[i], 0, bufferSize, 0, &uniformBuffersMapped[i]);
	}

	VkDescriptorSetLayoutBinding uboLayoutBinding{};
	uboLayoutBinding.binding = UBO_BINDING_ID;
	uboLayoutBinding.descriptorCount = 1;
	uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	uboLayoutBinding.pImmutableSamplers = nullptr;
	uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

	VkDescriptorSetLayoutBinding samplerLayoutBinding{};
	samplerLayoutBinding.binding = SAMPLER_BINDING_ID;
	samplerLayoutBinding.descriptorCount = 1;
	samplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_SAMPLER;
	samplerLayoutBinding.pImmutableSamplers = nullptr;
	samplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

	CreateTextureLayout();

	for (uint32_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
	{
		CreateForColoredSprite(i, uboLayoutBinding);
		CreateForTexturedVisual(i, uboLayoutBinding, samplerLayoutBinding);
		CreateForTiledVisual(i, uboLayoutBinding, samplerLayoutBinding);
	}

	LC_CATCH{ LC_THROW("LcDescriptorSetsVulkan::Create()") }
}

void LcDescriptorSetsVulkan::UpdateTextures(uint32_t frame)
{
	auto device = render.GetVulkanDevice();
	auto& texLoader = render.GetTextureLoader();
	const uint32_t texturesCount = texLoader.GetNumTextures();

	LC_TRY

	if (frame >= MAX_FRAMES_IN_FLIGHT)
	{
		throw LcException("Invalid frame id");
	}

	LcDescriptorLayout& layout = descriptorLayouts.at(static_cast<int>(LcDSLayoutType::Textures));

	VkDescriptorPool& pool = layout.pool.at(frame);
	if (pool)
	{
		vkDestroyDescriptorPool(device, pool, nullptr);
	}

	if (texturesCount == 0)
	{
		pool = nullptr;
		return;
	}

	VkDescriptorPoolSize poolSize{};
	poolSize.type = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
	poolSize.descriptorCount = 1;

	VkDescriptorPoolCreateInfo poolInfo{};
	poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	poolInfo.poolSizeCount = 1;
	poolInfo.pPoolSizes = &poolSize;
	poolInfo.maxSets = texturesCount;

	VkResult result = vkCreateDescriptorPool(device, &poolInfo, nullptr, &pool);
	if (result != VK_SUCCESS)
	{
		throw LcException("Failed to create descriptor pool");
	}

	std::vector<VkDescriptorSetLayout> layouts(texturesCount, layout.layout);

	VkDescriptorSetAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocInfo.descriptorPool = pool;
	allocInfo.descriptorSetCount = texturesCount;
	allocInfo.pSetLayouts = layouts.data();

	auto& textureSets = layout.sets.at(frame);
	textureSets.resize(texturesCount);

	result = vkAllocateDescriptorSets(device, &allocInfo, textureSets.data());
	if (result != VK_SUCCESS)
	{
		throw LcException("Failed to allocate descriptor sets");
	}

	uint32_t textureId = 0;
	std::vector<VkDescriptorImageInfo> textureImages;
	std::vector<VkWriteDescriptorSet> textureWrites;
	textureImages.reserve(texturesCount);
	textureWrites.reserve(texturesCount);

	auto& textures = texLoader.GetTexturesCache();
	for (auto& textureIt : textures)
	{
		VkDescriptorSet& currentSet = textureSets[textureId];

		VkDescriptorImageInfo imageInfo{};
		imageInfo.imageView = textureIt.second.imageView;
		imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		textureImages.push_back(imageInfo);

		VkWriteDescriptorSet descriptorWrite{};
		descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWrite.dstSet = currentSet;
		descriptorWrite.dstBinding = TEXTURES_BINDING_ID;
		descriptorWrite.descriptorCount = 1;
		descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
		descriptorWrite.pImageInfo = &textureImages.back();
		textureWrites.push_back(descriptorWrite);

		textureIt.second.sets[frame] = currentSet;

		onTextureUpdated.Broadcast(textureIt.first.c_str(), frame, currentSet);

		textureId++;
	}

	vkUpdateDescriptorSets(device, static_cast<uint32_t>(textureWrites.size()), textureWrites.data(), 0, nullptr);

	LC_CATCH{ LC_THROW("LcDescriptorSetsVulkan::UpdateTexturesForFrame()") }
}

void LcDescriptorSetsVulkan::CreateTextureLayout()
{
	auto device = render.GetVulkanDevice();

	LC_TRY

	LcDescriptorLayout& layout = descriptorLayouts[static_cast<int>(LcDSLayoutType::Textures)];

	VkDescriptorSetLayoutBinding textureLayoutBinding{};
	textureLayoutBinding.binding = TEXTURES_BINDING_ID;
	textureLayoutBinding.descriptorCount = 1;
	textureLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
	textureLayoutBinding.pImmutableSamplers = nullptr;
	textureLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

	VkDescriptorSetLayoutCreateInfo layoutInfo{};
	layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	layoutInfo.bindingCount = 1;
	layoutInfo.pBindings = &textureLayoutBinding;

	VkResult result = vkCreateDescriptorSetLayout(device, &layoutInfo, nullptr, &layout.layout);
	if (result != VK_SUCCESS)
	{
		throw LcException("Failed to create descriptor set layout");
	}

	LC_CATCH{ LC_THROW("LcDescriptorSetsVulkan::CreateTextureLayouts()") }
}

void LcDescriptorSetsVulkan::CreateForColoredSprite(uint32_t frame, const VkDescriptorSetLayoutBinding& uboLayoutBinding)
{
	auto device = render.GetVulkanDevice();

	LC_TRY

	if (frame >= MAX_FRAMES_IN_FLIGHT)
	{
		throw LcException("Invalid frame id");
	}

	LcDescriptorLayout& layout = descriptorLayouts[static_cast<int>(LcDSLayoutType::ColoredSprite)];

	if (!layout.layout)
	{
		VkDescriptorSetLayoutCreateInfo layoutInfo{};
		layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		layoutInfo.bindingCount = 1;
		layoutInfo.pBindings = &uboLayoutBinding;

		VkResult result = vkCreateDescriptorSetLayout(device, &layoutInfo, nullptr, &layout.layout);
		if (result != VK_SUCCESS)
		{
			throw LcException("Failed to create descriptor set layout");
		}
	}

	VkDescriptorPoolSize poolSize;
	poolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	poolSize.descriptorCount = 1;

	VkDescriptorPoolCreateInfo poolInfo{};
	poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	poolInfo.poolSizeCount = 1;
	poolInfo.pPoolSizes = &poolSize;
	poolInfo.maxSets = 1;

	VkResult result = vkCreateDescriptorPool(device, &poolInfo, nullptr, &layout.pool[frame]);
	if (result != VK_SUCCESS)
	{
		throw LcException("Failed to create descriptor pool");
	}

	VkDescriptorSetAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocInfo.descriptorPool = layout.pool[frame];
	allocInfo.descriptorSetCount = 1;
	allocInfo.pSetLayouts = &layout.layout;

	layout.sets[frame].resize(1);
	VkDescriptorSet& currentSet = layout.sets[frame][0];

	result = vkAllocateDescriptorSets(device, &allocInfo, &currentSet);
	if (result != VK_SUCCESS)
	{
		throw LcException("Failed to allocate descriptor sets");
	}

	VkDescriptorBufferInfo bufferInfo{};
	bufferInfo.buffer = uniformBuffers[frame];
	bufferInfo.offset = 0;
	bufferInfo.range = sizeof(LcUniformBufferObject);

	VkWriteDescriptorSet descriptorWrite{};
	descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	descriptorWrite.dstSet = currentSet;
	descriptorWrite.dstBinding = UBO_BINDING_ID;
	descriptorWrite.dstArrayElement = 0;
	descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	descriptorWrite.descriptorCount = 1;
	descriptorWrite.pBufferInfo = &bufferInfo;

	vkUpdateDescriptorSets(device, 1, &descriptorWrite, 0, nullptr);

	LC_CATCH{ LC_THROW("LcDescriptorSetsVulkan::CreateForColoredSprite()") }
}

void LcDescriptorSetsVulkan::CreateForTexturedVisual(uint32_t frame, const VkDescriptorSetLayoutBinding& uboLayoutBinding, const VkDescriptorSetLayoutBinding& samplerLayoutBinding)
{
	auto device = render.GetVulkanDevice();

	LC_TRY

	if (frame >= MAX_FRAMES_IN_FLIGHT)
	{
		throw LcException("Invalid frame id");
	}

	LcDescriptorLayout& layout = descriptorLayouts[static_cast<int>(LcDSLayoutType::TexturedVisual)];

	std::array<VkDescriptorSetLayoutBinding, 2> bindings = { uboLayoutBinding, samplerLayoutBinding };

	if (!layout.layout)
	{
		VkDescriptorSetLayoutCreateInfo layoutInfo{};
		layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
		layoutInfo.pBindings = bindings.data();

		VkResult result = vkCreateDescriptorSetLayout(device, &layoutInfo, nullptr, &layout.layout);
		if (result != VK_SUCCESS)
		{
			throw LcException("Failed to create descriptor set layout");
		}
	}

	std::vector<VkDescriptorSetLayout> layouts(bindings.size(), layout.layout);

	std::array<VkDescriptorPoolSize, 2> poolSizes{};

	poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	poolSizes[0].descriptorCount = 1;
	poolSizes[1].type = VK_DESCRIPTOR_TYPE_SAMPLER;
	poolSizes[1].descriptorCount = 1;

	VkDescriptorPoolCreateInfo poolInfo{};
	poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
	poolInfo.pPoolSizes = poolSizes.data();
	poolInfo.maxSets = 2;

	VkResult result = vkCreateDescriptorPool(device, &poolInfo, nullptr, &layout.pool[frame]);
	if (result != VK_SUCCESS)
	{
		throw LcException("Failed to create descriptor pool");
	}

	VkDescriptorSetAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocInfo.descriptorPool = layout.pool[frame];
	allocInfo.descriptorSetCount = 1;
	allocInfo.pSetLayouts = layouts.data();

	layout.sets[frame].resize(1);
	VkDescriptorSet& currentSet = layout.sets[frame][0];

	result = vkAllocateDescriptorSets(device, &allocInfo, &currentSet);
	if (result != VK_SUCCESS)
	{
		throw LcException("Failed to allocate descriptor sets");
	}

	VkDescriptorBufferInfo bufferInfo{};
	bufferInfo.buffer = uniformBuffers[frame];
	bufferInfo.offset = 0;
	bufferInfo.range = sizeof(LcUniformBufferObject);

	VkDescriptorImageInfo samplerInfo{};
	samplerInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	samplerInfo.sampler = render.GetTextureSampler();

	std::array<VkWriteDescriptorSet, 2> descriptorWrites{};

	descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	descriptorWrites[0].dstSet = currentSet;
	descriptorWrites[0].dstBinding = UBO_BINDING_ID;
	descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	descriptorWrites[0].descriptorCount = 1;
	descriptorWrites[0].pBufferInfo = &bufferInfo;

	descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	descriptorWrites[1].dstSet = currentSet;
	descriptorWrites[1].dstBinding = SAMPLER_BINDING_ID;
	descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_SAMPLER;
	descriptorWrites[1].descriptorCount = 1;
	descriptorWrites[1].pImageInfo = &samplerInfo;

	vkUpdateDescriptorSets(device, static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);

	LC_CATCH{ LC_THROW("LcDescriptorSetsVulkan::CreateForTexturedVisual()") }
}

void LcDescriptorSetsVulkan::CreateForTiledVisual(uint32_t frame, const VkDescriptorSetLayoutBinding& uboLayoutBinding, const VkDescriptorSetLayoutBinding& samplerLayoutBinding)
{
	auto device = render.GetVulkanDevice();

	LC_TRY

	if (frame >= MAX_FRAMES_IN_FLIGHT)
	{
		throw LcException("Invalid frame id");
	}

	LcDescriptorLayout& layout = descriptorLayouts[static_cast<int>(LcDSLayoutType::TiledVisual)];

	std::array<VkDescriptorSetLayoutBinding, 2> bindings = { uboLayoutBinding, samplerLayoutBinding };

	if (!layout.layout)
	{
		VkDescriptorSetLayoutCreateInfo layoutInfo{};
		layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
		layoutInfo.pBindings = bindings.data();

		VkResult result = vkCreateDescriptorSetLayout(device, &layoutInfo, nullptr, &layout.layout);
		if (result != VK_SUCCESS)
		{
			throw LcException("Failed to create descriptor set layout");
		}
	}

	std::vector<VkDescriptorSetLayout> layouts(bindings.size(), layout.layout);

	std::array<VkDescriptorPoolSize, 2> poolSizes{};

	poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	poolSizes[0].descriptorCount = 1;
	poolSizes[1].type = VK_DESCRIPTOR_TYPE_SAMPLER;
	poolSizes[1].descriptorCount = 1;

	VkDescriptorPoolCreateInfo poolInfo{};
	poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
	poolInfo.pPoolSizes = poolSizes.data();
	poolInfo.maxSets = 2;

	VkResult result = vkCreateDescriptorPool(device, &poolInfo, nullptr, &layout.pool[frame]);
	if (result != VK_SUCCESS)
	{
		throw LcException("Failed to create descriptor pool");
	}

	VkDescriptorSetAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocInfo.descriptorPool = layout.pool[frame];
	allocInfo.descriptorSetCount = 1;
	allocInfo.pSetLayouts = layouts.data();

	layout.sets[frame].resize(1);
	VkDescriptorSet& currentSet = layout.sets[frame][0];

	result = vkAllocateDescriptorSets(device, &allocInfo, &currentSet);
	if (result != VK_SUCCESS)
	{
		throw LcException("Failed to allocate descriptor sets");
	}

	VkDescriptorBufferInfo bufferInfo{};
	bufferInfo.buffer = uniformBuffers[frame];
	bufferInfo.offset = 0;
	bufferInfo.range = sizeof(LcUniformBufferObject);

	VkDescriptorImageInfo samplerInfo{};
	samplerInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	samplerInfo.sampler = render.GetTextureSampler();

	std::array<VkWriteDescriptorSet, 2> descriptorWrites{};

	descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	descriptorWrites[0].dstSet = currentSet;
	descriptorWrites[0].dstBinding = UBO_BINDING_ID;
	descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	descriptorWrites[0].descriptorCount = 1;
	descriptorWrites[0].pBufferInfo = &bufferInfo;

	descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	descriptorWrites[1].dstSet = currentSet;
	descriptorWrites[1].dstBinding = SAMPLER_BINDING_ID;
	descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_SAMPLER;
	descriptorWrites[1].descriptorCount = 1;
	descriptorWrites[1].pImageInfo = &samplerInfo;

	vkUpdateDescriptorSets(device, static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);

	LC_CATCH{ LC_THROW("LcDescriptorSetsVulkan::CreateForTiledVisual()") }
}

void LcDescriptorSetsVulkan::LookAt(uint32_t frame, LcVector3 cameraPos, LcVector3 cameraTarget, bool updateUniforms)
{
	buffer.mView = LookAtMatrix(cameraPos, cameraTarget, false);

	if (updateUniforms)
	{
		memcpy(uniformBuffersMapped.at(frame), &buffer, sizeof(LcUniformBufferObject));
	}
}

void LcDescriptorSetsVulkan::SetOrtho(uint32_t frame, float widthPixels, float heightPixels, float nearPlane, float farPlane)
{
	buffer.mProj = OrthoMatrix(widthPixels, heightPixels, nearPlane, farPlane, false, false);

	memcpy(uniformBuffersMapped.at(frame), &buffer, sizeof(LcUniformBufferObject));
}

void LcDescriptorSetsVulkan::SetGlobalTint(uint32_t frame, LcColor3 tint)
{
	buffer.globalTint = tint;

	memcpy(uniformBuffersMapped.at(frame), &buffer, sizeof(LcUniformBufferObject));
}

const VkDescriptorSet& LcDescriptorSetsVulkan::GetDescriptorSet(uint32_t frame, LcDSLayoutType type) const
{
	auto& layout = descriptorLayouts[static_cast<int>(type)];

	return layout.sets.at(frame)[0];
}