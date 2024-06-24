
#pragma once

// stl
#include <chrono>

// glm ��ѧ��
#include <glm/glm.hpp>

#include "../gameplay_logic/game_window.h"
#include "../gameplay_logic/base_game_object.h"
#include "../base_tools/enumeration_tools.h"

// GameWindow ���� GLFWwindow

namespace vulkancraft
{
	class MouseRotateController
	{
	public:

		MouseRotateController();

		// ��ʼ����������
		void initialize(GLFWwindow* window);

		// ��̬�ص��������������λ�ñ仯
		static void cursor_position_callback_static(GLFWwindow* window, double xpos, double ypos);

		// ���λ�ûص�����
		void cursor_position_callback(GLFWwindow* window, double xpos, double ypos);

		// ��ת�����Ƶ�����
		void rotate_control(GLFWwindow* window, float delta_time, BaseGameObject& game_object, RotateState kRotateState);

		// ��꾲ֹʱ��ֹ�ӽ�ת��
		void fix_mouse_offset();

		// DEBUG ��ӡ����ƶ���Ϣ
		void print_mouse_position();

	private:

		// ��һ֡����궯��
		double last_pos_x_ = 0.0;
		double last_pos_y_ = 0.0;

		float look_speed_ = 1.2f;

		glm::vec2 mouse_offset_ = {0, 0};
		glm::vec2 last_mouse_offset_ = {0, 0};

		GLFWwindow* glfw_window_ = nullptr;
	};

}
