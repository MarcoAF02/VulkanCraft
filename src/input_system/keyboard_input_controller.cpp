
#include "keyboard_input_controller.h"

namespace vulkancraft
{
	void KeyboardMovementController::move_in_plane_xz(GameWindow* game_window, float delta_time, BaseGameObject& game_object)
	{
		// 上下左右旋转，看
		glm::vec3 rotate {0, 0, 0};

		if (kKeyboardState_[keys.look_right]) rotate.y += 1.0f;
		if (kKeyboardState_[keys.look_right]) rotate.y -= 1.0f;
		if (kKeyboardState_[keys.look_up]) rotate.y += 1.0f;
		if (kKeyboardState_[keys.look_down]) rotate.y -= 1.0f;

		if (glm::dot(rotate, rotate) > std::numeric_limits<float>::epsilon())
		{
			game_object.transform_.rotation += rotate_speed_ * delta_time * glm::normalize(rotate);
		}

		// 限制最大旋转角度
		game_object.transform_.rotation.x = glm::clamp(game_object.transform_.rotation.x, -1.5f, 1.5f);
		game_object.transform_.rotation.y = glm::mod(game_object.transform_.rotation.y, glm::two_pi<float>());

		// 上下左右移动
		float yaw = game_object.transform_.rotation.y;

		const glm::vec3 forward_dir{ sin(yaw), 0.f, cos(yaw) };
		const glm::vec3 right_dir{ forward_dir.z, 0.f, -forward_dir.x };
		const glm::vec3 up_dir{ 0.f, -1.f, 0.f };

		glm::vec3 move_dir{ 0.f };
		if (kKeyboardState_[keys.move_forward]) move_dir += forward_dir;
		if (kKeyboardState_[keys.move_backward]) move_dir -= forward_dir;
		if (kKeyboardState_[keys.move_right]) move_dir += right_dir;
		if (kKeyboardState_[keys.move_left]) move_dir -= right_dir;
		if (kKeyboardState_[keys.move_up]) move_dir += up_dir;
		if (kKeyboardState_[keys.move_down]) move_dir -= up_dir;

		if (glm::dot(move_dir, move_dir) > std::numeric_limits<float>::epsilon())
		{
			game_object.transform_.translation += move_speed_ * delta_time * glm::normalize(move_dir);
		}
	}

}
