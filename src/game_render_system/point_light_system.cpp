
#include "point_light_system.h"

// libs
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

// std
#include <array>
#include <cassert>
#include <map>
#include <stdexcept>

namespace vulkancraft
{
	struct PointLightPushConstants
	{
		glm::vec4 position{};
		glm::vec4 color{};
		float radius;
	};

	PointLightSystem::PointLightSystem(GameDevice& game_device, VkRenderPass render_pass, VkDescriptorSetLayout global_set_layout) : game_device_{ game_device }
	{
		create_pipeline_layout(global_set_layout);
		create_pipeline(render_pass);
	}

	PointLightSystem::~PointLightSystem()
	{
		vkDestroyPipelineLayout(game_device_.get_vulkan_device(), pipeline_layout_, nullptr);
	}

	void PointLightSystem::create_pipeline_layout(VkDescriptorSetLayout global_set_layout)
	{
		VkPushConstantRange pushConstantRange{};
		pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
		pushConstantRange.offset = 0;
		pushConstantRange.size = sizeof(PointLightPushConstants);

		std::vector<VkDescriptorSetLayout> descriptorSetLayouts{ global_set_layout };

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

	void PointLightSystem::create_pipeline(VkRenderPass render_pass)
	{
		assert(pipeline_layout_ != nullptr && "Cannot create pipeline before pipeline layout");

		PipelineConfigInfo pipelineConfig{};
		VulkanRenderPipeline::default_pipeline_config_info(pipelineConfig);
		VulkanRenderPipeline::enable_alpha_blending(pipelineConfig);

		pipelineConfig.attribute_description_vector_.clear();
		pipelineConfig.binding_description_vector_.clear();
		pipelineConfig.render_pass_ = render_pass;
		pipelineConfig.pipeline_layout_ = pipeline_layout_;

		vulkan_render_pipeline_ = std::make_unique<VulkanRenderPipeline>
		(
			game_device_,
			"shaders/point_light.vert.spv",
			"shaders/point_light.frag.spv",
			pipelineConfig
		);
	}

	void PointLightSystem::update(FrameInfo& frame_info, GlobalUbo& ubo)
	{
		auto rotateLight = glm::rotate(glm::mat4(1.f), 0.5f * frame_info.frame_time, { 0.f, -1.f, 0.f });
		int lightIndex = 0;

		for (auto& kv : frame_info.game_object_map)
		{
			auto& obj = kv.second;
			if (obj.point_light_ == nullptr) continue;

			assert(lightIndex <= kMaxLightCount && "Point lights exceed maximum specified");

			// std::cout << kMaxLightCount << std::endl;

			// HACK: µÆÄã²»Ðí×ª
			// update light position
			// obj.transform_.translation = glm::vec3(rotateLight * glm::vec4(obj.transform_.translation, 1.f));

			// copy light to ubo
			ubo.point_lights[lightIndex].position = glm::vec4(obj.transform_.translation, 1.f);
			ubo.point_lights[lightIndex].color = glm::vec4(obj.color_, obj.point_light_ -> light_intensity);

			lightIndex += 1;
		}

		ubo.num_lights = lightIndex;
	}

	void PointLightSystem::render(FrameInfo& frameInfo)
	{
		// sort lights
		std::map<float, id_t> sorted;

		for (auto& kv : frameInfo.game_object_map)
		{
			auto& obj = kv.second;
			if (obj.point_light_ == nullptr) continue;

			// calculate distance
			auto offset = frameInfo.camera.get_position() - obj.transform_.translation;
			float disSquared = glm::dot(offset, offset);
			sorted[disSquared] = obj.get_id();
		}

		vulkan_render_pipeline_ -> bind(frameInfo.command_buffer);

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

		// iterate through sorted lights in reverse order
		for (auto it = sorted.rbegin(); it != sorted.rend(); ++it)
		{
			// use game obj id to find light object
			auto& obj = frameInfo.game_object_map.at(it->second);

			PointLightPushConstants push{};
			push.position = glm::vec4(obj.transform_.translation, 1.f);
			push.color = glm::vec4(obj.color_, obj.point_light_->light_intensity);
			push.radius = obj.transform_.scale.x;

			vkCmdPushConstants
			(
				frameInfo.command_buffer,
				pipeline_layout_,
				VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
				0,
				sizeof(PointLightPushConstants),
				&push
			);

			vkCmdDraw(frameInfo.command_buffer, 6, 1, 0, 0);
		}
	}

}  // namespace vulkancraft
