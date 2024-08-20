/**
* RenderSystemVulkan.cpp
* 14.08.2024
* (c) Denis Romakhov
*/

#include "RenderSystem/RenderSystemVulkan/RenderSystemVulkan.h"
#include "RenderSystem/RenderSystemVulkan/ColoredSpriteRenderVulkan.h"
#include "Application/ApplicationInterface.h"
#include "World/World.h"
#include "World/Camera.h"
#include "GUI/GuiManager.h"
#include "Core/LCException.h"
#include <optional>

#ifdef max
#undef max
#endif

static const int MAX_FRAMES_IN_FLIGHT = 2;

static const std::vector<const char*> DeviceExtensions = {
	VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

struct QueueFamilyIndices
{
	std::optional<uint32_t> graphicsFamily;
	std::optional<uint32_t> presentFamily;

	bool isComplete() { return graphicsFamily.has_value() && presentFamily.has_value(); }
};

struct SwapChainSupportDetails
{
	VkSurfaceCapabilitiesKHR capabilities;
	std::vector<VkSurfaceFormatKHR> formats;
	std::vector<VkPresentModeKHR> presentModes;
};

std::vector<const char*> GetRequiredExtensions();
bool CheckDeviceExtensionSupport(VkPhysicalDevice device);
QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice device, VkSurfaceKHR surface);
SwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice device, VkSurfaceKHR surface);
bool IsDeviceSuitable(VkPhysicalDevice device, VkSurfaceKHR surface);
VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
VkPresentModeKHR ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities, int width, int height);


class LcVisual2DLifetimeStrategyVulkan : public LcLifetimeStrategy<IVisual, IWorld::TVisualSet>
{
public:
	LcVisual2DLifetimeStrategyVulkan() {}
	//
	virtual ~LcVisual2DLifetimeStrategyVulkan() {}
	//
	virtual std::shared_ptr<IVisual> Create() override
	{
		std::shared_ptr<IVisual> newVisual;

		/*switch (curTypeId)
		{
		case LcCreatables::Sprite: newVisual = std::make_shared<LcSpriteDX10>(); break;
		case LcCreatables::Widget: newVisual = std::make_shared<LcWidgetDX10>(); break;
		}*/

		// add layer Z for initial valid sorting in multiset
		newVisual->SetPos(LcVector3{ 0.0f, 0.0f, layer2D });

		return newVisual;
	}
	//
	virtual void Destroy(IVisual& item, IWorld::TVisualSet& items) override {}
};


LcRenderSystemVulkan::LcRenderSystemVulkan()
	: instance(VK_NULL_HANDLE), surface(VK_NULL_HANDLE)
	, physicalDevice(VK_NULL_HANDLE), device(VK_NULL_HANDLE)
	, graphicsQueue(VK_NULL_HANDLE), presentQueue(VK_NULL_HANDLE)
	, swapChain(VK_NULL_HANDLE), renderPass(VK_NULL_HANDLE)
	, commandPool(VK_NULL_HANDLE), commandBuffer(VK_NULL_HANDLE)
	, imageAvailableSemaphore(VK_NULL_HANDLE)
	, renderFinishedSemaphore(VK_NULL_HANDLE)
	, inFlightFence(VK_NULL_HANDLE), swapChainExtent{}
	, swapChainImageFormat(VkFormat::VK_FORMAT_UNDEFINED)
	, prevSetupRequested(false)
	, worldScaleFonts(false)
	, uniforms(*this)
{
}

LcRenderSystemVulkan::~LcRenderSystemVulkan()
{
	Shutdown();
}

