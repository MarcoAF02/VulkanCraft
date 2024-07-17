
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
			if (glfwGetKey(glfw_window, move_key_map_.look_right) == GLFW_PRESS) rotate.y += 1.f;
			if (glfwGetKey(glfw_window, move_key_map_.look_left) == GLFW_PRESS) rotate.y -= 1.f;
			if (glfwGetKey(glfw_window, move_key_map_.look_up) == GLFW_PRESS) rotate.x += 1.f;
			if (glfwGetKey(glfw_window, move_key_map_.look_down) == GLFW_PRESS) rotate.x -= 1.f;
		}

		if (kRotateState == kRotateX)
		{
			if (glfwGetKey(glfw_window, move_key_map_.look_up) == GLFW_PRESS) rotate.x += 1.f;
			if (glfwGetKey(glfw_window, move_key_map_.look_down) == GLFW_PRESS) rotate.x -= 1.f;
		}

		if (kRotateState == kRotateY)
		{
			if (glfwGetKey(glfw_window, move_key_map_.look_right) == GLFW_PRESS) rotate.y += 1.f;
			if (glfwGetKey(glfw_window, move_key_map_.look_left) == GLFW_PRESS) rotate.y -= 1.f;
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
		if (glfwGetKey(glfw_window, move_key_map_.move_forward) == GLFW_PRESS) moveDir += forwardDir;
		if (glfwGetKey(glfw_window, move_key_map_.move_backward) == GLFW_PRESS) moveDir -= forwardDir;
		if (glfwGetKey(glfw_window, move_key_map_.move_right) == GLFW_PRESS) moveDir += rightDir;
		if (glfwGetKey(glfw_window, move_key_map_.move_left) == GLFW_PRESS) moveDir -= rightDir;
		if (glfwGetKey(glfw_window, move_key_map_.move_up) == GLFW_PRESS) moveDir += upDir;
		if (glfwGetKey(glfw_window, move_key_map_.move_down) == GLFW_PRESS) moveDir -= upDir;

		if (glm::dot(moveDir, moveDir) > std::numeric_limits<float>::epsilon())
		{
			game_object.transform_.translation += move_force_ * delta_time * glm::normalize(moveDir);
		}
	}

	void KeyboardMovementController::player_move(GLFWwindow* glfw_window, float delta_time, BaseGameObject& player_obj, btRigidBody* player_rb)
	{
		float yaw = player_obj.transform_.rotation.y;
		const glm::vec3 forwardDir{ sin(yaw), 0.f, cos(yaw) };
		const glm::vec3 rightDir{ forwardDir.z, 0.f, -forwardDir.x };
		const glm::vec3 upDir{ 0.f, -1.f, 0.f };

		glm::vec3 moveDir{ 0.f };
		if (glfwGetKey(glfw_window, move_key_map_.move_forward) == GLFW_PRESS) moveDir += forwardDir;
		if (glfwGetKey(glfw_window, move_key_map_.move_backward) == GLFW_PRESS) moveDir -= forwardDir;
		if (glfwGetKey(glfw_window, move_key_map_.move_right) == GLFW_PRESS) moveDir += rightDir;
		if (glfwGetKey(glfw_window, move_key_map_.move_left) == GLFW_PRESS) moveDir -= rightDir;

		// TODO: 上下移动应该只存在于飞行模式，空格和左 Shift 应该改成跳跃和潜行
		if (glfwGetKey(glfw_window, move_key_map_.move_up) == GLFW_PRESS) moveDir += upDir;
		if (glfwGetKey(glfw_window, move_key_map_.move_down) == GLFW_PRESS) moveDir -= upDir;

		btVector3 bt_move_dir{ 0.0f, 0.0f, 0.0f };

		if (glm::dot(moveDir, moveDir) > std::numeric_limits<float>::epsilon())
		{
			player_move_input_state_ = kMoveInput;

			glm::vec3 record_move_dir = glm::normalize(moveDir);

			bt_move_dir =
			{
				record_move_dir.x,
				record_move_dir.y,
				record_move_dir.z
			};
		}
		else
		{
			player_move_input_state_ = kStop;
		}

		if (player_move_input_state_ == kMoveInput) // 玩家有移动输入时
		{
			btVector3 cur_velocity = player_rb->getLinearVelocity();

			float cur_move_speed = cur_velocity.length();
			float x_speed = cur_velocity.x();
			float z_speed = cur_velocity.z();

			glm::vec2 horizontal_vec =
			{
				x_speed,
				z_speed
			};

			float horizontal_speed = glm::length(horizontal_vec);

			if (horizontal_speed >= max_move_speed_)
			{
				glm::vec2 normalized_horizontal_vec = glm::normalize(horizontal_vec);
				glm::vec2 limited_horizontal_vec = normalized_horizontal_vec * max_move_speed_;

				btVector3 limited_horizontal_velocity(limited_horizontal_vec.x, 0, limited_horizontal_vec.y);
				btVector3 new_velocity = btVector3(limited_horizontal_velocity.x(), cur_velocity.y(), limited_horizontal_velocity.z());

				player_rb->setLinearVelocity(new_velocity);
			}
			else
			{
				player_rb->applyCentralForce(bt_move_dir * move_force_ * delta_time);
			}
		}

		if (player_move_input_state_ == kStop) // 玩家没有移动输入时
		{
			btVector3 cur_velocity = player_rb->getLinearVelocity();

			float cur_move_speed = cur_velocity.length();
			float x_speed = cur_velocity.x();
			float y_speed = cur_velocity.y();
			float z_speed = cur_velocity.z();

			x_speed *= reduce_move_coefficient_;
			z_speed *= reduce_move_coefficient_;

			btVector3 stop_vec(x_speed, y_speed, z_speed);

			player_rb->setLinearVelocity(stop_vec);
		}
	}

} // namespace vulkancraft
