
#pragma once

// glm ��ѧ��
#include <glm/glm.hpp>

#include "../gameplay_logic/game_window.h"
#include "../gameplay_logic/base_game_object.h"

// GameWindow ���� GLFWwindow

namespace vulkancraft
{
	class MouseRotateController
	{
	public:

		MouseRotateController(GLFWwindow* window);

		// ��̬�ص��������������λ�ñ仯
		static void cursor_position_callback_static(GLFWwindow* window, double xpos, double ypos);

		// ���λ�ûص�����
		void cursor_position_callback(GLFWwindow* window, double xpos, double ypos);

		// ��ת�����Ƶ�����
		void rotate_control(GLFWwindow* window, float delta_time, BaseGameObject& game_object);

		// DEBUG ��ӡ����ƶ���Ϣ
		void print_mouse_position();

	private:

		double last_pos_x_ = 0.0;
		double last_pos_y_ = 0.0;

		float look_speed_ = 1.2f;

		glm::vec2 mouse_move_input_ = {0, 0};
	};

}
