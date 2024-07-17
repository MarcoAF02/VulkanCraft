
#pragma once

#include "../gameplay_logic/game_window.h"
#include "../gameplay_logic/base_game_object.h"
#include "../base_tools/enumeration_tools.h"

// Bullet 3 ����ϵͳ
#include "btBulletDynamicsCommon.h"

// stl
#include <chrono>

// ������������༯��

namespace vulkancraft
{
	class KeyboardMovementController // �ƶ����̿�����
	{
	public:
		struct MoveKeyMappings
		{
			int move_left = GLFW_KEY_A;
			int move_right = GLFW_KEY_D;
			int move_forward = GLFW_KEY_W;
			int move_backward = GLFW_KEY_S;
			int move_up = GLFW_KEY_SPACE;
			int move_down = GLFW_KEY_LEFT_SHIFT;
			int look_left = GLFW_KEY_LEFT;
			int look_right = GLFW_KEY_RIGHT;
			int look_up = GLFW_KEY_UP;
			int look_down = GLFW_KEY_DOWN;
		};

		// ���̿��ƹ۲��ߵĲ���
		void move_in_plane_xz(GLFWwindow* glfw_window, float delta_time, BaseGameObject& game_object, RotateState kRotateState);

		// ���̿�������ƶ�����Ҫһ�� btRigidBody��������������ƶ���
		void player_move(GLFWwindow* glfw_window, float delta_time, BaseGameObject& player_obj, btRigidBody* player_rb);

		PlayerMoveInputState player_move_input_state_ = kStop; // ��ʼ������ƶ�״̬

		MoveKeyMappings move_key_map_{};
		float move_force_{ 120.0f };
		float max_move_speed_{ 4.0f };
		float reduce_move_coefficient_{ 0.999996f }; // ���û�а��°���ʱ���ƶ���С���ٶȣ�ϵ����
		float look_speed_{ 1.2f };
	};

} // namespace vulkancraft
