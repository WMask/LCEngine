/**
* UtilsVulkan.h
* 21.08.2024
* (c) Denis Romakhov
*/

#pragma once

#include <vulkan/vulkan.h>
#include <filesystem>
#include <optional>
#include <string>
#include <array>
#include <map>

#include "World/WorldInterface.h"
#include "World/SpriteInterface.h"
#include "GUI/WidgetInterface.h"
#include "Core/LCTypesEx.h"


static const uint32_t MAX_FRAMES_IN_FLIGHT = 2;
static const uint32_t HAS_COLOR            = 0;
static const uint32_t HAS_CUSTOM_UV        = 1;
static const uint32_t HAS_TEXTURE          = 2;
static const uint32_t HAS_ANIMATION        = 3;
static const uint32_t UBO_BINDING_ID       = 0;
static const uint32_t SAMPLER_BINDING_ID   = 1;
static const uint32_t TEXTURES_BINDING_ID  = 0;
static const float    VK_TRUE_F            = 1.0f;


static const std::vector<const char*> DeviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };

using LcCounterValues = std::array<uint32_t, MAX_FRAMES_IN_FLIGHT>;

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

struct LcTextureVulkan
{
	LcTextureVulkan()
		: image(VK_NULL_HANDLE)
		, imageMemory(VK_NULL_HANDLE)
		, imageView(VK_NULL_HANDLE)
		, size{}, sets{}
	{
	}
	VkImage image;
	//
	VkDeviceMemory imageMemory;
	//
	VkImageView imageView;
	//
	LcSize size;
	//
	std::array<VkDescriptorSet, MAX_FRAMES_IN_FLIGHT> sets;
};


std::vector<const char*> GetRequiredExtensions();
bool CheckDeviceExtensionSupport(VkPhysicalDevice device);
QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice device, VkSurfaceKHR surface);
SwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice device, VkSurfaceKHR surface);
bool IsDeviceSuitable(VkPhysicalDevice device, VkSurfaceKHR surface);
VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
VkPresentModeKHR ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities, int width, int height);
uint32_t FindMemoryType(VkPhysicalDevice device, uint32_t typeFilter, VkMemoryPropertyFlags properties);
void CreateBuffer(VkDevice device, VkPhysicalDevice physicalDevice, VkDeviceSize size, VkBufferUsageFlags usage,
	VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);


/**
* Texture loader */
class LcTextureLoaderVulkan : public LcUpdateCounter
{
public:
	//
	struct LcTextureDataVulkan : public LcTextureVulkan
	{
		~LcTextureDataVulkan();
	};
	using TTexturesMap = std::map<std::filesystem::path, LcTextureDataVulkan>;


public:
	//
	LcTextureLoaderVulkan(class IRenderDeviceVulkan& inRender);
	//
	~LcTextureLoaderVulkan();
	/** Loads texture or get cached texture */
	void LoadTexture(const std::filesystem::path& texPath, LcTextureVulkan& outTexture);
	//
	void RemoveTextures() { texturesCache.clear(); }
	/** If world is not null - only unused textures removed. If null - all textures removed. */
	void ClearCache(IWorld* world);
	//
	inline TTexturesMap& GetTexturesCache() { return texturesCache; }
	//
	inline const TTexturesMap& GetTexturesCache() const { return texturesCache; }
	//
	inline uint32_t GetNumTextures() const { return static_cast<uint32_t>(texturesCache.size()); }


protected:
	//
	VkCommandBuffer BeginSingleTimeCommands();
	//
	void EndSingleTimeCommands(VkCommandBuffer commandBuffer);
	//
	void TransitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout);
	//
	void CopyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);
	//
	void CreateImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage,
		VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory);


protected:
	//
	static VkDevice deviceInstance;
	//
	class IRenderDeviceVulkan& render;
	//
	TTexturesMap texturesCache;

};

/**
* Camera manager */
struct LcCameraManager : public LcUpdateCounter
{
	inline void SetView(LcVector3 newPos, LcVector3 newTarget)
	{
		pos = newPos;
		target = newTarget;
		viewCounter.UpdateCounter();
	}
	//
	inline void SetView(LcVector3 newPos)
	{
		SetView(newPos, { newPos.x, newPos.y, 1.0f });
	}
	//
	inline void SetProj(LcSize newSize)
	{
		size = newSize;
		projCounter.UpdateCounter();
	}
	//
	inline bool IsViewUpdated(uint32_t currentFrame)
	{
		return viewCounter.IsUpdated(viewCounters[currentFrame]);
	}
	//
	inline bool IsProjUpdated(uint32_t currentFrame)
	{
		return projCounter.IsUpdated(projCounters[currentFrame]);
	}
	//
	LcUpdateCounter viewCounter;
	LcUpdateCounter projCounter;
	LcCounterValues viewCounters;
	LcCounterValues projCounters;
	LcVector3 pos;
	LcVector3 target;
	LcSize size;
};
