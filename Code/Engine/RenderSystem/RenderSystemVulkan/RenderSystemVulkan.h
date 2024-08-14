/**
* RenderSystemVulkan.h
* 14.08.2024
* (c) Denis Romakhov
*/

#pragma once

#include <deque>
#include <vulkan/vulkan.h>

#include "Module.h"
#include "World/WorldInterface.h"

#pragma warning(disable : 4251)
#pragma warning(disable : 5046)

typedef std::deque<std::shared_ptr<IVisual2DRender>> TVisual2DRenderList;


/**
* Render device */
class RENDERSYSTEMVULKAN_API IRenderDeviceVulkan
{
public:
	/**
	* Return Vulkan device */
	virtual void* GetD3D10Device() const = 0;
	/**
	* Get sprite renders */
	virtual TVisual2DRenderList& GetVisual2DRenderList() = 0;
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
	virtual bool CanRender() const override { return imageAvailableSemaphore != nullptr; }
	//
	virtual LcRSStats GetStats() const override;
	//
	virtual LcRenderSystemType GetType() const override { return LcRenderSystemType::DX10; }


protected:// LcRenderSystemBase interface implementation
	//
	virtual void Render(const IVisual* visual, const LcAppContext& context) override;


public:// IRenderDeviceVulkan interface implementation
	//
	virtual void* GetD3D10Device() const override { return nullptr; }
	//
	virtual TVisual2DRenderList& GetVisual2DRenderList() override { return visual2DRenders; }
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
	void CreateGraphicsPipeline();
	//
	void CreateFramebuffers();
	//
	void CreateCommandPool();
	//
	void CreateSyncObjects();
	//
	void RecordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex);


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
	VkPipelineLayout pipelineLayout;
	VkPipeline graphicsPipeline;
	//
	VkCommandPool commandPool;
	VkCommandBuffer commandBuffer;
	//
	VkSemaphore imageAvailableSemaphore;
	VkSemaphore renderFinishedSemaphore;
	VkFence inFlightFence;
	//
	TVisual2DRenderList visual2DRenders;
	//
	TVFeaturesList prevSpriteFeatures;
	//
	LcSize renderSystemSize;
	//
	LcVector3 worldScale;
	//
	bool worldScaleFonts;
	//
	bool prevSetupRequested;

};
