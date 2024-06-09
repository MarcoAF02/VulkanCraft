
#include "../vulkan_base/game_device.h"
#include "../vulkan_base_render/game_model.h"
#include "game_pipeline.h"

// std
#include <cassert>
#include <fstream>
#include <iostream>
#include <stdexcept>

namespace vulkancraft
{
	VulkanRenderPipeline::VulkanRenderPipeline
	(
		GameDevice& device,
		const std::string& vertFilepath,
		const std::string& fragFilepath,
		const PipelineConfigInfo& configInfo
	) : game_device_ { device }
	{
		createGraphicsPipeline(vertFilepath, fragFilepath, configInfo);
	}

	VulkanRenderPipeline::~VulkanRenderPipeline()
	{
		vkDestroyShaderModule(game_device_.get_vulkan_device(), vert_shader_module_, nullptr);
		vkDestroyShaderModule(game_device_.get_vulkan_device(), frag_shader_module_, nullptr);
		vkDestroyPipeline(game_device_.get_vulkan_device(), graphics_pipeline_, nullptr);
	}

	std::vector<char> VulkanRenderPipeline::read_file(const std::string& file_path)
	{
		// HACK: 这里从磁盘读取 shader 二进制文件
		std::ifstream file{ file_path, std::ios::ate | std::ios::binary };

		if (!file.is_open())
		{
			throw std::runtime_error("打开 shader 二进制文件失败，使用的路径为：" + file_path);
		}

		size_t file_size = static_cast<size_t>(file.tellg());
		std::vector<char> buffer(file_size);

		file.seekg(0);
		file.read(buffer.data(), file_size);
		file.close();

		return buffer;
	}

