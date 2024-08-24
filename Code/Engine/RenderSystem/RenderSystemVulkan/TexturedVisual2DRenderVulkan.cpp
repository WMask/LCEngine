/**
* TexturedVisual2DRenderVulkan.cpp
* 24.08.2024
* (c) Denis Romakhov
*/

#include "RenderSystem/RenderSystemVulkan/TexturedVisual2DRenderVulkan.h"
#include "RenderSystem/RenderSystemVulkan/RenderSystemVulkan.h"
#include "RenderSystem/RenderSystemVulkan/UtilsVulkan.h"
#include "World/SpriteInterface.h"
#include "GUI/WidgetInterface.h"
#include "Core/LCException.h"
#include <shaderc/shaderc.hpp>


static const char* TexturedSpriteShaderName = "TexturedVisual2d.glsl";

struct VULKANTEXTUREDVISUALDATA
{
	LcMatrix4 mModel;
	LcColor4 colors[4];
	LcVector2 uvs[4];
	uint8_t options[4];
};


LcTexturedVisual2DRenderVulkan::LcTexturedVisual2DRenderVulkan(class IRenderDeviceVulkan& inRender, const LcAppContext& context)
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

	auto shaderText = render.GetShaderCode(TexturedSpriteShaderName);
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
	pushConstantRange.size = sizeof(VULKANTEXTUREDVISUALDATA);
	pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

	VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
	pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutInfo.setLayoutCount = 1;
	pipelineLayoutInfo.pSetLayouts = render.GetUniforms().GetLayoutFor(LcDSLayoutType::TexturedVisual);
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

LcTexturedVisual2DRenderVulkan::~LcTexturedVisual2DRenderVulkan()
{
	auto device = render.GetVulkanDevice();
	if (device)
	{
		if (graphicsPipeline) vkDestroyPipeline(device, graphicsPipeline, nullptr);
		if (pipelineLayout) vkDestroyPipelineLayout(device, pipelineLayout, nullptr);
	}
}

void LcTexturedVisual2DRenderVulkan::Setup(const IVisual* visual, const LcAppContext& context)
{
	auto commandBuffer = render.GetCommandBuffer();
	auto descriptorSet = render.GetUniforms().GetDescriptorSetFor(LcDSLayoutType::TexturedVisual);
	if (!commandBuffer || !descriptorSet)
	{
		throw std::exception("LcTexturedVisual2DRenderVulkan::Setup(): Invalid render params");
	}

	vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline);
	vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, descriptorSet, 0, nullptr);
}

