
#pragma once

#include "../gameplay_logic/game_window.h"

namespace vulkancraft
{
	class KeyboardMenuController // 菜单键盘控制器
	{
	public:
		struct MenuKeyMappings
		{
			int unlock_cursor = GLFW_KEY_ESCAPE; // 解锁鼠标光标
			int lock_cursor = GLFW_KEY_LEFT_ALT; // 锁定鼠标光标
		};

		MenuKeyMappings menu_key_map_{};

		// 菜单按键操作
		void control_pause_menu(GLFWwindow* glfw_window);
	};
}