void LcRenderSystemVulkan::Shutdown()
{
	auto localInstance = instance;
	auto localDevice = device;
	instance = nullptr;
	device = nullptr;

	if (localDevice)
	{
		auto localImageAvailableSemaphore = imageAvailableSemaphore;
		imageAvailableSemaphore = nullptr;

		for (auto framebuffer : swapChainFramebuffers)
		{
			vkDestroyFramebuffer(localDevice, framebuffer, nullptr);
		}

		for (auto imageView : swapChainImageViews)
		{
			vkDestroyImageView(localDevice, imageView, nullptr);
		}

		if (swapChain) vkDestroySwapchainKHR(localDevice, swapChain, nullptr);

		// destroy visual renders (shader pipelines)
		visual2DRenders.clear();

		if (renderPass) vkDestroyRenderPass(localDevice, renderPass, nullptr);

		uniforms.Destroy(localDevice);

		if (renderFinishedSemaphore) vkDestroySemaphore(localDevice, renderFinishedSemaphore, nullptr);
		if (localImageAvailableSemaphore) vkDestroySemaphore(localDevice, localImageAvailableSemaphore, nullptr);
		if (inFlightFence) vkDestroyFence(localDevice, inFlightFence, nullptr);

		if (commandPool) vkDestroyCommandPool(localDevice, commandPool, nullptr);
		vkDestroyDevice(localDevice, nullptr);
	}

	if (localInstance)
	{
		if (surface) vkDestroySurfaceKHR(localInstance, surface, nullptr);
		vkDestroyInstance(localInstance, nullptr);
	}

	LcRenderSystemBase::Shutdown();
}

void LcRenderSystemVulkan::Create(void* windowHandle, LcWinMode winMode, bool inVSync, bool inAllowFullscreen, const LcAppContext& context)
{
	LC_TRY

	HWND hWnd = (HWND)windowHandle;
	RECT clientRect;
	GetClientRect(hWnd, &clientRect);

	int width = clientRect.right - clientRect.left;
	int height = clientRect.bottom - clientRect.top;

	CreateInstance(hWnd);
	PickPhysicalDevice();
	CreateLogicalDevice();
	CreateSwapChain(width, height);
	CreateRenderPass();
	CreateFramebuffers();
	CreateCommandPool();
	CreateSyncObjects();

	uniforms.Create(MAX_FRAMES_IN_FLIGHT);
	uniforms.LookAt({ width / 2.0f, height / 2.0f, 0.0f }, false);
	uniforms.SetOrtho(width, height);

	visual2DRenders.push_back(std::make_shared<LcColoredSpriteRenderVulkan>(*this, context));

	LC_CATCH{ LC_THROW("LcRenderSystemVulkan::Create()") }
}

void LcRenderSystemVulkan::CreateInstance(HWND hWnd)
{
	LC_TRY

	VkApplicationInfo appInfo{};
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pApplicationName = "LcEngine App";
	appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.pEngineName = "LcEngine";
	appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.apiVersion = VK_API_VERSION_1_0;

	VkInstanceCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pApplicationInfo = &appInfo;

	auto extensions = GetRequiredExtensions();
	createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
	createInfo.ppEnabledExtensionNames = extensions.data();
	createInfo.enabledLayerCount = 0;
	createInfo.pNext = nullptr;

	VkResult result = vkCreateInstance(&createInfo, nullptr, &instance);
	if (result != VK_SUCCESS)
	{
		throw LcException("Failed to create instance");
	}

	VkWin32SurfaceCreateInfoKHR surfaceCreateInfo{};
	surfaceCreateInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
	surfaceCreateInfo.hwnd = hWnd;
	surfaceCreateInfo.hinstance = GetModuleHandle(nullptr);

	result = vkCreateWin32SurfaceKHR(instance, &surfaceCreateInfo, nullptr, &surface);
	if (result != VK_SUCCESS)
	{
		throw LcException("Failed to create window surface");
	}

	LC_CATCH{ LC_THROW("LcRenderSystemVulkan::CreateInstance()") }
}

