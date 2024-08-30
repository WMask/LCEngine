/**
* TiledVisual2DRenderVulkan.cpp
* 29.08.2024
* (c) Denis Romakhov
*/

#include "RenderSystem/RenderSystemVulkan/TiledVisual2DRenderVulkan.h"
#include "RenderSystem/RenderSystemVulkan/RenderSystemVulkan.h"
#include "RenderSystem/RenderSystemVulkan/VisualsVulkan.h"
#include "Core/LCException.h"
#include "Core/LCUtils.h"
#include <shaderc/shaderc.hpp>


static const char* TiledVisual2DShaderName = "TiledVisual2d.glsl";

struct VULKANTILEDVISUALDATA
{
	LcMatrix4 mModel;
	LcColor4 colors[4];
	LcVector4 uvs[4];
};

struct VULKANTILEDVERTEX
{
	LcVector3 pos;		  // position
	LcVector2 uv;		  // UV texture coordinates

	static VkVertexInputBindingDescription GetBindingDescription()
	{
		VkVertexInputBindingDescription bindingDescription{};
		bindingDescription.binding = 0;
		bindingDescription.stride = sizeof(VULKANTILEDVERTEX);
		bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

		return bindingDescription;
	}

	static std::array<VkVertexInputAttributeDescription, 2> GetAttributeDescriptions()
	{
		std::array<VkVertexInputAttributeDescription, 2> attributeDescriptions{};

		attributeDescriptions[0].binding = 0;
		attributeDescriptions[0].location = 0;
		attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
		attributeDescriptions[0].offset = offsetof(VULKANTILEDVERTEX, pos);

		attributeDescriptions[1].binding = 0;
		attributeDescriptions[1].location = 1;
		attributeDescriptions[1].format = VK_FORMAT_R32G32_SFLOAT;
		attributeDescriptions[1].offset = offsetof(VULKANTILEDVERTEX, uv);

		return attributeDescriptions;
	}
};

LC_VULKAN_TILES_BUFFER::~LC_VULKAN_TILES_BUFFER()
{
	auto device = LcTiledVisual2DRenderVulkan::deviceInstance;
	if (device)
	{
		vkDestroyBuffer(device, buffer, nullptr);
		vkFreeMemory(device, bufferMemory, nullptr);
	}
}

VkDevice LcTiledVisual2DRenderVulkan::deviceInstance = nullptr;