	void VulkanRenderPipeline::createGraphicsPipeline
	(
		const std::string& vertFilepath,
		const std::string& fragFilepath,
		const PipelineConfigInfo& configInfo
	)
	{
		assert(configInfo.pipeline_layout_ != VK_NULL_HANDLE && "Cannot create graphics pipeline: no pipelineLayout provided in configInfo");
		assert(configInfo.render_pass_ != VK_NULL_HANDLE && "Cannot create graphics pipeline: no renderPass provided in configInfo");

		auto vertCode = read_file(vertFilepath);
		auto fragCode = read_file(fragFilepath);

		createShaderModule(vertCode, &vert_shader_module_);
		createShaderModule(fragCode, &frag_shader_module_);

		VkPipelineShaderStageCreateInfo shaderStages[2];
		shaderStages[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		shaderStages[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
		shaderStages[0].module = vert_shader_module_;
		shaderStages[0].pName = "main";
		shaderStages[0].flags = 0;
		shaderStages[0].pNext = nullptr;
		shaderStages[0].pSpecializationInfo = nullptr;
		shaderStages[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		shaderStages[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
		shaderStages[1].module = frag_shader_module_;
		shaderStages[1].pName = "main";
		shaderStages[1].flags = 0;
		shaderStages[1].pNext = nullptr;
		shaderStages[1].pSpecializationInfo = nullptr;

		auto& bindingDescriptions = configInfo.binding_description_vector_;
		auto& attributeDescriptions = configInfo.attribute_description_vector_;

		VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
		vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
		vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
		vertexInputInfo.vertexBindingDescriptionCount = static_cast<uint32_t>(bindingDescriptions.size());
		vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();
		vertexInputInfo.pVertexBindingDescriptions = bindingDescriptions.data();

		VkGraphicsPipelineCreateInfo pipelineInfo{};
		pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
		pipelineInfo.stageCount = 2;
		pipelineInfo.pStages = shaderStages;
		pipelineInfo.pVertexInputState = &vertexInputInfo;
		pipelineInfo.pInputAssemblyState = &configInfo.input_assembly_info_;
		pipelineInfo.pViewportState = &configInfo.viewport_info_;
		pipelineInfo.pRasterizationState = &configInfo.rasterization_info_;
		pipelineInfo.pMultisampleState = &configInfo.multisample_info_;
		pipelineInfo.pColorBlendState = &configInfo.color_blend_info_;
		pipelineInfo.pDepthStencilState = &configInfo.depth_stencil_info_;
		pipelineInfo.pDynamicState = &configInfo.dynamic_state_info_;

		pipelineInfo.layout = configInfo.pipeline_layout_;
		pipelineInfo.renderPass = configInfo.render_pass_;
		pipelineInfo.subpass = configInfo.subpass_;

		pipelineInfo.basePipelineIndex = -1;
		pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;

		if (vkCreateGraphicsPipelines(
			game_device_.get_vulkan_device(),
			VK_NULL_HANDLE,
			1,
			&pipelineInfo,
			nullptr,
			&graphics_pipeline_) != VK_SUCCESS
			)
		{
			throw std::runtime_error("failed to create graphics pipeline");
		}
	}

	void VulkanRenderPipeline::createShaderModule(const std::vector<char>& code, VkShaderModule* shaderModule)
	{
		VkShaderModuleCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		createInfo.codeSize = code.size();
		createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

		if (vkCreateShaderModule(game_device_.get_vulkan_device(), &createInfo, nullptr, shaderModule) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create shader module");
		}
	}

	void VulkanRenderPipeline::bind(VkCommandBuffer commandBuffer)
	{
		vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphics_pipeline_);
	}

	void VulkanRenderPipeline::default_pipeline_config_info(PipelineConfigInfo& configInfo)
	{
		configInfo.input_assembly_info_.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		configInfo.input_assembly_info_.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		configInfo.input_assembly_info_.primitiveRestartEnable = VK_FALSE;

		configInfo.viewport_info_.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		configInfo.viewport_info_.viewportCount = 1;
		configInfo.viewport_info_.pViewports = nullptr;
		configInfo.viewport_info_.scissorCount = 1;
		configInfo.viewport_info_.pScissors = nullptr;

		configInfo.rasterization_info_.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
		configInfo.rasterization_info_.depthClampEnable = VK_FALSE;
		configInfo.rasterization_info_.rasterizerDiscardEnable = VK_FALSE;
		configInfo.rasterization_info_.polygonMode = VK_POLYGON_MODE_FILL;
		configInfo.rasterization_info_.lineWidth = 1.0f;
		configInfo.rasterization_info_.cullMode = VK_CULL_MODE_NONE;
		configInfo.rasterization_info_.frontFace = VK_FRONT_FACE_CLOCKWISE;
		configInfo.rasterization_info_.depthBiasEnable = VK_FALSE;
		configInfo.rasterization_info_.depthBiasConstantFactor = 0.0f;  // Optional
		configInfo.rasterization_info_.depthBiasClamp = 0.0f;           // Optional
		configInfo.rasterization_info_.depthBiasSlopeFactor = 0.0f;     // Optional

		configInfo.multisample_info_.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		configInfo.multisample_info_.sampleShadingEnable = VK_FALSE;
		configInfo.multisample_info_.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
		configInfo.multisample_info_.minSampleShading = 1.0f;           // Optional
		configInfo.multisample_info_.pSampleMask = nullptr;             // Optional
		configInfo.multisample_info_.alphaToCoverageEnable = VK_FALSE;  // Optional
		configInfo.multisample_info_.alphaToOneEnable = VK_FALSE;       // Optional

		configInfo.color_blend_attachment_.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
		configInfo.color_blend_attachment_.blendEnable = VK_FALSE;
		configInfo.color_blend_attachment_.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;   // Optional
		configInfo.color_blend_attachment_.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;  // Optional
		configInfo.color_blend_attachment_.colorBlendOp = VK_BLEND_OP_ADD;              // Optional
		configInfo.color_blend_attachment_.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;   // Optional
		configInfo.color_blend_attachment_.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;  // Optional
		configInfo.color_blend_attachment_.alphaBlendOp = VK_BLEND_OP_ADD;              // Optional

		configInfo.color_blend_info_.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		configInfo.color_blend_info_.logicOpEnable = VK_FALSE;
		configInfo.color_blend_info_.logicOp = VK_LOGIC_OP_COPY;  // Optional
		configInfo.color_blend_info_.attachmentCount = 1;
		configInfo.color_blend_info_.pAttachments = &configInfo.color_blend_attachment_;
		configInfo.color_blend_info_.blendConstants[0] = 0.0f;  // Optional
		configInfo.color_blend_info_.blendConstants[1] = 0.0f;  // Optional
		configInfo.color_blend_info_.blendConstants[2] = 0.0f;  // Optional
		configInfo.color_blend_info_.blendConstants[3] = 0.0f;  // Optional

		configInfo.depth_stencil_info_.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
		configInfo.depth_stencil_info_.depthTestEnable = VK_TRUE;
		configInfo.depth_stencil_info_.depthWriteEnable = VK_TRUE;
		configInfo.depth_stencil_info_.depthCompareOp = VK_COMPARE_OP_LESS;
		configInfo.depth_stencil_info_.depthBoundsTestEnable = VK_FALSE;
		configInfo.depth_stencil_info_.minDepthBounds = 0.0f;  // Optional
		configInfo.depth_stencil_info_.maxDepthBounds = 1.0f;  // Optional
		configInfo.depth_stencil_info_.stencilTestEnable = VK_FALSE;
		configInfo.depth_stencil_info_.front = {};  // Optional
		configInfo.depth_stencil_info_.back = {};   // Optional

		configInfo.dynamic_state_enable_vector_ = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };
		configInfo.dynamic_state_info_.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
		configInfo.dynamic_state_info_.pDynamicStates = configInfo.dynamic_state_enable_vector_.data();
		configInfo.dynamic_state_info_.dynamicStateCount = static_cast<uint32_t>(configInfo.dynamic_state_enable_vector_.size());
		configInfo.dynamic_state_info_.flags = 0;

		configInfo.binding_description_vector_ = GameModel::Vertex::get_binding_description_vector();
		configInfo.attribute_description_vector_ = GameModel::Vertex::get_attribute_description_vector();
	}

	void VulkanRenderPipeline::enable_alpha_blending(PipelineConfigInfo& configInfo)
	{
		configInfo.color_blend_attachment_.blendEnable = VK_TRUE;
		configInfo.color_blend_attachment_.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
		configInfo.color_blend_attachment_.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
		configInfo.color_blend_attachment_.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
		configInfo.color_blend_attachment_.colorBlendOp = VK_BLEND_OP_ADD;
		configInfo.color_blend_attachment_.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
		configInfo.color_blend_attachment_.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
		configInfo.color_blend_attachment_.alphaBlendOp = VK_BLEND_OP_ADD;
	}

}  // namespace vulkancraft