void LcRenderSystemVulkan::PickPhysicalDevice()
{
	LC_TRY

	uint32_t deviceCount = 0;
	vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);
	if (deviceCount == 0)
	{
		throw LcException("Failed to find GPUs with Vulkan support");
	}

	std::vector<VkPhysicalDevice> devices(deviceCount);
	vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

	for (const auto& device : devices)
	{
		if (IsDeviceSuitable(device, surface))
		{
			physicalDevice = device;
			break;
		}
	}

	if (physicalDevice == VK_NULL_HANDLE)
	{
		throw LcException("Failed to find a suitable GPU");
	}

	LC_CATCH{ LC_THROW("LcRenderSystemVulkan::PickPhysicalDevice()") }
}

void LcRenderSystemVulkan::CreateLogicalDevice()
{
	LC_TRY

	QueueFamilyIndices indices = FindQueueFamilies(physicalDevice, surface);

	std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
	std::set<uint32_t> uniqueQueueFamilies = { indices.graphicsFamily.value(), indices.presentFamily.value() };

	float queuePriority = 1.0f;
	for (uint32_t queueFamily : uniqueQueueFamilies)
	{
		VkDeviceQueueCreateInfo queueCreateInfo{};
		queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queueCreateInfo.queueFamilyIndex = queueFamily;
		queueCreateInfo.queueCount = 1;
		queueCreateInfo.pQueuePriorities = &queuePriority;
		queueCreateInfos.push_back(queueCreateInfo);
	}

	VkPhysicalDeviceFeatures deviceFeatures{};

	VkDeviceCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

	createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
	createInfo.pQueueCreateInfos = queueCreateInfos.data();
	createInfo.pEnabledFeatures = &deviceFeatures;
	createInfo.enabledExtensionCount = static_cast<uint32_t>(DeviceExtensions.size());
	createInfo.ppEnabledExtensionNames = DeviceExtensions.data();
	createInfo.enabledLayerCount = 0;

	VkResult result = vkCreateDevice(physicalDevice, &createInfo, nullptr, &device);
	if (result != VK_SUCCESS)
	{
		throw LcException("Failed to create logical device");
	}

	vkGetDeviceQueue(device, indices.graphicsFamily.value(), 0, &graphicsQueue);
	vkGetDeviceQueue(device, indices.presentFamily.value(), 0, &presentQueue);

	LC_CATCH{ LC_THROW("LcRenderSystemVulkan::CreateLogicalDevice()") }
}

void LcRenderSystemVulkan::CreateSwapChain(int width, int height)
{
	LC_TRY

	SwapChainSupportDetails swapChainSupport = QuerySwapChainSupport(physicalDevice, surface);

	VkSurfaceFormatKHR surfaceFormat = ChooseSwapSurfaceFormat(swapChainSupport.formats);
	VkPresentModeKHR presentMode = ChooseSwapPresentMode(swapChainSupport.presentModes);
	VkExtent2D extent = ChooseSwapExtent(swapChainSupport.capabilities, width, height);

	uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
	if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount)
	{
		imageCount = swapChainSupport.capabilities.maxImageCount;
	}

	VkSwapchainCreateInfoKHR createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	createInfo.surface = surface;

	createInfo.minImageCount = imageCount;
	createInfo.imageFormat = surfaceFormat.format;
	createInfo.imageColorSpace = surfaceFormat.colorSpace;
	createInfo.imageExtent = extent;
	createInfo.imageArrayLayers = 1;
	createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

	QueueFamilyIndices indices = FindQueueFamilies(physicalDevice, surface);
	uint32_t queueFamilyIndices[] = { indices.graphicsFamily.value(), indices.presentFamily.value() };
	if (indices.graphicsFamily != indices.presentFamily)
	{
		createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
		createInfo.queueFamilyIndexCount = 2;
		createInfo.pQueueFamilyIndices = queueFamilyIndices;
	}
	else
	{
		createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
	}

	createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
	createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	createInfo.presentMode = presentMode;
	createInfo.clipped = VK_TRUE;
	createInfo.oldSwapchain = VK_NULL_HANDLE;

	VkResult result = vkCreateSwapchainKHR(device, &createInfo, nullptr, &swapChain);
	if (result != VK_SUCCESS)
	{
		throw LcException("Failed to create swap chain");
	}

	vkGetSwapchainImagesKHR(device, swapChain, &imageCount, nullptr);
	swapChainImages.resize(imageCount);
	vkGetSwapchainImagesKHR(device, swapChain, &imageCount, swapChainImages.data());

	swapChainImageFormat = surfaceFormat.format;
	swapChainExtent = extent;
	swapChainImageViews.resize(swapChainImages.size());

	for (size_t i = 0; i < swapChainImages.size(); i++)
	{
		VkImageViewCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		createInfo.image = swapChainImages[i];
		createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		createInfo.format = swapChainImageFormat;
		createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		createInfo.subresourceRange.baseMipLevel = 0;
		createInfo.subresourceRange.levelCount = 1;
		createInfo.subresourceRange.baseArrayLayer = 0;
		createInfo.subresourceRange.layerCount = 1;

		result = vkCreateImageView(device, &createInfo, nullptr, &swapChainImageViews[i]);
		if (result != VK_SUCCESS)
		{
			throw LcException("Failed to create image view");
		}
	}

	LC_CATCH{ LC_THROW("LcRenderSystemVulkan::CreateSwapChain()") }
}

