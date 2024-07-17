
#pragma once

#include "../gameplay_logic/game_window.h"

namespace vulkancraft
{
	class KeyboardMenuController // �˵����̿�����
	{
	public:
		struct MenuKeyMappings
		{
			int unlock_cursor = GLFW_KEY_ESCAPE; // ���������
			int lock_cursor = GLFW_KEY_LEFT_ALT; // ���������
		};

		MenuKeyMappings menu_key_map_{};

		// �˵���������
		void control_pause_menu(GLFWwindow* glfw_window);
	};
}
