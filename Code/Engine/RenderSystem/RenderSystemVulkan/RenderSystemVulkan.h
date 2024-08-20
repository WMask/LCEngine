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
#include "RenderSystem/RenderSystemVulkan/UniformsVulkan.h"
#include "World/WorldInterface.h"

#pragma warning(disable : 4251)
#pragma warning(disable : 5046)

typedef std::deque<std::shared_ptr<IVisual2DRender>> TVisual2DRenderList;


/**
* Render device */
class IRenderDeviceVulkan
{
public:
	/**
	* Create shader module */
	virtual VkShaderModule CreateShaderModule(const std::vector<uint32_t>& code) = 0;
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
	* Get sprite renders */
	virtual TVisual2DRenderList& GetVisual2DRenderList() = 0;
	/**
	* Get uniforms */
	virtual const LcUniformsVulkan& GetUniforms() const = 0;
	/**
	* Get uniforms */
	virtual LcUniformsVulkan& GetUniforms() = 0;
	/**
	* Get current image */
	virtual uint32_t GetCurrentImage() const = 0;
	/**
	* Get shader code */
	virtual std::string GetShaderCode(const std::string& shaderName) const = 0;

};


/**
* Vulkan render system */
class RENDERSYSTEMVULKAN_API LcRenderSystemVulkan
	: public LcRenderSystemBase
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
	virtual bool CanRender() const override { return device != nullptr && imageAvailableSemaphore != nullptr; }
	//
	virtual LcRSStats GetStats() const override;
	//
	virtual LcRenderSystemType GetType() const override { return LcRenderSystemType::DX10; }


protected:// LcRenderSystemBase interface implementation
	//
	virtual void Render(const IVisual* visual, const LcAppContext& context) override;


public:// IRenderDeviceVulkan interface implementation
	//
	virtual VkShaderModule CreateShaderModule(const std::vector<uint32_t>& code) override;
	//
	virtual VkDevice GetVulkanDevice() const override { return device; }
	//
	virtual VkPhysicalDevice GetPhysicalDevice() const override { return physicalDevice; }
	//
	virtual VkCommandBuffer GetCommandBuffer() const override { return commandBuffer; }
	//
	virtual VkRenderPass GetRenderPass() const override { return renderPass; }
	//
	virtual VkCommandPool GetCommandPool() const override { return commandPool; }
	//
	virtual TVisual2DRenderList& GetVisual2DRenderList() override { return visual2DRenders; }
	//
	virtual const LcUniformsVulkan& GetUniforms() const override { return uniforms; }
	//
	virtual LcUniformsVulkan& GetUniforms() override { return uniforms; }
	//
	virtual uint32_t GetCurrentImage() const override { return currentImageIndex; }
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
	void CreateSyncObjects();


protected:
	//
	VkInstance instance;
	VkSurfaceKHR surface;
	//
	VkPhysicalDevice physicalDevice;
	VkDevice device;
	//
	VkQueue graphicsQueue;
	VkQueue presentQueue;
	//
	VkSwapchainKHR swapChain;
	std::vector<VkImage> swapChainImages;
	VkFormat swapChainImageFormat;
	VkExtent2D swapChainExtent;
	std::vector<VkImageView> swapChainImageViews;
	std::vector<VkFramebuffer> swapChainFramebuffers;
	//
	VkRenderPass renderPass;
	//
	VkCommandPool commandPool;
	VkCommandBuffer commandBuffer;
	//
	VkSemaphore imageAvailableSemaphore;
	VkSemaphore renderFinishedSemaphore;
	VkFence inFlightFence;
	//
	uint32_t currentImageIndex;
	//
	TVisual2DRenderList visual2DRenders;
	//
	TVFeaturesList prevSpriteFeatures;
	//
	LcSize renderSystemSize;
	//
	LcUniformsVulkan uniforms;
	//
	bool worldScaleFonts;
	//
	bool prevSetupRequested;

};