void LcRenderSystemVulkan::CreateRenderPass()
{
	LC_TRY

	VkAttachmentDescription colorAttachment{};
	colorAttachment.format = swapChainImageFormat;
	colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
	colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

	VkAttachmentReference colorAttachmentRef{};
	colorAttachmentRef.attachment = 0;
	colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	VkSubpassDescription subpass{};
	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass.colorAttachmentCount = 1;
	subpass.pColorAttachments = &colorAttachmentRef;

	VkSubpassDependency dependency{};
	dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
	dependency.dstSubpass = 0;
	dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency.srcAccessMask = 0;
	dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

	VkRenderPassCreateInfo renderPassInfo{};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	renderPassInfo.attachmentCount = 1;
	renderPassInfo.pAttachments = &colorAttachment;
	renderPassInfo.subpassCount = 1;
	renderPassInfo.pSubpasses = &subpass;
	renderPassInfo.dependencyCount = 1;
	renderPassInfo.pDependencies = &dependency;

	VkResult result = vkCreateRenderPass(device, &renderPassInfo, nullptr, &renderPass);
	if (result != VK_SUCCESS)
	{
		throw LcException("Failed to create render pass");
	}

	LC_CATCH{ LC_THROW("LcRenderSystemVulkan::CreateRenderPass()") }
}

void LcRenderSystemVulkan::CreateFramebuffers()
{
	swapChainFramebuffers.resize(swapChainImageViews.size());

	for (size_t i = 0; i < swapChainImageViews.size(); i++)
	{
		VkImageView attachments[] = { swapChainImageViews[i] };

		VkFramebufferCreateInfo framebufferInfo{};
		framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferInfo.renderPass = renderPass;
		framebufferInfo.attachmentCount = 1;
		framebufferInfo.pAttachments = attachments;
		framebufferInfo.width = swapChainExtent.width;
		framebufferInfo.height = swapChainExtent.height;
		framebufferInfo.layers = 1;

		VkResult result = vkCreateFramebuffer(device, &framebufferInfo, nullptr, &swapChainFramebuffers[i]);
		if (result != VK_SUCCESS)
		{
			throw LcException("Failed to create framebuffer");
		}
	}
}

void LcRenderSystemVulkan::CreateCommandPool()
{
	LC_TRY

	QueueFamilyIndices queueFamilyIndices = FindQueueFamilies(physicalDevice, surface);

	VkCommandPoolCreateInfo poolInfo{};
	poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
	poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();

	VkResult result = vkCreateCommandPool(device, &poolInfo, nullptr, &commandPool);
	if (result != VK_SUCCESS)
	{
		throw LcException("Failed to create command pool");
	}

	VkCommandBufferAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.commandPool = commandPool;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandBufferCount = 1;

	result = vkAllocateCommandBuffers(device, &allocInfo, &commandBuffer);
	if (result != VK_SUCCESS)
	{
		throw LcException("Failed to allocate command buffers");
	}

	LC_CATCH{ LC_THROW("LcRenderSystemVulkan::CreateCommandPool()") }
}

