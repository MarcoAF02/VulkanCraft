
#pragma once

#include "../vulkan_base/game_device.h"

// std
#include <string>
#include <vector>

namespace vulkancraft
{
	struct PipelineConfigInfo
	{
		PipelineConfigInfo() = default;
		PipelineConfigInfo(const PipelineConfigInfo&) = delete;
		PipelineConfigInfo& operator = (const PipelineConfigInfo&) = delete;

		std::vector<VkVertexInputBindingDescription> binding_description_vector_ = {};
		std::vector<VkVertexInputAttributeDescription> attribute_description_vector_ = {};

		VkPipelineViewportStateCreateInfo viewport_info_ = {};
		VkPipelineInputAssemblyStateCreateInfo input_assembly_info_ = {};
		VkPipelineRasterizationStateCreateInfo rasterization_info_ = {};
		VkPipelineMultisampleStateCreateInfo multisample_info_ = {};
		VkPipelineColorBlendAttachmentState color_blend_attachment_ = {};
		VkPipelineColorBlendStateCreateInfo color_blend_info_ = {};
		VkPipelineDepthStencilStateCreateInfo depth_stencil_info_ = {};

		std::vector<VkDynamicState> dynamic_state_enable_vector_ = {};

		VkPipelineDynamicStateCreateInfo dynamic_state_info_ = {};
		VkPipelineLayout pipeline_layout_ = nullptr;
		VkRenderPass render_pass_ = nullptr;
		uint32_t subpass_ = 0;
	};

	class VulkanRenderPipeline
	{
	public:
		VulkanRenderPipeline
		(
			GameDevice& device,
			const std::string& vert_file_path,
			const std::string& frag_file_path,
			const PipelineConfigInfo& config_info
		);

		~VulkanRenderPipeline();

		VulkanRenderPipeline(const VulkanRenderPipeline&) = delete;
		VulkanRenderPipeline& operator = (const VulkanRenderPipeline&) = delete;

		void bind(VkCommandBuffer command_buffer);

		static void default_pipeline_config_info(PipelineConfigInfo& config_info);
		static void enable_alpha_blending(PipelineConfigInfo& config_info);

	private:

		static std::vector<char> read_file(const std::string& file_path);

		void createGraphicsPipeline
		(
			const std::string& vert_file_path,
			const std::string& frag_file_path,
			const PipelineConfigInfo& config_info
		);

		void createShaderModule(const std::vector<char>& code_vector, VkShaderModule* shader_module);

		GameDevice& game_device_;
		VkPipeline graphics_pipeline_;
		VkShaderModule vert_shader_module_;
		VkShaderModule frag_shader_module_;
	};

}  // namespace vulkancraft
