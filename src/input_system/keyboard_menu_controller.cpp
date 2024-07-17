
#include "keyboard_menu_controller.h"

namespace vulkancraft
{
	void KeyboardMenuController::control_pause_menu(GLFWwindow* glfw_window)
	{
		if (glfwGetKey(glfw_window, menu_key_map_.unlock_cursor) == GLFW_PRESS)
		{
			// ��ʾ���ָ�벢�������뿪����
			glfwSetInputMode(glfw_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		}

		if (glfwGetKey(glfw_window, menu_key_map_.lock_cursor) == GLFW_PRESS)
		{
			// �������ָ�벢����̶��ڴ�����
			glfwSetInputMode(glfw_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		}
	}
}
