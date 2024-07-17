
#include "keyboard_menu_controller.h"

namespace vulkancraft
{
	void KeyboardMenuController::control_pause_menu(GLFWwindow* glfw_window)
	{
		if (glfwGetKey(glfw_window, menu_key_map_.unlock_cursor) == GLFW_PRESS)
		{
			// 显示鼠标指针并允许光标离开窗口
			glfwSetInputMode(glfw_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		}

		if (glfwGetKey(glfw_window, menu_key_map_.lock_cursor) == GLFW_PRESS)
		{
			// 隐藏鼠标指针并将其固定在窗口内
			glfwSetInputMode(glfw_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		}
	}
}
