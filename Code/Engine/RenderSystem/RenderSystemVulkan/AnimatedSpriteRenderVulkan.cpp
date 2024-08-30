/**
* AnimatedSpriteRenderVulkan.cpp
* 30.08.2024
* (c) Denis Romakhov
*/

#include "RenderSystem/RenderSystemVulkan/AnimatedSpriteRenderVulkan.h"
#include "RenderSystem/RenderSystemVulkan/RenderSystemVulkan.h"
#include "RenderSystem/RenderSystemVulkan/VisualsVulkan.h"
#include "RenderSystem/RenderSystemVulkan/UtilsVulkan.h"
#include "World/SpriteInterface.h"
#include "GUI/WidgetInterface.h"
#include "Core/LCException.h"
#include "Core/LCUtils.h"
#include <shaderc/shaderc.hpp>


static const char* AnimatedSpriteShaderName = "AnimatedSprite2d.glsl";

struct VULKANANIMATEDSPRITEDATA
{
	LcMatrix4 mModel;
	LcColor4 colors[4];
	LcVector4 anim; // x - frame width, y - frame height, z - offsetX, w - offsetY
	float hasColor;
};


LcAnimatedSpriteRenderVulkan::LcAnimatedSpriteRenderVulkan(IRenderDeviceVulkan& inRender, const LcAppContext& context)
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

	auto shaderText = render.GetShaderCode(AnimatedSpriteShaderName);
	if (shaderText.empty())
	{
		throw LcException("Cannot find shader");
	}

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

	// create pipeline layout
	VkPushConstantRange pushConstantRange{};
	pushConstantRange.offset = 0;
	pushConstantRange.size = sizeof(VULKANANIMATEDSPRITEDATA);
	pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

	std::array<VkDescriptorSetLayout, 2> layouts = {
		*render.GetDescriptorSets().GetLayout(LcDSLayoutType::AnimatedSprite),
		*render.GetDescriptorSets().GetLayout(LcDSLayoutType::Textures)
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

	LC_CATCH{ LC_THROW("LcAnimatedSpriteRenderVulkan()") }
}

LcAnimatedSpriteRenderVulkan::~LcAnimatedSpriteRenderVulkan()
{
	auto device = render.GetVulkanDevice();
	if (device)
	{
		if (graphicsPipeline) vkDestroyPipeline(device, graphicsPipeline, nullptr);
		if (pipelineLayout) vkDestroyPipelineLayout(device, pipelineLayout, nullptr);
	}
}

void LcAnimatedSpriteRenderVulkan::Setup(const IVisual* visual, const LcAppContext& context)
{
	auto commandBuffer = render.GetCommandBuffer();
	auto& descriptorSet = render.GetDescriptorSets().GetDescriptorSet(render.GetCurrentFrame(), LcDSLayoutType::AnimatedSprite);
	if (!commandBuffer || !visual || !descriptorSet)
	{
		throw LcException("LcAnimatedSpriteRenderVulkan::Setup(): Invalid render params");
	}

	vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline);

	// bind descriptors to set 0
	const uint32_t setOffset = 0;
	const uint32_t setCount = 1;

	// bind per type descriptors (LcDSLayoutType::TexturedVisual)
	vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout,
		setOffset, setCount, &descriptorSet,
		0, nullptr
	);
}

void LcAnimatedSpriteRenderVulkan::Render(const IVisual* visual, const LcAppContext& context)
{
	LC_TRY

	auto device = render.GetVulkanDevice();
	auto commandBuffer = render.GetCommandBuffer();
	auto sprite = (visual->GetTypeId() == LcCreatables::Sprite) ? static_cast<const ISprite*>(visual) : nullptr;
	if (!device || !commandBuffer || !sprite)
	{
		throw LcException("Invalid render params");
	}

	VULKANANIMATEDSPRITEDATA pushConst{};

	// update components
	auto colors = sprite->GetColorsComponent();
	auto tint = sprite->GetTintComponent();
	if (colors || tint)
	{
		auto colorsData = colors ? colors->GetData() : tint->GetData();
		memcpy(pushConst.colors, colorsData, sizeof(pushConst.colors));
		pushConst.hasColor = VK_TRUE_F;
	}
	else
	{
		static LcColor4 defaultColors[] = { LcDefaults::White4, LcDefaults::White4, LcDefaults::White4, LcDefaults::White4 };
		memcpy(pushConst.colors, defaultColors, sizeof(pushConst.colors));
	}

	if (auto anim = sprite->GetAnimationComponent())
	{
		pushConst.anim = anim->GetAnimData();
	}

	if (auto texComp = sprite->GetTextureComponent())
	{
		const LcSpriteVulkan* spriteVulkan = static_cast<const LcSpriteVulkan*>(visual);
		const VkDescriptorSet& textureSet = spriteVulkan->spriteSet[render.GetCurrentFrame()];
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
	pushConst.mModel = TransformMatrix(spritePos, spriteSize, sprite->GetRotZ(), false, false);

	// draw visual
	vkCmdPushConstants(commandBuffer, pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(VULKANANIMATEDSPRITEDATA), &pushConst);
	vkCmdDraw(commandBuffer, 4, 2, 0, 0);

	LC_CATCH{ LC_THROW("LcAnimatedSpriteRenderVulkan::Render()") }
}

bool LcAnimatedSpriteRenderVulkan::Supports(const TVFeaturesList& features) const
{
	bool needAnimation = false, needTiles = false;
	for (auto& feature : features)
	{
		needAnimation |= (feature == LcComponents::FrameAnimation);
		needTiles |= (feature == LcComponents::Tiled);
	}
	return !needTiles && needAnimation;
}
