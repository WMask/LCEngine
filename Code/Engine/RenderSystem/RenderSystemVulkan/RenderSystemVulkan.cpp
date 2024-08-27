/**
* RenderSystemVulkan.cpp
* 14.08.2024
* (c) Denis Romakhov
*/

#include "RenderSystem/RenderSystemVulkan/RenderSystemVulkan.h"
#include "RenderSystem/RenderSystemVulkan/ColoredSpriteRenderVulkan.h"
#include "RenderSystem/RenderSystemVulkan/TexturedVisual2DRenderVulkan.h"
#include "Application/ApplicationInterface.h"
#include "World/World.h"
#include "World/Camera.h"
#include "GUI/GuiManager.h"
#include "Core/LCException.h"

#ifdef max
#undef max
#endif


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
	, commandPool(VK_NULL_HANDLE), textureSampler(VK_NULL_HANDLE)
	, swapChainExtent{}, swapChainImageFormat(VkFormat::VK_FORMAT_UNDEFINED)
	, isInitialized(false), currentFrame(0)
	, prevSetupRequested(false)
	, worldScaleFonts(false)
	, constBuffers(*this)
	, texLoader(*this)
{
}

LcRenderSystemVulkan::~LcRenderSystemVulkan()
{
	Shutdown();
}

void LcRenderSystemVulkan::Shutdown()
{
	if (isInitialized)
	{
		isInitialized = false;

		for (auto framebuffer : swapChainFramebuffers)
		{
			vkDestroyFramebuffer(device, framebuffer, nullptr);
		}

		for (auto imageView : swapChainImageViews)
		{
			vkDestroyImageView(device, imageView, nullptr);
		}

		if (swapChain) vkDestroySwapchainKHR(device, swapChain, nullptr);

		// destroy texture images
		texLoader.RemoveTextures();

		// destroy visual renders (shader pipelines)
		visual2DRenders.clear();

		// destroy uniform descriptor pool and layout
		constBuffers.Destroy(device);

		vkDestroySampler(device, textureSampler, nullptr);

		if (renderPass) vkDestroyRenderPass(device, renderPass, nullptr);

		for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
		{
			vkDestroySemaphore(device, renderFinishedSemaphores[i], nullptr);
			vkDestroySemaphore(device, imageAvailableSemaphores[i], nullptr);
			vkDestroyFence(device, inFlightFences[i], nullptr);
		}

		if (commandPool) vkDestroyCommandPool(device, commandPool, nullptr);
		vkDestroyDevice(device, nullptr);

		if (surface) vkDestroySurfaceKHR(instance, surface, nullptr);
		vkDestroyInstance(instance, nullptr);
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

	// init Vulkan
	CreateInstance(hWnd);
	PickPhysicalDevice();
	CreateLogicalDevice();
	CreateSwapChain(width, height);
	CreateRenderPass();
	CreateFramebuffers();
	CreateCommandPool();
	CreateTextureSampler();
	CreateSyncObjects();

	// set uniforms
	constBuffers.Create();
	constBuffers.LookAt({ width / 2.0f, height / 2.0f, 0.0f }, false);
	constBuffers.SetOrtho(width, height);

	// add visual renders
	visual2DRenders.push_back(std::make_unique<LcColoredSpriteRenderVulkan>(*this, context));
	visual2DRenders.push_back(std::make_unique<LcTexturedVisual2DRenderVulkan>(*this, context));

	isInitialized = true;

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

	uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 0;
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

	commandBuffers.resize(MAX_FRAMES_IN_FLIGHT);

	VkCommandBufferAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.commandPool = commandPool;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandBufferCount = (uint32_t)commandBuffers.size();

	result = vkAllocateCommandBuffers(device, &allocInfo, commandBuffers.data());
	if (result != VK_SUCCESS)
	{
		throw LcException("Failed to allocate command buffers");
	}

	LC_CATCH{ LC_THROW("LcRenderSystemVulkan::CreateCommandPool()") }
}

void LcRenderSystemVulkan::CreateTextureSampler()
{
	VkPhysicalDeviceProperties properties{};
	vkGetPhysicalDeviceProperties(physicalDevice, &properties);

	VkSamplerCreateInfo samplerInfo{};
	samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
	samplerInfo.magFilter = VK_FILTER_LINEAR;
	samplerInfo.minFilter = VK_FILTER_LINEAR;
	samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerInfo.anisotropyEnable = VK_TRUE;
	samplerInfo.maxAnisotropy = properties.limits.maxSamplerAnisotropy;
	samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
	samplerInfo.unnormalizedCoordinates = VK_FALSE;
	samplerInfo.compareEnable = VK_FALSE;
	samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
	samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;

	VkResult result = vkCreateSampler(device, &samplerInfo, nullptr, &textureSampler);
	if (result != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to create texture sampler");
	}
}

void LcRenderSystemVulkan::CreateSyncObjects()
{
	imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
	renderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
	inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);

	VkSemaphoreCreateInfo semaphoreInfo{};
	semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	VkFenceCreateInfo fenceInfo{};
	fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
	{
		if (vkCreateSemaphore(device, &semaphoreInfo, nullptr, &imageAvailableSemaphores[i]) != VK_SUCCESS ||
			vkCreateSemaphore(device, &semaphoreInfo, nullptr, &renderFinishedSemaphores[i]) != VK_SUCCESS ||
			vkCreateFence(device, &fenceInfo, nullptr, &inFlightFences[i]) != VK_SUCCESS)
		{
			throw LcException("Failed to create synchronization objects for a frame");
		}
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
		constBuffers.SetGlobalTint(globalTint);
	});
}

