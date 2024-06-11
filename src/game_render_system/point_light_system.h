
#pragma once

#include "../gameplay_logic/game_camera.h"
#include "../vulkan_base/game_device.h"
#include "../vulkan_base_render/game_frame_info.h"
#include "../gameplay_logic/base_game_object.h"
#include "../vulkan_base_render/game_pipeline.h"

// std
#include <iostream>
#include <vector>
#include <memory>

namespace vulkancraft
{
	class PointLightSystem
	{
	public:
		PointLightSystem(GameDevice& game_device, VkRenderPass render_pass, VkDescriptorSetLayout global_set_layout);
		~PointLightSystem();

		PointLightSystem(const PointLightSystem&) = delete;
		PointLightSystem& operator=(const PointLightSystem&) = delete;

		void update(FrameInfo& frame_info, GlobalUbo& ubo);
		void render(FrameInfo& frame_info);

	private:
		void create_pipeline_layout(VkDescriptorSetLayout global_set_layout);
		void create_pipeline(VkRenderPass render_pass);

		GameDevice& game_device_;

		std::unique_ptr<VulkanRenderPipeline> vulkan_render_pipeline_;
		VkPipelineLayout pipeline_layout_;
	};

}  // namespace vulkancraft