LcTiledVisual2DRenderVulkan::LcTiledVisual2DRenderVulkan(IRenderDeviceVulkan& inRender, const LcAppContext& context)
	: render(inRender)
	, pipelineLayout(VK_NULL_HANDLE)
	, graphicsPipeline(VK_NULL_HANDLE)
{
	LC_TRY

	auto device = render.GetVulkanDevice();
	if (!device)
	{
		throw LcException("LcTiledVisual2DRenderVulkan(): Invalid arguments");
	}

	auto shaderText = render.GetShaderCode(TiledVisual2DShaderName);
	if (shaderText.empty())
	{
		throw LcException("Cannot find shader");
	}

	deviceInstance = device;

	shaderc::Compiler compiler;
	shaderc::CompileOptions options;

	// compile shaders
	auto fragShaderCompiled = compiler.PreprocessGlsl(shaderText, shaderc_glsl_fragment_shader, "fs.tmp", options);
	auto fragShaderAssembly = compiler.CompileGlslToSpvAssembly(shaderText, shaderc_glsl_fragment_shader, "fs.tmp", options);
	if (fragShaderAssembly.GetCompilationStatus() != shaderc_compilation_status_success)
	{
		throw LcException(fragShaderAssembly.GetErrorMessage().c_str());
	}

	std::string fragShaderAssemblyCode(fragShaderAssembly.cbegin(), fragShaderAssembly.cend());
	auto fragShaderAssembled = compiler.AssembleToSpv(fragShaderAssemblyCode);

	options.AddMacroDefinition("COMPILE_VERTEX_SHADER");

	auto vertShaderCompiled = compiler.PreprocessGlsl(shaderText, shaderc_glsl_vertex_shader, "vs.tmp", options);
	auto vertShaderAssembly = compiler.CompileGlslToSpvAssembly(shaderText, shaderc_glsl_vertex_shader, "vs.tmp", options);
	if (vertShaderAssembly.GetCompilationStatus() != shaderc_compilation_status_success)
	{
		throw LcException(vertShaderAssembly.GetErrorMessage().c_str());
	}

	std::string vertShaderAssemblyCode(vertShaderAssembly.cbegin(), vertShaderAssembly.cend());
	auto vertShaderAssembled = compiler.AssembleToSpv(vertShaderAssemblyCode);

	// create shader modules
	std::vector<uint32_t> vertShaderCode(vertShaderAssembled.cbegin(), vertShaderAssembled.cend());
	std::vector<uint32_t> fragShaderCode(fragShaderAssembled.cbegin(), fragShaderAssembled.cend());
	VkShaderModule vertShaderModule = render.CreateShaderModule(vertShaderCode);
	VkShaderModule fragShaderModule = render.CreateShaderModule(fragShaderCode);

	VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
	vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
	vertShaderStageInfo.module = vertShaderModule;
	vertShaderStageInfo.pName = "main";

	VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
	fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
	fragShaderStageInfo.module = fragShaderModule;
	fragShaderStageInfo.pName = "main";

	VkPipelineShaderStageCreateInfo shaderStages[] = { vertShaderStageInfo, fragShaderStageInfo };

	auto bindingDescription = VULKANTILEDVERTEX::GetBindingDescription();
	auto attributeDescriptions = VULKANTILEDVERTEX::GetAttributeDescriptions();

	// create vertex input
	VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
	vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	vertexInputInfo.vertexBindingDescriptionCount = 1;
	vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
	vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
	vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();

	VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
	inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	inputAssembly.primitiveRestartEnable = VK_FALSE;

	// create pipeline layout
	VkPushConstantRange pushConstantRange{};
	pushConstantRange.offset = 0;
	pushConstantRange.size = sizeof(VULKANTILEDVISUALDATA);
	pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

	std::array<VkDescriptorSetLayout, 2> layouts = {
		*render.GetDescriptorSets().GetLayoutForFrame(render.GetCurrentFrame(), LcDSLayoutType::TiledVisual),
		*render.GetDescriptorSets().GetLayoutForFrame(render.GetCurrentFrame(), LcDSLayoutType::Textures)
	};

	VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
	pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutInfo.setLayoutCount = static_cast<uint32_t>(layouts.size());
	pipelineLayoutInfo.pSetLayouts = layouts.data();
	pipelineLayoutInfo.pushConstantRangeCount = 1;
	pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;

	VkResult result = vkCreatePipelineLayout(device, &pipelineLayoutInfo, nullptr, &pipelineLayout);
	if (result != VK_SUCCESS)
	{
		throw LcException("Failed to create pipeline layout");
	}

	// create pipeline
	VkGraphicsPipelineCreateInfo pipelineInfo{};
	render.FillPipelineDefaults(pipelineInfo);

	pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	pipelineInfo.stageCount = 2;
	pipelineInfo.pStages = shaderStages;
	pipelineInfo.pVertexInputState = &vertexInputInfo;
	pipelineInfo.pInputAssemblyState = &inputAssembly;
	pipelineInfo.layout = pipelineLayout;
	pipelineInfo.renderPass = render.GetRenderPass();

	result = vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &graphicsPipeline);
	if (result != VK_SUCCESS)
	{
		throw LcException("Failed to create graphics pipeline");
	}

	// cleanup
	vkDestroyShaderModule(device, fragShaderModule, nullptr);
	vkDestroyShaderModule(device, vertShaderModule, nullptr);

	LC_CATCH{ LC_THROW("LcTiledVisual2DRenderVulkan()") }
}

LcTiledVisual2DRenderVulkan::~LcTiledVisual2DRenderVulkan()
{
	vertexBuffers.clear();

	auto device = render.GetVulkanDevice();
	if (device)
	{
		if (graphicsPipeline) vkDestroyPipeline(device, graphicsPipeline, nullptr);
		if (pipelineLayout) vkDestroyPipelineLayout(device, pipelineLayout, nullptr);
	}
}