void LcTexturedVisual2DRenderVulkan::Render(const IVisual* visual, const LcAppContext& context)
{
	LC_TRY

	auto device = render.GetVulkanDevice();
	auto commandBuffer = render.GetCommandBuffer();
	auto sprite = (visual->GetTypeId() == LcCreatables::Sprite) ? static_cast<const ISprite*>(visual) : nullptr;
	auto widget = (visual->GetTypeId() == LcCreatables::Widget) ? static_cast<const IWidget*>(visual) : nullptr;
	if (!device || !commandBuffer || (!sprite && !widget))
	{
		throw std::exception("LcColoredSpriteRenderVulkan::Render(): Invalid render params");
	}

	if (sprite)
	{
		VULKANTEXTUREDVISUALDATA pushConst{};

		// update components
		auto colors = sprite->GetColorsComponent();
		auto tint = sprite->GetTintComponent();
		if (colors || tint)
		{
			auto colorsData = colors ? colors->GetData() : tint->GetData();
			memcpy(pushConst.colors, colorsData, sizeof(pushConst.colors));
			pushConst.options[HAS_COLOR] = 1;
		}
		else
		{
			static LcColor4 defaultColors[] = { LcDefaults::White4, LcDefaults::White4, LcDefaults::White4, LcDefaults::White4 };
			memcpy(pushConst.colors, defaultColors, sizeof(pushConst.colors));
		}

		if (auto customUV = sprite->GetCustomUVComponent())
		{
			memcpy(pushConst.uvs, customUV->GetData(), sizeof(pushConst.uvs));
			pushConst.options[HAS_CUSTOM_UV] = 1;
		}

		if (auto texComp = sprite->GetTextureComponent())
		{
			LcTextureVulkan texture{};
			render.GetTextureLoader().LoadTexture(texComp->GetTexturePath().c_str(), texture);
			render.GetUniforms().SetTextureFor(LcDSLayoutType::TexturedVisual, texture.imageView);
			pushConst.options[HAS_TEXTURE] = 1;
		}

		// update transform
		LcVector2 worldScale2D(context.world->GetWorldScale().GetScale());
		LcVector3 worldScale{ worldScale2D.x, worldScale2D.y, 1.0f };
		LcVector3 spritePos = sprite->GetPos() * worldScale;
		LcVector2 spriteSize = sprite->GetSize() * worldScale2D;
		pushConst.mModel = TransformMatrix(spritePos, spriteSize, sprite->GetRotZ(), false, false);

		// draw visual
		vkCmdPushConstants(commandBuffer, pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(VULKANTEXTUREDVISUALDATA), &pushConst);
		vkCmdDraw(commandBuffer, 4, 2, 0, 0);
	}
	else
	if (widget)
	{/*
		VS_FLAGS_BUFFER flags{};

		// update components
		auto colors = widget->GetColorsComponent();
		auto tint = widget->GetTintComponent();
		if (colors || tint)
		{
			auto colorsData = colors ? colors->GetData() : tint->GetData();
			d3dDevice->UpdateSubresource(colorsBuffer, 0, NULL, colorsData, 0, 0);
			flags.bHasColor = TRUE;
		}
		else
		{
			static LcColor4 defaultColors[] = { LcDefaults::Invisible, LcDefaults::Invisible, LcDefaults::Invisible, LcDefaults::Invisible };
			d3dDevice->UpdateSubresource(colorsBuffer, 0, NULL, defaultColors, 0, 0);
		}

		if (auto customUV = widget->GetButtonComponent())
		{
			d3dDevice->UpdateSubresource(uvsBuffer, 0, NULL, customUV->GetData(), 0, 0);
			flags.bHasCustomUV = TRUE;
		}
		else
		if (auto customUV = widget->GetCheckboxComponent())
		{
			d3dDevice->UpdateSubresource(uvsBuffer, 0, NULL, customUV->GetData(), 0, 0);
			flags.bHasCustomUV = TRUE;
		}

		auto widgetDX10 = static_cast<const LcWidgetDX10*>(widget);
		if (widget->HasComponent(LcComponents::Texture))
		{
			d3dDevice->PSSetShaderResources(0, 1, (ID3D10ShaderResourceView**)&widgetDX10->spriteTextureSV);
			flags.bHasTexture = TRUE;
		}
		else
		{
			ID3D10ShaderResourceView* nullSRV[1] = { nullptr };
			d3dDevice->PSSetShaderResources(0, 1, nullSRV);
		}

		d3dDevice->UpdateSubresource(flagsBuffer, 0, NULL, &flags, 0, 0);

		// update transform
		LcVector2 worldScale2D(context.world->GetWorldScale().GetScale());
		LcVector3 worldScale{ worldScale2D.x, worldScale2D.y, 1.0f };
		LcVector3 widgetPos = widget->GetPos() * worldScale;
		LcVector2 widgetSize = widget->GetSize() * worldScale2D;
		LcMatrix4 trans = TransformMatrix(widgetPos, widgetSize);
		d3dDevice->UpdateSubresource(transBuffer, 0, NULL, &trans, 0, 0);

		// render sprite
		d3dDevice->Draw(4, 0);

		if (widgetDX10->textTextureSV)
		{
			// set text texture
			d3dDevice->PSSetShaderResources(0, 1, (ID3D10ShaderResourceView**)widgetDX10->textTextureSV.GetAddressOf());
			flags.bHasTexture = TRUE;

			static LcVector4 defaultUVs[] = { To4(LcVector2{ 0.0, 0.0 }), To4(LcVector2{ 1.0, 0.0 }), To4(LcVector2{ 1.0, 1.0 }), To4(LcVector2{ 0.0, 1.0 }) };
			d3dDevice->UpdateSubresource(uvsBuffer, 0, NULL, defaultUVs, 0, 0);
			flags.bHasCustomUV = TRUE;

			d3dDevice->UpdateSubresource(flagsBuffer, 0, NULL, &flags, 0, 0);

			// move in front of the sprite
			trans[2][3] = widget->GetPos().z + 0.01f;
			d3dDevice->UpdateSubresource(transBuffer, 0, NULL, &trans, 0, 0);

			// render text texture
			d3dDevice->Draw(4, 0);
		}*/
	}

	LC_CATCH{ LC_THROW("LcTexturedVisual2DRenderVulkan::Render()") }
}

bool LcTexturedVisual2DRenderVulkan::Supports(const TVFeaturesList& features) const
{
	bool needTexture = false, needAnimation = false, needTiles = false, needBasicParticles = false;
	for (auto& feature : features)
	{
		needTexture |= (feature == LcComponents::Texture);
		needAnimation |= (feature == LcComponents::FrameAnimation);
		needTiles |= (feature == LcComponents::Tiled);
		needBasicParticles |= (feature == LcComponents::Particles);
	}
	return !needAnimation && !needTiles && !needBasicParticles && needTexture;
}
