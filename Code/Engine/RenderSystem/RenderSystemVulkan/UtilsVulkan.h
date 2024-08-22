/**
* UtilsVulkan.h
* 21.08.2024
* (c) Denis Romakhov
*/

#pragma once

#include <vulkan/vulkan.h>
#include <string>
#include <map>
#include <optional>

#include "World/WorldInterface.h"
#include "World/SpriteInterface.h"
#include "GUI/WidgetInterface.h"
#include "Core/LCTypesEx.h"


static const int MAX_FRAMES_IN_FLIGHT = 2;

static const std::vector<const char*> DeviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };

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


/**
* Texture loader */
class LcTextureLoaderVulkan
{
public:
	//
	LcTextureLoaderVulkan(class IRenderDeviceVulkan& inRender);
	//
	~LcTextureLoaderVulkan();
	//
	bool LoadTexture(const char* texPath, VkImage* outImage, VkDeviceMemory* outImageMemory, VkImageView* outImageView, LcSize* outTexSize);
	//
	void RemoveTextures() { texturesCache.clear(); }
	/** If world is not null - only unused textures removed. If null - all textures removed. */
	void ClearCache(IWorld* world);
	//
	inline int GetNumTextures() const { return (int)texturesCache.size(); }


protected:
	//
	VkCommandBuffer BeginSingleTimeCommands();
	//
	void EndSingleTimeCommands(VkCommandBuffer commandBuffer);
	//
	void TransitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout);
    //
    void CopyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);


protected:
	//
	struct LcTextureDataVulkan
	{
		LcTextureDataVulkan();
		//
		~LcTextureDataVulkan();
		//
		VkImage image;
		//
		VkDeviceMemory imageMemory;
		//
		VkImageView imageView;
		//
		LcSize size;
	};
	//
	static VkDevice deviceInstance;
	//
	class IRenderDeviceVulkan& render;
	//
	std::map<std::string, LcTextureDataVulkan> texturesCache;

};