void LcRenderSystemVulkan::CreateSyncObjects()
{
	VkSemaphoreCreateInfo semaphoreInfo{};
	semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	VkFenceCreateInfo fenceInfo{};
	fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

	if (vkCreateSemaphore(device, &semaphoreInfo, nullptr, &imageAvailableSemaphore) != VK_SUCCESS ||
		vkCreateSemaphore(device, &semaphoreInfo, nullptr, &renderFinishedSemaphore) != VK_SUCCESS ||
		vkCreateFence(device, &fenceInfo, nullptr, &inFlightFence) != VK_SUCCESS)
	{
		throw LcException("Failed to create synchronization objects for a frame");
	}
}

void LcRenderSystemVulkan::Clear(IWorld* world, bool removeRooted)
{
}

void LcRenderSystemVulkan::Subscribe(const LcAppContext& context)
{
	auto contextPtr = &context;

	context.world->GetWorldScale().onScaleChanged.AddListener([this, contextPtr](LcVector2 newScale)
	{
		LC_TRY

		if (contextPtr->world->GetWorldScale().GetScaleFonts())
		{
			auto& visuals = contextPtr->world->GetVisuals();
			for (auto& visual : visuals)
			{
				if (visual->GetTypeId() == LcCreatables::Widget)
				{
					//auto widget = static_cast<IWidget*>(visual.get());
					//widget->RecreateFont(*contextPtr);
				}
			}
		}

		LC_CATCH{ LC_THROW("LcRenderSystemVulkan::worldScaleUpdated()") }
	});

	context.world->onTintChanged.AddListener([this](LcColor3 globalTint)
	{
		uniforms.SetGlobalTint(globalTint);
	});
}

void LcRenderSystemVulkan::Update(float deltaSeconds, const LcAppContext& context)
{
	LcRenderSystemBase::Update(deltaSeconds, context);
}

void LcRenderSystemVulkan::UpdateCamera(float deltaSeconds, LcVector3 newPos, LcVector3 newTarget)
{
	uniforms.LookAt(newPos, newTarget);
}

void LcRenderSystemVulkan::Render(const LcAppContext& context)
{
	LC_TRY

	if (!CanRender())
	{
		throw LcException("Can't render");
	}

	vkWaitForFences(device, 1, &inFlightFence, VK_TRUE, UINT64_MAX);
	vkResetFences(device, 1, &inFlightFence);

	vkAcquireNextImageKHR(device, swapChain, UINT64_MAX, imageAvailableSemaphore, VK_NULL_HANDLE, &currentImageIndex);

	// Begin command buffer
	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

	VkResult result = vkBeginCommandBuffer(commandBuffer, &beginInfo);
	if (result != VK_SUCCESS)
	{
		throw LcException("Failed to begin recording command buffer");
	}

	VkClearValue clearColor = { {{0.0f, 0.0f, 1.0f, 0.0f}} };

	VkRenderPassBeginInfo renderPassInfo{};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	renderPassInfo.renderPass = renderPass;
	renderPassInfo.framebuffer = swapChainFramebuffers[currentImageIndex];
	renderPassInfo.renderArea.offset = { 0, 0 };
	renderPassInfo.renderArea.extent = swapChainExtent;
	renderPassInfo.clearValueCount = 1;
	renderPassInfo.pClearValues = &clearColor;

	// Begin render pass
	vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

	VkViewport viewport{};
	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = static_cast<float>(swapChainExtent.width);
	viewport.height = static_cast<float>(swapChainExtent.height);
	viewport.minDepth = -1.0f;
	viewport.maxDepth = 1.0f;
	vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

	VkRect2D scissor{};
	scissor.offset = { 0, 0 };
	scissor.extent = swapChainExtent;
	vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

	// Render visuals
	LcRenderSystemBase::Render(context);

	vkCmdEndRenderPass(commandBuffer);

	result = vkEndCommandBuffer(commandBuffer);
	if (result != VK_SUCCESS)
	{
		throw LcException("Failed to record command buffer");
	}

	// Submit command buffers
	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

	VkSemaphore waitSemaphores[] = { imageAvailableSemaphore };
	VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = waitSemaphores;
	submitInfo.pWaitDstStageMask = waitStages;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &commandBuffer;

	VkSemaphore signalSemaphores[] = { renderFinishedSemaphore };
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = signalSemaphores;

	result = vkQueueSubmit(graphicsQueue, 1, &submitInfo, inFlightFence);
	if (result != VK_SUCCESS)
	{
		throw LcException("Failed to submit draw command buffer");
	}

	// Present to screen
	VkPresentInfoKHR presentInfo{};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = signalSemaphores;

	VkSwapchainKHR swapChains[] = { swapChain };
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = swapChains;
	presentInfo.pImageIndices = &currentImageIndex;

	vkQueuePresentKHR(presentQueue, &presentInfo);

	LC_CATCH{ LC_THROW("LcRenderSystemVulkan::Render()") }
}

