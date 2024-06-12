
#include "base_render_system.h"

// libs
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

// std
#include <array>
#include <cassert>
#include <stdexcept>

namespace vulkancraft
{
	struct SimplePushConstantData
	{
		glm::mat4 modelMatrix{ 1.f };
		glm::mat4 normalMatrix{ 1.f };
	};

	SimpleRenderSystem::SimpleRenderSystem
	(
		GameDevice& device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout
	) : game_device_{ device }
	{
		create_pipeline_layout(globalSetLayout);
		create_pipeline(renderPass);
	}

	SimpleRenderSystem::~SimpleRenderSystem()
	{
		vkDestroyPipelineLayout(game_device_.get_vulkan_device(), pipeline_layout_, nullptr);
	}

	void SimpleRenderSystem::create_pipeline_layout(VkDescriptorSetLayout globalSetLayout)
	{
		VkPushConstantRange pushConstantRange{};
		pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
		pushConstantRange.offset = 0;
		pushConstantRange.size = sizeof(SimplePushConstantData);

		std::vector<VkDescriptorSetLayout> descriptorSetLayouts{ globalSetLayout };

		VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo.setLayoutCount = static_cast<uint32_t>(descriptorSetLayouts.size());
		pipelineLayoutInfo.pSetLayouts = descriptorSetLayouts.data();
		pipelineLayoutInfo.pushConstantRangeCount = 1;
		pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;

		if (vkCreatePipelineLayout(game_device_.get_vulkan_device(), &pipelineLayoutInfo, nullptr, &pipeline_layout_) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create pipeline layout!");
		}
	}

	void SimpleRenderSystem::create_pipeline(VkRenderPass renderPass)
	{
		assert(pipeline_layout_ != nullptr && "Cannot create pipeline before pipeline layout");

		PipelineConfigInfo pipelineConfig{};
		VulkanRenderPipeline::default_pipeline_config_info(pipelineConfig);
		pipelineConfig.render_pass_ = renderPass;
		pipelineConfig.pipeline_layout_ = pipeline_layout_;

		vulkan_render_pipeline_ = std::make_unique<VulkanRenderPipeline>
		(
			game_device_,
			"shaders/base_render_shader.vert.spv",
			"shaders/base_render_shader.frag.spv",
			pipelineConfig
		);
	}

	void SimpleRenderSystem::render_game_objects(FrameInfo& frameInfo)
	{
		vulkan_render_pipeline_->bind(frameInfo.command_buffer);

		vkCmdBindDescriptorSets
		(
			frameInfo.command_buffer,
			VK_PIPELINE_BIND_POINT_GRAPHICS,
			pipeline_layout_,
			0,
			1,
			&frameInfo.global_descriptor_set,
			0,
			nullptr
		);

		for (auto& kv : frameInfo.game_object_map)
		{
			auto& obj = kv.second;
			if (obj.model_ == nullptr) continue;

			SimplePushConstantData push{};
			push.modelMatrix = obj.transform_.mat4();
			push.normalMatrix = obj.transform_.normal_matrix();

			vkCmdPushConstants
			(
				frameInfo.command_buffer,
				pipeline_layout_,
				VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
				0,
				sizeof(SimplePushConstantData),
				&push
			);

			obj.model_ -> bind(frameInfo.command_buffer);
			obj.model_ -> draw(frameInfo.command_buffer);
		}
	}

}  // namespace vulkancraft
