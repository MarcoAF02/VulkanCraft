
#pragma once

#include "../gameplay_logic/game_window.h"
#include "../gameplay_logic/base_game_object.h"
#include "../game_physics_system/physics_aabb.h"
#include "../base_tools/enumeration_tools.h"

// stl
#include <chrono>

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

			// TODO: UI 菜单功能单独开一个控制器
			int unlock_cursor = GLFW_KEY_ESCAPE; // 解锁鼠标光标
			int lock_cursor = GLFW_KEY_LEFT_ALT; // 锁定鼠标光标
		};

		// 键盘控制观察者的操作
		void move_in_plane_xz(GLFWwindow* glfw_window, float delta_time, BaseGameObject& game_object, RotateState kRotateState);

		// 键盘控制玩家移动
		void player_move(GLFWwindow* glfw_window, float delta_time, BaseGameObject& player_obj);

		// 菜单按键操作
		void control_pause_menu(GLFWwindow* glfw_window);

		KeyMappings key_map_{};
		float move_speed_{ 1.6f };
		float look_speed_{ 1.2f };
	};

}  // namespace vulkancraft
