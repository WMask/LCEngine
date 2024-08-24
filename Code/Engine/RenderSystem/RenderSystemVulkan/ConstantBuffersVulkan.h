/**
* ConstantBuffersVulkan.h
* 20.08.2024
* (c) Denis Romakhov
*/

#pragma once

#include <array>
#include <deque>
#include <vulkan/vulkan.h>

#include "Module.h"
#include "RenderSystem/RenderSystem.h"
#include "World/WorldInterface.h"

#pragma warning(disable : 4251)
#pragma warning(disable : 5046)


/** Uniform descriptor layout types */
enum class LcDSLayoutType : int
{
	ColoredSprite,
	AnimatedSprite,
	TexturedVisual,
	TiledVisual,
	Max
};

static const int LcDSLayoutTypeSize = static_cast<int>(LcDSLayoutType::Max);


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
class LcConstantBuffersVulkan
{
public:
	//
	LcConstantBuffersVulkan(class IRenderDeviceVulkan& inRender);
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
	void SetTextureFor(LcDSLayoutType type, VkImageView imageView);
	//
	const VkDescriptorSet* GetDescriptorSetFor(LcDSLayoutType type) const;
	//
	inline const VkDescriptorSetLayout* GetLayoutFor(LcDSLayoutType type) const { return &descriptorLayouts[static_cast<int>(type)].layout; }
	//
	inline const LcMatrix4& GetViewMatrix() const { return buffer.mView; }
	//
	inline const LcMatrix4& GetProjMatrix() const { return buffer.mProj; }


protected:
	//
	void CreateForColoredSprite(const VkDescriptorSetLayoutBinding& uboLayoutBinding);
	//
	void CreateForTexturedVisual(
		const VkDescriptorSetLayoutBinding& uboLayoutBinding,
		const VkDescriptorSetLayoutBinding& samplerLayoutBinding,
		const VkDescriptorSetLayoutBinding& textureLayoutBinding);


protected:
	//
	struct LcDescriptorLayout
	{
		VkDescriptorSetLayout layout;
		//
		VkDescriptorPool pool;
		//
		std::vector<VkDescriptorSet> sets;
	};


protected:
	//
	class IRenderDeviceVulkan& render;
	//
	std::array<LcDescriptorLayout, LcDSLayoutTypeSize> descriptorLayouts;
	//
	LcUniformBufferObject buffer;
	//
	std::vector<VkBuffer> uniformBuffers;
	//
	std::vector<VkDeviceMemory> uniformBuffersMemory;
	//
	std::vector<void*> uniformBuffersMapped;
	//
	unsigned int frames;

};
