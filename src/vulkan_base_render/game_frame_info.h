
#pragma once

#include "../gameplay_logic/game_camera.h"
#include "../gameplay_logic/base_game_object.h"

// lib
#include <vulkan/vulkan.h>

namespace vulkancraft
{
	const int kMaxLightCount = 32; // 目前的设计是场景中最多 32 个灯光

	struct PointLight
	{
		glm::vec4 position { };  // ignore w
		glm::vec4 color { };     // w is intensity
	};

	struct GlobalUbo
	{
		glm::mat4 projection{ 1.f };
		glm::mat4 view{ 1.f };
		glm::mat4 inverse_view{ 1.f };
		glm::vec4 ambient_light_color{ 1.f, 1.f, 1.f, .02f };  // w is intensity
		PointLight point_lights_[kMaxLightCount];
		int num_lights;
	};

	struct FrameInfo
	{
		int frame_index;
		float frame_time;
		VkCommandBuffer command_buffer;
		GameBaseCamera& camera;
		VkDescriptorSet global_descriptor_set;
		BaseGameObject::Map& game_objects;
	};

}  // namespace vulkancraft