void LcRenderSystemVulkan::RequestResize(int width, int height)
{
	LC_TRY

	/*DXGI_MODE_DESC displayModeDesc{};
	if (!LcFindDisplayMode(width, height, &displayModeDesc))
	{
		throw std::exception("LcRenderSystemVulkan::RequestResize(): Cannot find display mode");
	}

	swapChain->ResizeTarget(&displayModeDesc);*/

	LC_CATCH{ LC_THROW("LcRenderSystemVulkan::RequestResize()") }
}

void LcRenderSystemVulkan::Resize(int width, int height, const LcAppContext& context)
{
	LC_TRY

	LcSize newViewportSize{ width, height };
	bool needResize = (renderSystemSize != newViewportSize);

	if (needResize)
	{
		// recreate widget render
		//textRender->Init(context);

		// update world settings
		cameraPos = LcVector3{ width / 2.0f, height / 2.0f, 0.0f };
		cameraTarget = LcVector3{ cameraPos.x, cameraPos.y, 1.0f };

		context.world->UpdateWorldScale(newViewportSize);
		context.world->GetCamera().Set(cameraPos, cameraTarget);
		UpdateCamera(0.1f, cameraPos, cameraTarget);

		// update projection matrix
		LcMatrix4 proj = OrthoMatrix(LcSize{ width, height }, 1.0f, -1.0f);
		//d3dDevice->UpdateSubresource(constBuffers.projMatrixBuffer.Get(), 0, NULL, &proj, 0, 0);

		renderSystemSize = newViewportSize;
	}

	LC_CATCH{ LC_THROW("LcRenderSystemVulkan::Resize()") }
}

void LcRenderSystemVulkan::SetMode(LcWinMode winMode)
{
	//if (swapChain) swapChain->SetFullscreenState((winMode == LcWinMode::Fullscreen), nullptr);
}

LcRSStats LcRenderSystemVulkan::GetStats() const
{
	return LcRSStats{
		//texLoader->GetNumTextures(),
		//tiledRender ? tiledRender->GetNumTiles() : 0,
		//textRender ? textRender->GetNumFonts() : 0
	};
}

void LcRenderSystemVulkan::Render(const IVisual* visual, const LcAppContext& context)
{
	LC_TRY

	if (!visual) throw std::exception("LcRenderSystemVulkan::Render(): Invalid visual");

	for (auto& render : visual2DRenders)
	{
		if (render->Supports(visual->GetFeaturesList()))
		{
			if (prevSpriteFeatures != visual->GetFeaturesList() || prevSetupRequested)
			{
				prevSetupRequested = false;
				prevSpriteFeatures = visual->GetFeaturesList();
				render->Setup(visual, context);
			}

			render->Render(visual, context);
			break;
		}
	}

	LC_CATCH{ LC_THROW("LcRenderSystemVulkan::Render()") }
}

