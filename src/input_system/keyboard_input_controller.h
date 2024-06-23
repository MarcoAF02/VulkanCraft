
#pragma once

#include "../gameplay_logic/game_window.h"
#include "../gameplay_logic/base_game_object.h"
#include "../game_physics_system/physics_aabb.h"
#include "../base_tools/enumeration_tools.h"

// stl
#include <chrono>

// ������������༯��

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

			// TODO: UI �˵����ܵ�����һ��������
			int unlock_cursor = GLFW_KEY_ESCAPE; // ���������
			int lock_cursor = GLFW_KEY_LEFT_ALT; // ���������
		};

		// ���̿��ƹ۲��ߵĲ���
		void move_in_plane_xz(GLFWwindow* glfw_window, float delta_time, BaseGameObject& game_object, RotateState kRotateState);

		// ���̿�������ƶ�
		void player_move(GLFWwindow* glfw_window, float delta_time, BaseGameObject& player_obj);

		// �˵���������
		void control_pause_menu(GLFWwindow* glfw_window);

		KeyMappings key_map_{};
		float move_speed_{ 1.6f };
		float look_speed_{ 1.2f };
	};

}  // namespace vulkancraft
