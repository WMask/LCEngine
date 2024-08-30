/**
* RenderSystemVulkan.h
* 14.08.2024
* (c) Denis Romakhov
*/

#pragma once

#include <deque>
#include <vulkan/vulkan.h>

#include "Module.h"
#include "RenderSystem/RenderSystem.h"
#include "RenderSystem/RenderSystemVulkan/DescriptorSetsVulkan.h"
#include "RenderSystem/RenderSystemVulkan/UtilsVulkan.h"
#include "World/WorldInterface.h"

#pragma warning(disable : 4251)
#pragma warning(disable : 5046)

typedef std::deque<std::unique_ptr<IVisual2DRender>> TVisual2DRenderList;


/**
* Render device */
class IRenderDeviceVulkan
{
public:
	/**
	* Create shader module */
	virtual VkShaderModule CreateShaderModule(const std::vector<uint32_t>& code) = 0;
	/**
	* Fill pipeline defaults */
	virtual void FillPipelineDefaults(VkGraphicsPipelineCreateInfo& pipeline) = 0;
	/**
	* Force sprite render setup. Updates shaders and buffers */
	virtual void ForceRenderSetup() = 0;
	/**
	* Return Vulkan device */
	virtual VkDevice GetVulkanDevice() const = 0;
	/**
	* Return Physical device */
	virtual VkPhysicalDevice GetPhysicalDevice() const = 0;
	/**
	* Return command buffer */
	virtual VkCommandBuffer GetCommandBuffer() const = 0;
	/**
	* Return render pass */
	virtual VkRenderPass GetRenderPass() const = 0;
	/**
	* Return command pool */
	virtual VkCommandPool GetCommandPool() const = 0;
	/**
	* Return graphics queue */
	virtual VkQueue GetGraphicsQueue() const = 0;
	/**
	* Return default texture sampler */
	virtual VkSampler GetTextureSampler() const = 0;
	/**
	* Return texture loder */
	virtual LcTextureLoaderVulkan& GetTextureLoader() = 0;
	/**
	* Get sprite renders */
	virtual TVisual2DRenderList& GetVisual2DRenderList() = 0;
	/**
	* Get descriptor sets */
	virtual const LcDescriptorSetsVulkan& GetDescriptorSets() const = 0;
	/**
	* Get descriptor sets */
	virtual LcDescriptorSetsVulkan& GetDescriptorSets() = 0;
	/**
	* Get current frame */
	virtual uint32_t GetCurrentFrame() const = 0;
	/**
	* Get shader code */
	virtual std::string GetShaderCode(const std::string& shaderName) const = 0;

};


/**
* Vulkan render system */
class RENDERSYSTEMVULKAN_API LcRenderSystemVulkan
	: public LcRenderSystemBase
	, public LcUncopyable
	, public IRenderDeviceVulkan
{
public:
	LcRenderSystemVulkan();


public:// IRenderSystem interface implementation
	//
	virtual ~LcRenderSystemVulkan() override;
	//
	virtual void Create(void* windowHandle, LcWinMode mode, bool inVSync, bool inAllowFullscreen, const LcAppContext& context) override;
	//
	virtual void Shutdown() override;
	//
	virtual void Clear(class IWorld* world, bool removeRooted = false) override;
	//
	virtual void Subscribe(const LcAppContext& context);
	//
	virtual void Update(float deltaSeconds, const LcAppContext& context) override;
	//
	virtual void UpdateCamera(float deltaSeconds, LcVector3 newPos, LcVector3 newTarget) override;
	//
	virtual void Render(const LcAppContext& context) override;
	//
	virtual void RequestResize(int width, int height) override;
	//
	virtual void Resize(int width, int height, const LcAppContext& context) override;
	//
	virtual void SetMode(LcWinMode mode) override;
	//
	virtual bool CanRender() const override { return isInitialized; }
	//
	virtual LcRSStats GetStats() const override;
	//
	virtual LcRenderSystemType GetType() const override { return LcRenderSystemType::Vulkan; }


protected:// LcRenderSystemBase interface implementation
	//
	virtual void Render(const IVisual* visual, const LcAppContext& context) override;


public:// IRenderDeviceVulkan interface implementation
	//
	virtual VkShaderModule CreateShaderModule(const std::vector<uint32_t>& code) override;
	//
	virtual void FillPipelineDefaults(VkGraphicsPipelineCreateInfo& pipeline) override;
	//
	virtual void ForceRenderSetup() override { prevSetupRequested = true; }
	//
	virtual VkDevice GetVulkanDevice() const override { return device; }
	//
	virtual VkPhysicalDevice GetPhysicalDevice() const override { return physicalDevice; }
	//
	virtual VkCommandBuffer GetCommandBuffer() const override { return commandBuffers[currentFrame]; }
	//
	virtual VkRenderPass GetRenderPass() const override { return renderPass; }
	//
	virtual VkCommandPool GetCommandPool() const override { return commandPool; }
	//
	virtual VkQueue GetGraphicsQueue() const override { return graphicsQueue; }
	//
	virtual VkSampler GetTextureSampler() const override { return textureSampler; }
	//
	virtual LcTextureLoaderVulkan& GetTextureLoader() override { return texLoader; }
	//
	virtual TVisual2DRenderList& GetVisual2DRenderList() override { return visual2DRenders; }
	//
	virtual const LcDescriptorSetsVulkan& GetDescriptorSets() const override { return descriptorSets; }
	//
	virtual LcDescriptorSetsVulkan& GetDescriptorSets() override { return descriptorSets; }
	//
	virtual uint32_t GetCurrentFrame() const override { return currentFrame; }
	//
	virtual std::string GetShaderCode(const std::string& shaderName) const override;


protected:
	//
	void CreateInstance(HWND hWnd);
	//
	void PickPhysicalDevice();
	//
	void CreateLogicalDevice();
	//
	void CreateSwapChain(int width, int height);
	//
	void CreateRenderPass();
	//
	void CreateFramebuffers();
	//
	void CreateCommandPool();
	//
	void CreateTextureSampler();
	//
	void CreateSyncObjects();


protected:
	//
	VkInstance instance;
	VkSurfaceKHR surface;
	//
	VkPhysicalDevice physicalDevice;
	VkDevice device;
	//
	VkRenderPass renderPass;
	VkQueue graphicsQueue;
	VkQueue presentQueue;
	VkSampler textureSampler;
	//
	VkCommandPool commandPool;
	std::vector<VkCommandBuffer> commandBuffers;
	//
	VkSwapchainKHR swapChain;
	std::vector<VkImage> swapChainImages;
	VkFormat swapChainImageFormat;
	VkExtent2D swapChainExtent;
	std::vector<VkImageView> swapChainImageViews;
	std::vector<VkFramebuffer> swapChainFramebuffers;
	//
	std::vector<VkSemaphore> imageAvailableSemaphores;
	std::vector<VkSemaphore> renderFinishedSemaphores;
	std::vector<VkFence> inFlightFences;


protected:
	//
	TVisual2DRenderList visual2DRenders;
	//
	LcDescriptorSetsVulkan descriptorSets;
	//
	LcTextureLoaderVulkan texLoader;
	//
	LcUpdateCounters texLoaderCounters;
	//
	LcCameraManager cameraManager;
	//
	TVFeaturesList prevSpriteFeatures;
	//
	uint32_t currentFrame;
	//
	LcSize renderSystemSize;
	//
	bool isInitialized;
	//
	bool worldScaleFonts;
	//
	bool prevSetupRequested;

};
