/**
* ColoredSpriteRenderVulkan.cpp
* 15.08.2024
* (c) Denis Romakhov
*/

#include "RenderSystem/RenderSystemVulkan/ColoredSpriteRenderVulkan.h"
#include "RenderSystem/RenderSystemVulkan/RenderSystemVulkan.h"
#include "World/SpriteInterface.h"
#include "Core/LCException.h"
#include <shaderc/shaderc.hpp>


static const char* ColoredSpriteShaderName = "ColoredSprite2d.glsl";

struct VULKANCOLOREDSPRITEDATA
{
	LcMatrix4 mModel;
	LcColor4 colors[4];
};


LcColoredSpriteRenderVulkan::LcColoredSpriteRenderVulkan(IRenderDeviceVulkan& inRender, const LcAppContext& context)
	: render(inRender)
	, pipelineLayout(VK_NULL_HANDLE)
	, graphicsPipeline(VK_NULL_HANDLE)
{
	LC_TRY

	auto device = render.GetVulkanDevice();
	if (!device)
	{
		throw LcException("Invalid device");
	}

	auto shaderText = render.GetShaderCode(ColoredSpriteShaderName);
	if (shaderText.empty())
	{
		throw LcException("Cannot find shader");
	}

	shaderc::Compiler compiler;
	shaderc::CompileOptions options;

	// Compile shaders
	auto fragShaderCompiled = compiler.PreprocessGlsl(shaderText, shaderc_glsl_fragment_shader, "fs.tmp", options);
	auto fragShaderAssembly = compiler.CompileGlslToSpvAssembly(shaderText, shaderc_glsl_fragment_shader, "fs.tmp", options);

	std::string fragShaderAssemblyCode(fragShaderAssembly.cbegin(), fragShaderAssembly.cend());
	auto fragShaderAssembled = compiler.AssembleToSpv(fragShaderAssemblyCode);

	options.AddMacroDefinition("COMPILE_VERTEX_SHADER");

	auto vertShaderCompiled = compiler.PreprocessGlsl(shaderText, shaderc_glsl_vertex_shader, "vs.tmp", options);
	auto vertShaderAssembly = compiler.CompileGlslToSpvAssembly(shaderText, shaderc_glsl_vertex_shader, "vs.tmp", options);

	std::string vertShaderAssemblyCode(vertShaderAssembly.cbegin(), vertShaderAssembly.cend());
	auto vertShaderAssembled = compiler.AssembleToSpv(vertShaderAssemblyCode);

	// Create shader modules
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

	// Create pipeline layout
	VkPushConstantRange pushConstantRange{};
	pushConstantRange.offset = 0;
	pushConstantRange.size = sizeof(VULKANCOLOREDSPRITEDATA);
	pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

	VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
	pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutInfo.setLayoutCount = 1;
	pipelineLayoutInfo.pSetLayouts = render.GetDescriptorSets().GetLayoutForFrame(render.GetCurrentFrame(), LcDSLayoutType::ColoredSprite);
	pipelineLayoutInfo.pushConstantRangeCount = 1;
	pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;

	VkResult result = vkCreatePipelineLayout(device, &pipelineLayoutInfo, nullptr, &pipelineLayout);
	if (result != VK_SUCCESS)
	{
		throw LcException("Failed to create pipeline layout");
	}

	// Create pipeline
	VkGraphicsPipelineCreateInfo pipelineInfo{};
	render.FillPipelineDefaults(pipelineInfo);

	pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	pipelineInfo.stageCount = 2;
	pipelineInfo.pStages = shaderStages;
	pipelineInfo.layout = pipelineLayout;
	pipelineInfo.renderPass = render.GetRenderPass();

	result = vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &graphicsPipeline);
	if (result != VK_SUCCESS)
	{
		throw LcException("Failed to create graphics pipeline");
	}

	// Cleanup
	vkDestroyShaderModule(device, fragShaderModule, nullptr);
	vkDestroyShaderModule(device, vertShaderModule, nullptr);

	LC_CATCH{ LC_THROW("LcColoredSpriteRenderVulkan()") }
}

LcColoredSpriteRenderVulkan::~LcColoredSpriteRenderVulkan()
{
	auto device = render.GetVulkanDevice();
	if (device)
	{
		if (graphicsPipeline) vkDestroyPipeline(device, graphicsPipeline, nullptr);
		if (pipelineLayout) vkDestroyPipelineLayout(device, pipelineLayout, nullptr);
	}
}

void LcColoredSpriteRenderVulkan::Setup(const IVisual* visual, const LcAppContext& context)
{
	auto commandBuffer = render.GetCommandBuffer();
	auto descriptorSets = render.GetDescriptorSets().GetSetsForFrame(render.GetCurrentFrame(), LcDSLayoutType::ColoredSprite);
	if (!commandBuffer || (descriptorSets.size() == 0))
	{
		throw std::exception("LcColoredSpriteRenderVulkan::Setup(): Invalid render params");
	}

	const uint32_t dscOffset = 0;
	const uint32_t dscCount = static_cast<uint32_t>(descriptorSets.size());

	vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline);

	// bind per type descriptors (LcDSLayoutType::ColoredSprite)
	vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout,
		dscOffset, dscCount, descriptorSets.data(),
		0, nullptr
	);
}

void LcColoredSpriteRenderVulkan::Render(const IVisual* visual, const LcAppContext& context)
{
	LC_TRY

	auto device = render.GetVulkanDevice();
	auto commandBuffer = render.GetCommandBuffer();
	auto sprite = (visual->GetTypeId() == LcCreatables::Sprite) ? static_cast<const ISprite*>(visual) : nullptr;
	if (!device || !commandBuffer || !sprite)
	{
		throw std::exception("LcColoredSpriteRenderVulkan::Render(): Invalid render params");
	}

	VULKANCOLOREDSPRITEDATA pushConst{};

	// update components
	auto colors = sprite->GetColorsComponent();
	auto tint = sprite->GetTintComponent();
	if (colors || tint)
	{
		auto colorsData = colors ? colors->GetData() : tint->GetData();
		memcpy(pushConst.colors, colorsData, sizeof(pushConst.colors));
	}
	else
	{
		static const LcColor4 defaultColors[] = { LcDefaults::White4, LcDefaults::White4, LcDefaults::White4, LcDefaults::White4 };
		memcpy(pushConst.colors, defaultColors, sizeof(pushConst.colors));
	}

	// update transform
	LcVector2 worldScale2D(context.world->GetWorldScale().GetScale());
	LcVector3 worldScale{ worldScale2D.x, worldScale2D.y, 1.0f };
	LcVector3 spritePos = sprite->GetPos() * worldScale;
	LcVector2 spriteSize = sprite->GetSize() * worldScale2D;
	pushConst.mModel = TransformMatrix(spritePos, spriteSize, sprite->GetRotZ(), false, false);

	// draw sprite
	vkCmdPushConstants(commandBuffer, pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(VULKANCOLOREDSPRITEDATA), &pushConst);
	vkCmdDraw(commandBuffer, 4, 2, 0, 0);

	LC_CATCH{ LC_THROW("LcColoredSpriteRenderVulkan::Render()") }
}

bool LcColoredSpriteRenderVulkan::Supports(const TVFeaturesList& features) const
{
	bool needTexture = false, needAnimation = false, needTiles = false;
	for (auto& feature : features)
	{
		needTexture |= (feature == LcComponents::Texture);
		needAnimation |= (feature == LcComponents::FrameAnimation);
		needTiles |= (feature == LcComponents::Tiled);
	}
	return !needTiles && !needAnimation && !needTexture;
}