std::string LcRenderSystemVulkan::GetShaderCode(const std::string& shaderName) const
{
	return shaders.at(shaderName);
}

VkShaderModule LcRenderSystemVulkan::CreateShaderModule(const std::vector<uint32_t>& code)
{
	VkShaderModuleCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	createInfo.codeSize = code.size() * sizeof(uint32_t);
	createInfo.pCode = code.data();

	VkShaderModule shaderModule;
	VkResult result = vkCreateShaderModule(device, &createInfo, nullptr, &shaderModule);
	if (result != VK_SUCCESS)
	{
		throw LcException("Failed to create shader module");
	}

	return shaderModule;
}


std::vector<const char*> GetRequiredExtensions()
{
	std::vector<const char*> extensions = {
		VK_KHR_SURFACE_EXTENSION_NAME,
		VK_KHR_WIN32_SURFACE_EXTENSION_NAME
	};
	return extensions;
}

bool CheckDeviceExtensionSupport(VkPhysicalDevice device)
{
	uint32_t extensionCount;
	vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

	std::vector<VkExtensionProperties> availableExtensions(extensionCount);
	vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

	std::set<std::string> requiredExtensions(DeviceExtensions.begin(), DeviceExtensions.end());

	for (const auto& extension : availableExtensions)
	{
		requiredExtensions.erase(extension.extensionName);
	}

	return requiredExtensions.empty();
}

QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice device, VkSurfaceKHR surface)
{
	QueueFamilyIndices indices;

	uint32_t queueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

	std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

	int i = 0;
	for (const auto& queueFamily : queueFamilies)
	{
		if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
		{
			indices.graphicsFamily = i;
		}

		VkBool32 presentSupport = false;
		vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupport);

		if (presentSupport) indices.presentFamily = i;

		if (indices.isComplete())
		{
			break;
		}

		i++;
	}

	return indices;
}

SwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice device, VkSurfaceKHR surface)
{
	SwapChainSupportDetails details;
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.capabilities);

	uint32_t formatCount;
	vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr);
	if (formatCount != 0)
	{
		details.formats.resize(formatCount);
		vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, details.formats.data());
	}

	uint32_t presentModeCount;
	vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, nullptr);
	if (presentModeCount != 0)
	{
		details.presentModes.resize(presentModeCount);
		vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, details.presentModes.data());
	}

	return details;
}

bool IsDeviceSuitable(VkPhysicalDevice device, VkSurfaceKHR surface)
{
	QueueFamilyIndices indices = FindQueueFamilies(device, surface);

	bool extensionsSupported = CheckDeviceExtensionSupport(device);

	bool swapChainAdequate = false;
	if (extensionsSupported)
	{
		SwapChainSupportDetails swapChainSupport = QuerySwapChainSupport(device, surface);
		swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
	}

	return indices.isComplete() && extensionsSupported && swapChainAdequate;
}

VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats)
{
	for (const auto& availableFormat : availableFormats)
	{
		if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
		{
			return availableFormat;
		}
	}

	return availableFormats[0];
}

VkPresentModeKHR ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes)
{
	for (const auto& availablePresentMode : availablePresentModes)
	{
		if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR)
		{
			return availablePresentMode;
		}
	}

	return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities, int width, int height)
{
	if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
	{
		return capabilities.currentExtent;
	}
	else
	{
		VkExtent2D actualExtent = {
			static_cast<uint32_t>(width),
			static_cast<uint32_t>(height)
		};

		actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
		actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

		return actualExtent;
	}
}


TRenderSystemPtr GetRenderSystem()
{
	return std::make_unique<LcRenderSystemVulkan>();
}
