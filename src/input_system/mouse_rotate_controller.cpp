
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
		MouseRotateController* controller = static_cast<MouseRotateController*>(glfwGetWindowUserPointer(window));

		if (controller)
		{
			controller -> cursor_position_callback(window, xpos, ypos);
		}
	}

	void MouseRotateController::cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
	{
		mouse_offset_.x = static_cast<float>(xpos - last_pos_x_);
		mouse_offset_.y = static_cast<float>(ypos - last_pos_y_);

		last_pos_x_ = xpos;
		last_pos_y_ = ypos;
	}

	void MouseRotateController::fix_mouse_offset()
	{
		glm::vec2 current_mouse_offset = mouse_offset_;
		if (current_mouse_offset == last_mouse_offset_) mouse_offset_ = { 0, 0 };
		last_mouse_offset_ = current_mouse_offset;
	}

	void MouseRotateController::print_mouse_position()
	{
		std::cout << "鼠标动量为：" << mouse_offset_.x << " " << mouse_offset_.y << std::endl;
	}

	void MouseRotateController::rotate_control(GLFWwindow* window, float delta_time, BaseGameObject& game_object, RotateState kRotateState)
	{
		fix_mouse_offset();

		glm::vec3 rotate{ 0 };

		if (kRotateState == kRotateNone)
		{
			rotate = { 0, 0, 0 };
		}

		if (kRotateState == kRotateAll)
		{
			rotate.x -= mouse_offset_.y;
			rotate.y += mouse_offset_.x;
		}

		if (kRotateState == kRotateX)
		{
			rotate.x -= mouse_offset_.y;
		}

		if (kRotateState == kRotateY)
		{
			rotate.y += mouse_offset_.x;
		}

		if (glm::dot(rotate, rotate) > std::numeric_limits<float>::epsilon())
		{
			game_object.transform_.rotation += look_speed_ * delta_time * rotate;
		}

		// limit pitch values between about +/- 85ish degrees
		game_object.transform_.rotation.x = glm::clamp(game_object.transform_.rotation.x, -1.5f, 1.5f);
		game_object.transform_.rotation.y = glm::mod(game_object.transform_.rotation.y, glm::two_pi<float>());
	}

}
