
#pragma once

#include "../gameplay_logic/game_window.h"
#include "../gameplay_logic/base_game_object.h"

// 键盘输入管理类集合

namespace vulkancraft
{
	class KeyboardMovementController
	{
	public:
		struct KeyMappings
		{
			int move_left = GLFW_KEY_A;
			int move_right = GLFW_KEY_D;
			int move_forward = GLFW_KEY_W;
			int move_backward = GLFW_KEY_S;
			int move_up = GLFW_KEY_E;
			int move_down = GLFW_KEY_Q;
			int look_left = GLFW_KEY_LEFT;
			int look_right = GLFW_KEY_RIGHT;
			int look_up = GLFW_KEY_UP;
			int look_down = GLFW_KEY_DOWN;
		};

		void move_in_plane_xz(GLFWwindow* glfw_window, float delta_time, BaseGameObject& game_object);

		KeyMappings key_map_{};
		float move_speed_{ 1.6f };
		float look_speed_{ 1.2f };
	};

}  // namespace vulkancraft
