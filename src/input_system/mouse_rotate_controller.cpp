
#include "mouse_rotate_controller.h"

namespace vulkancraft
{
	MouseRotateController::MouseRotateController(GLFWwindow* window)
	{
		glfwSetWindowUserPointer(window, this);
		glfwSetCursorPosCallback(window, MouseRotateController::cursor_position_callback_static);
	}

	void MouseRotateController::cursor_position_callback_static(GLFWwindow* window, double xpos, double ypos)
	{
		auto controller = static_cast<MouseRotateController*>(glfwGetWindowUserPointer(window));

		if (controller)
		{
			controller -> cursor_position_callback(window, xpos, ypos);
		}
	}

	void MouseRotateController::cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
	{
		mouse_move_input_.x = static_cast<float>(xpos - last_pos_x_);
		mouse_move_input_.y = static_cast<float>(ypos - last_pos_y_);

		// 误差修正
		if (mouse_move_input_.x == 1 || mouse_move_input_.x == -1) mouse_move_input_.x = 0;
		if (mouse_move_input_.y == 1 || mouse_move_input_.y == -1) mouse_move_input_.y = 0;

		last_pos_x_ = xpos;
		last_pos_y_ = ypos;
	}

	void MouseRotateController::print_mouse_position()
	{
		if (mouse_move_input_.x == 0 || mouse_move_input_.y == 0) return;
		std::cout << "鼠标动量为：" << mouse_move_input_.x << " " << mouse_move_input_.y << std::endl;
	}

	void MouseRotateController::rotate_control(GLFWwindow* window, float delta_time, BaseGameObject& game_object)
	{
		glm::vec3 rotate{ 0 };

		rotate.x -= mouse_move_input_.y;
		rotate.y += mouse_move_input_.x;

		if (glm::dot(rotate, rotate) > std::numeric_limits<float>::epsilon())
		{
			game_object.transform_.rotation += look_speed_ * delta_time * rotate;
		}

		// limit pitch values between about +/- 85ish degrees
		game_object.transform_.rotation.x = glm::clamp(game_object.transform_.rotation.x, -1.5f, 1.5f);
		game_object.transform_.rotation.y = glm::mod(game_object.transform_.rotation.y, glm::two_pi<float>());
	}

}
