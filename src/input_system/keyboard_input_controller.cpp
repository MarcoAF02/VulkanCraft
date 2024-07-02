
#include "keyboard_input_controller.h"

// std
#include <limits>

namespace vulkancraft
{
	void KeyboardMovementController::move_in_plane_xz(GLFWwindow* glfw_window, float delta_time, BaseGameObject& game_object, RotateState kRotateState)
	{
		glm::vec3 rotate { 0 } ;

		if (kRotateState == kRotateNone)
		{
			rotate = {0, 0, 0};
		}

		if (kRotateState == kRotateAll)
		{
			if (glfwGetKey(glfw_window, key_map_.look_right) == GLFW_PRESS) rotate.y += 1.f;
			if (glfwGetKey(glfw_window, key_map_.look_left) == GLFW_PRESS) rotate.y -= 1.f;
			if (glfwGetKey(glfw_window, key_map_.look_up) == GLFW_PRESS) rotate.x += 1.f;
			if (glfwGetKey(glfw_window, key_map_.look_down) == GLFW_PRESS) rotate.x -= 1.f;
		}

		if (kRotateState == kRotateX)
		{
			if (glfwGetKey(glfw_window, key_map_.look_up) == GLFW_PRESS) rotate.x += 1.f;
			if (glfwGetKey(glfw_window, key_map_.look_down) == GLFW_PRESS) rotate.x -= 1.f;
		}

		if (kRotateState == kRotateY)
		{
			if (glfwGetKey(glfw_window, key_map_.look_right) == GLFW_PRESS) rotate.y += 1.f;
			if (glfwGetKey(glfw_window, key_map_.look_left) == GLFW_PRESS) rotate.y -= 1.f;
		}

		if (glm::dot(rotate, rotate) > std::numeric_limits<float>::epsilon())
		{
			game_object.transform_.rotation += look_speed_ * delta_time * glm::normalize(rotate);
		}

		// limit pitch values between about +/- 85ish degrees
		game_object.transform_.rotation.x = glm::clamp(game_object.transform_.rotation.x, -1.5f, 1.5f);
		game_object.transform_.rotation.y = glm::mod(game_object.transform_.rotation.y, glm::two_pi<float>());

		float yaw = game_object.transform_.rotation.y;
		const glm::vec3 forwardDir{ sin(yaw), 0.f, cos(yaw) };
		const glm::vec3 rightDir{ forwardDir.z, 0.f, -forwardDir.x };
		const glm::vec3 upDir{ 0.f, -1.f, 0.f };

		glm::vec3 moveDir{ 0.f };
		if (glfwGetKey(glfw_window, key_map_.move_forward) == GLFW_PRESS) moveDir += forwardDir;
		if (glfwGetKey(glfw_window, key_map_.move_backward) == GLFW_PRESS) moveDir -= forwardDir;
		if (glfwGetKey(glfw_window, key_map_.move_right) == GLFW_PRESS) moveDir += rightDir;
		if (glfwGetKey(glfw_window, key_map_.move_left) == GLFW_PRESS) moveDir -= rightDir;
		if (glfwGetKey(glfw_window, key_map_.move_up) == GLFW_PRESS) moveDir += upDir;
		if (glfwGetKey(glfw_window, key_map_.move_down) == GLFW_PRESS) moveDir -= upDir;

		if (glm::dot(moveDir, moveDir) > std::numeric_limits<float>::epsilon())
		{
			game_object.transform_.translation += move_speed_ * delta_time * glm::normalize(moveDir);
		}
	}

	void KeyboardMovementController::player_move(GLFWwindow* glfw_window, float delta_time, BaseGameObject& player_obj)
	{
		// TODO: 这里先处理跳跃逻辑，然后处理移动插值平滑阻尼逻辑
		// TODO: 最后，处理撞墙重置移动速度的逻辑，参考 Unity 官方的 FPS MicroGame

		float yaw = player_obj.transform_.rotation.y;
		const glm::vec3 forwardDir{ sin(yaw), 0.f, cos(yaw) };
		const glm::vec3 rightDir{ forwardDir.z, 0.f, -forwardDir.x };
		const glm::vec3 upDir{ 0.f, -1.f, 0.f };

		glm::vec3 moveDir{ 0.f };
		if (glfwGetKey(glfw_window, key_map_.move_forward) == GLFW_PRESS) moveDir += forwardDir;
		if (glfwGetKey(glfw_window, key_map_.move_backward) == GLFW_PRESS) moveDir -= forwardDir;
		if (glfwGetKey(glfw_window, key_map_.move_right) == GLFW_PRESS) moveDir += rightDir;
		if (glfwGetKey(glfw_window, key_map_.move_left) == GLFW_PRESS) moveDir -= rightDir;

		// TODO: 上下移动应该只存在于飞行模式
		if (glfwGetKey(glfw_window, key_map_.move_up) == GLFW_PRESS) moveDir += upDir;
		if (glfwGetKey(glfw_window, key_map_.move_down) == GLFW_PRESS) moveDir -= upDir;

		if (glm::dot(moveDir, moveDir) > std::numeric_limits<float>::epsilon())
		{
			player_obj.transform_.translation += move_speed_ * delta_time * glm::normalize(moveDir);
		}
	}

	void KeyboardMovementController::control_pause_menu(GLFWwindow* glfw_window)
	{
		if (glfwGetKey(glfw_window, key_map_.unlock_cursor) == GLFW_PRESS)
		{
			// 显示鼠标指针并允许光标离开窗口
			glfwSetInputMode(glfw_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		}

		if (glfwGetKey(glfw_window, key_map_.lock_cursor) == GLFW_PRESS)
		{
			// 隐藏鼠标指针并将其固定在窗口内
			glfwSetInputMode(glfw_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		}
	}

}  // namespace vulkancraft