void LcRenderSystemVulkan::Update(float deltaSeconds, const LcAppContext& context)
{
	LcRenderSystemBase::Update(deltaSeconds, context);
}

void LcRenderSystemVulkan::UpdateCamera(float deltaSeconds, LcVector3 newPos, LcVector3 newTarget)
{
	constBuffers.LookAt(newPos, newTarget);
}

void LcRenderSystemVulkan::Render(const LcAppContext& context)
{
	if (!CanRender()) return;

	LC_TRY

	vkWaitForFences(device, 1, &inFlightFences[currentFrame], VK_TRUE, UINT64_MAX);

	uint32_t imageIndex;
	vkAcquireNextImageKHR(device, swapChain, UINT64_MAX, imageAvailableSemaphores[currentFrame], VK_NULL_HANDLE, &imageIndex);

	vkResetFences(device, 1, &inFlightFences[currentFrame]);

	// begin command buffer
	auto commandBuffer = commandBuffers[currentFrame];
	vkResetCommandBuffer(commandBuffer, 0);

	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	VkResult result = vkBeginCommandBuffer(commandBuffer, &beginInfo);
	if (result != VK_SUCCESS)
	{
		throw LcException("Failed to begin recording command buffer");
	}

	VkClearValue clearColor = { {{0.0f, 0.0f, 1.0f, 1.0f}} };

	VkRenderPassBeginInfo renderPassInfo{};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	renderPassInfo.renderPass = renderPass;
	renderPassInfo.framebuffer = swapChainFramebuffers[imageIndex];
	renderPassInfo.renderArea.offset = { 0, 0 };
	renderPassInfo.renderArea.extent = swapChainExtent;
	renderPassInfo.clearValueCount = 1;
	renderPassInfo.pClearValues = &clearColor;

	// begin render pass
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

	// render visuals
	LcRenderSystemBase::Render(context);
	prevSetupRequested = true;

	vkCmdEndRenderPass(commandBuffer);

	result = vkEndCommandBuffer(commandBuffer);
	if (result != VK_SUCCESS)
	{
		throw LcException("Failed to record command buffer");
	}

	// submit command buffers
	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

	VkSemaphore waitSemaphores[] = { imageAvailableSemaphores[currentFrame] };
	VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = waitSemaphores;
	submitInfo.pWaitDstStageMask = waitStages;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &commandBuffers[currentFrame];

	VkSemaphore signalSemaphores[] = { renderFinishedSemaphores[currentFrame] };
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = signalSemaphores;

	if (vkQueueSubmit(graphicsQueue, 1, &submitInfo, inFlightFences[currentFrame]) != VK_SUCCESS)
	{
		throw LcException("Failed to submit draw command buffer");
	}

	// present to screen
	VkPresentInfoKHR presentInfo{};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = signalSemaphores;

	VkSwapchainKHR swapChains[] = { swapChain };
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = swapChains;
	presentInfo.pImageIndices = &imageIndex;

	result = vkQueuePresentKHR(presentQueue, &presentInfo);
	if (result != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to present swap chain image");
	}

	currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;

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

	if (!visual)
	{
		throw LcException("LcRenderSystemVulkan::Render(): Invalid visual");
	}

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
	std::string shaderCode;

	LC_TRY

	auto shaderCodeIt = shaders.find(shaderName);
	if (shaderCodeIt == shaders.end())
	{
		throw LcException("Failed to find shader code");
	}

	shaderCode = shaderCodeIt->second;

	LC_CATCH{ LC_THROW_EX("LcRenderSystemVulkan::GetShaderCode('", shaderName.c_str(), "')") }

	return shaderCode;
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

void LcRenderSystemVulkan::FillPipelineDefaults(VkGraphicsPipelineCreateInfo& pipeline)
{
	static VkPipelineRasterizationStateCreateInfo rasterizer{};
	static VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
	static VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
	static VkPipelineViewportStateCreateInfo viewportState{};
	static VkPipelineMultisampleStateCreateInfo multisampling{};
	static VkPipelineColorBlendAttachmentState colorBlendAttachment{};
	static VkPipelineColorBlendStateCreateInfo colorBlending{};
	static VkPipelineDynamicStateCreateInfo dynamicState{};
	static std::vector<VkDynamicState> dynamicStates = {
		VK_DYNAMIC_STATE_VIEWPORT,
		VK_DYNAMIC_STATE_SCISSOR
	};

	if (rasterizer.sType == 0)
	{
		rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
		rasterizer.depthClampEnable = VK_FALSE;
		rasterizer.rasterizerDiscardEnable = VK_FALSE;
		rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
		rasterizer.lineWidth = 1.0f;
		rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
		rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
		rasterizer.depthBiasEnable = VK_FALSE;

		vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
		vertexInputInfo.vertexBindingDescriptionCount = 0;
		vertexInputInfo.vertexAttributeDescriptionCount = 0;

		inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_FAN;
		inputAssembly.primitiveRestartEnable = VK_FALSE;

		viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		viewportState.viewportCount = 1;
		viewportState.scissorCount = 1;

		multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		multisampling.sampleShadingEnable = VK_FALSE;
		multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

		colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
		colorBlendAttachment.blendEnable = VK_FALSE;

		colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		colorBlending.logicOpEnable = VK_FALSE;
		colorBlending.logicOp = VK_LOGIC_OP_COPY;
		colorBlending.attachmentCount = 1;
		colorBlending.pAttachments = &colorBlendAttachment;
		colorBlending.blendConstants[0] = 0.0f;
		colorBlending.blendConstants[1] = 0.0f;
		colorBlending.blendConstants[2] = 0.0f;
		colorBlending.blendConstants[3] = 0.0f;

		dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
		dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
		dynamicState.pDynamicStates = dynamicStates.data();
	}

	pipeline.pVertexInputState = &vertexInputInfo;
	pipeline.pInputAssemblyState = &inputAssembly;
	pipeline.pViewportState = &viewportState;
	pipeline.pRasterizationState = &rasterizer;
	pipeline.pMultisampleState = &multisampling;
	pipeline.pColorBlendState = &colorBlending;
	pipeline.pDynamicState = &dynamicState;
}


TRenderSystemPtr GetRenderSystem()
{
	return std::make_unique<LcRenderSystemVulkan>();
}