void LcTiledVisual2DRenderVulkan::ClearCache(IWorld* world)
{
    LC_TRY

    if (world)
    {
        std::set<const IVisual*> aliveBufList;
        auto& visuals = world->GetVisuals();
        for (auto visual : visuals)
        {
			auto sprite = static_cast<ISprite*>(visual.get());
			auto tiledComp = sprite ? sprite->GetTiledComponent() : nullptr;
            if (tiledComp)
            {
				aliveBufList.insert(sprite);
            }
        }

        std::set<const IVisual*> eraseBufList;
        for (auto buf : vertexBuffers)
        {
            if (aliveBufList.find(buf.first) == aliveBufList.end())
            {
				eraseBufList.insert(buf.first);
            }
        }

        for (auto entry : eraseBufList)
        {
			vertexBuffers.erase(entry);
        }
    }
    else
    {
		vertexBuffers.clear();
    }

    LC_CATCH{ LC_THROW("LcTiledVisual2DRenderVulkan::ClearCache()") }
}

std::vector<VULKANTILEDVERTEX> GenerateTiles(const LcTiledSpriteComponent& tiledComp)
{
	std::vector<VULKANTILEDVERTEX> tiles;
	tiles.reserve(tiledComp.GetTilesData().size() * 6);

	for (const auto& tile : tiledComp.GetTilesData())
	{
		tiles.push_back(VULKANTILEDVERTEX{ tile.pos[0], tile.uv[0] });
		tiles.push_back(VULKANTILEDVERTEX{ tile.pos[2], tile.uv[2] });
		tiles.push_back(VULKANTILEDVERTEX{ tile.pos[1], tile.uv[1] });
		tiles.push_back(VULKANTILEDVERTEX{ tile.pos[0], tile.uv[0] });
		tiles.push_back(VULKANTILEDVERTEX{ tile.pos[1], tile.uv[1] });
		tiles.push_back(VULKANTILEDVERTEX{ tile.pos[3], tile.uv[3] });
	}

	return tiles;
}

void LcTiledVisual2DRenderVulkan::Setup(const IVisual* visual, const LcAppContext& context)
{
	LC_TRY

	auto device = render.GetVulkanDevice();
	auto physicalDevice = render.GetPhysicalDevice();
	auto commandBuffer = render.GetCommandBuffer();
	auto descriptorSets = render.GetDescriptorSets().GetSetsForFrame(render.GetCurrentFrame(), LcDSLayoutType::TiledVisual);
	if (!visual || !device || !physicalDevice || !commandBuffer || (descriptorSets.size() == 0))
	{
		throw LcException("Invalid render device");
	}

	auto tiledComp = static_cast<LcTiledSpriteComponent*>(visual ? visual->GetComponent(LcComponents::Tiled).get() : nullptr);
	if (!tiledComp)
	{
		throw LcException("No Tiled component found");
	}

	// find cached or create new vertex buffer
	auto vbIt = vertexBuffers.find(visual);
	if (vbIt == vertexBuffers.end())
	{
		// create vertex buffer
		auto tilesData = GenerateTiles(*tiledComp);
		auto& newTileMap = vertexBuffers[visual];
		newTileMap.vertexCount = (int)tilesData.size();

		VkBufferCreateInfo bufferInfo{};
		bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bufferInfo.size = sizeof(tilesData[0]) * tilesData.size();
		bufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
		bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		VkResult result = vkCreateBuffer(device, &bufferInfo, nullptr, &newTileMap.buffer);
		if (result != VK_SUCCESS)
		{
			throw LcException("Failed to create vertex buffer");
		}

		VkMemoryRequirements memRequirements;
		vkGetBufferMemoryRequirements(device, newTileMap.buffer, &memRequirements);

		VkMemoryAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocInfo.allocationSize = memRequirements.size;
		allocInfo.memoryTypeIndex = FindMemoryType(physicalDevice, memRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

		result = vkAllocateMemory(device, &allocInfo, nullptr, &newTileMap.bufferMemory);
		if (result != VK_SUCCESS)
		{
			throw LcException("Failed to allocate vertex buffer memory");
		}

		vkBindBufferMemory(device, newTileMap.buffer, newTileMap.bufferMemory, 0);

		void* data = nullptr;
		vkMapMemory(device, newTileMap.bufferMemory, 0, bufferInfo.size, 0, &data);
		memcpy(data, tilesData.data(), static_cast<size_t>(bufferInfo.size));
		vkUnmapMemory(device, newTileMap.bufferMemory);
	}

	vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline);

	// bind descriptors to set 0
	const uint32_t setOffset = 0;
	const uint32_t setCount = static_cast<uint32_t>(descriptorSets.size());

	// bind per type descriptors (LcDSLayoutType::TiledVisual)
	vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout,
		setOffset, setCount, descriptorSets.data(),
		0, nullptr
	);

	LC_CATCH{ LC_THROW("LcTiledVisual2DRenderVulkan::Setup()") }
}

