
#pragma once

#include "../gameplay_logic/game_camera.h"
#include "../gameplay_logic/base_game_object.h"

// lib
#include <vulkan/vulkan.h>

// HACK: 此处注意内存对齐
// std - 140: alignas(16) 

namespace vulkancraft
{
	// 这个数值如果变化 shader 内也要跟上
	const int kMaxLightCount = 256; // 目前的设计是场景中最多 256 个灯光

	struct PointLight
	{
		alignas(16) glm::vec4 position { };  // ignore w
		alignas(16) glm::vec4 color { };     // w is intensity
	};

	struct GlobalUbo
	{
		alignas(16) glm::mat4 projection{ 1.f };
		alignas(16) glm::mat4 view{ 1.f };
		alignas(16) glm::mat4 inverse_view{ 1.f };
		alignas(16) glm::vec4 ambient_light_color{ 1.0f, 1.0f, 1.0f, 0.2f };  // w is intensity
		alignas(16) PointLight point_lights[kMaxLightCount];
		alignas(4) int num_lights;
	};

	struct FrameInfo
	{
		alignas(4) int frame_index;
		alignas(8) float frame_time;
		alignas(8) VkCommandBuffer command_buffer;
		GameBaseCamera& camera;
		alignas(8) VkDescriptorSet global_descriptor_set;
		alignas(8) BaseGameObject::Map& game_object_map;
	};

}  // namespace vulkancraft
