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

	// Create states
	VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
	vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	vertexInputInfo.vertexBindingDescriptionCount = 0;
	vertexInputInfo.vertexAttributeDescriptionCount = 0;

	VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
	inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_FAN;
	inputAssembly.primitiveRestartEnable = VK_FALSE;

	VkPipelineViewportStateCreateInfo viewportState{};
	viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	viewportState.viewportCount = 1;
	viewportState.scissorCount = 1;

	VkPipelineRasterizationStateCreateInfo rasterizer{};
	rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	rasterizer.depthClampEnable = VK_FALSE;
	rasterizer.rasterizerDiscardEnable = VK_FALSE;
	rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
	rasterizer.lineWidth = 1.0f;
	rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
	rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
	rasterizer.depthBiasEnable = VK_FALSE;

	VkPipelineMultisampleStateCreateInfo multisampling{};
	multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	multisampling.sampleShadingEnable = VK_FALSE;
	multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

	VkPipelineColorBlendAttachmentState colorBlendAttachment{};
	colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
	colorBlendAttachment.blendEnable = VK_FALSE;

	VkPipelineColorBlendStateCreateInfo colorBlending{};
	colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	colorBlending.logicOpEnable = VK_FALSE;
	colorBlending.logicOp = VK_LOGIC_OP_COPY;
	colorBlending.attachmentCount = 1;
	colorBlending.pAttachments = &colorBlendAttachment;
	colorBlending.blendConstants[0] = 0.0f;
	colorBlending.blendConstants[1] = 0.0f;
	colorBlending.blendConstants[2] = 0.0f;
	colorBlending.blendConstants[3] = 0.0f;

	std::vector<VkDynamicState> dynamicStates = {
		VK_DYNAMIC_STATE_VIEWPORT,
		VK_DYNAMIC_STATE_SCISSOR
	};

	// Create pipeline
	VkPipelineDynamicStateCreateInfo dynamicState{};
	dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
	dynamicState.pDynamicStates = dynamicStates.data();

	VkPushConstantRange pushConstantRange{};
	pushConstantRange.offset = 0;
	pushConstantRange.size = sizeof(VULKANCOLOREDSPRITEDATA);
	pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

	VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
	pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutInfo.setLayoutCount = 1;
	pipelineLayoutInfo.pSetLayouts = &render.GetUniforms().GetDescriptorSetLayout();
	pipelineLayoutInfo.pushConstantRangeCount = 1;
	pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;

	VkResult result = vkCreatePipelineLayout(device, &pipelineLayoutInfo, nullptr, &pipelineLayout);
	if (result != VK_SUCCESS)
	{
		throw LcException("Failed to create pipeline layout");
	}

	VkGraphicsPipelineCreateInfo pipelineInfo{};
	pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	pipelineInfo.stageCount = 2;
	pipelineInfo.pStages = shaderStages;
	pipelineInfo.pVertexInputState = &vertexInputInfo;
	pipelineInfo.pInputAssemblyState = &inputAssembly;
	pipelineInfo.pViewportState = &viewportState;
	pipelineInfo.pRasterizationState = &rasterizer;
	pipelineInfo.pMultisampleState = &multisampling;
	pipelineInfo.pColorBlendState = &colorBlending;
	pipelineInfo.pDynamicState = &dynamicState;
	pipelineInfo.layout = pipelineLayout;
	pipelineInfo.renderPass = render.GetRenderPass();
	pipelineInfo.subpass = 0;
	pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;

	result = vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &graphicsPipeline);
	if (result != VK_SUCCESS)
	{
		throw LcException("Failed to create graphics pipeline");
	}

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
		device = nullptr;
	}
}

void LcColoredSpriteRenderVulkan::Setup(const IVisual* visual, const LcAppContext& context)
{
	auto commandBuffer = render.GetCommandBuffer();
	auto descriptorSet = render.GetUniforms().GetCurrentDescriptorSet();
	if (!commandBuffer || !descriptorSet)
	{
		throw std::exception("LcColoredSpriteRenderVulkan::Setup(): Invalid render params");
	}

	vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline);
	vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, descriptorSet, 0, nullptr);
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