void LcTiledVisual2DRenderVulkan::Render(const IVisual* visual, const LcAppContext& context)
{
	LC_TRY

	auto device = render.GetVulkanDevice();
	auto commandBuffer = render.GetCommandBuffer();
	auto sprite = (visual->GetTypeId() == LcCreatables::Sprite) ? static_cast<const ISprite*>(visual) : nullptr;
	auto vbIt = vertexBuffers.find(visual);
	if (!device || !commandBuffer || !sprite || (vbIt == vertexBuffers.end()))
	{
		throw LcException("Invalid render params");
	}

	VULKANTILEDVISUALDATA pushConst{};

	// update components
	if (auto texComp = sprite->GetTextureComponent())
	{
		const LcSpriteVulkan* widgetVulkan = static_cast<const LcSpriteVulkan*>(sprite);
		const VkDescriptorSet& textureSet = widgetVulkan->spriteSet[render.GetCurrentFrame()];
		if (!textureSet)
		{
			DebugMsg("Descriptor set not ready yet for texture: '%s'\n", texComp->GetTexturePath().c_str());
			return;
		}

		// bind texture descriptor to set 1
		const uint32_t setOffset = 1;
		const uint32_t setCount = 1;

		// bind per object descriptors
		vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout,
			setOffset, setCount, &textureSet,
			0, nullptr
		);
	}

	// update transform
	LcVector2 worldScale2D(context.world->GetWorldScale().GetScale());
	LcVector3 worldScale{ worldScale2D.x, worldScale2D.y, 1.0f };
	LcVector3 spritePos = sprite->GetPos() * worldScale;
	LcVector2 spriteSize = sprite->GetSize() * worldScale2D;
	spritePos.z = 0.0f;

	if (auto tiledComp = sprite->GetTiledComponent())
	{
		spriteSize = tiledComp->GetTilesScale() * To2(worldScale);
	}

	pushConst.mModel = TransformMatrix(spritePos, spriteSize, 0.0f, false, false);

	// draw sprite    
	VkBuffer vertexBuffers[] = { vbIt->second.buffer };
	VkDeviceSize offsets[] = { 0 };
	vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);
	vkCmdPushConstants(commandBuffer, pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(VULKANTILEDVISUALDATA), &pushConst);
	vkCmdDraw(commandBuffer, vbIt->second.vertexCount, vbIt->second.vertexCount / 3u, 0, 0);

	LC_CATCH{ LC_THROW("LcTiledVisual2DRenderVulkan::Render()") }
}

bool LcTiledVisual2DRenderVulkan::Supports(const TVFeaturesList& features) const
{
	bool needTexture = false, needAnimation = false, needTiles = false;
	for (auto& feature : features)
	{
		needTexture |= (feature == LcComponents::Texture);
		needAnimation |= (feature == LcComponents::FrameAnimation);
		needTiles |= (feature == LcComponents::Tiled);
	}
	return !needAnimation && needTexture && needTiles;
}
