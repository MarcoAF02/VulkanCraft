
#pragma once

#include "../gameplay_logic/game_camera.h"
#include "../game_physics_system/physics_aabb.h"
#include "../game_physics_system/rigidbody_system.h"
#include "../input_system/keyboard_input_controller.h"
#include "../input_system/mouse_rotate_controller.h"

// std
#include <iostream>

// glm 数学库
#include <glm/glm.hpp>

struct vulkancraft::TransformComponent;
class vulkancraft::AABBCollider; // AABB 碰撞盒
class vulkancraft::Rigidbody; // 物理系统
class vulkancraft::KeyboardMovementController; // 键盘控制器
class vulkancraft::MouseRotateController; // 鼠标旋转控制器

namespace vulkancraft
{
	// 玩家相机视野的参数
	struct PlayerCameraView
	{
		float fovy;
		float aspect;
		float near;
		float far;
	};

	class CharacterController
	{
	public:

		CharacterController();
		~CharacterController();

		CharacterController(const CharacterController&) = delete;
		CharacterController& operator = (const CharacterController&) = delete;

		void init_character_controller(glm::vec3 spawn_point);

		// 更新相机观察者的位置
		void update_camera_viewer_transform();

		// 玩家身体会左右旋转，只有摄像机会上下旋转
		void move(float fixed_delta_time, GLFWwindow* glfw_window);
		void rotate(float fixed_delta_time, GLFWwindow* glfw_window);

		// TODO: 跳起来顶头回落
		void update_player_collision(float delta_time); // 自由落体更新

		// 得到相机观察物体的坐标（只用坐标就可以了，这足够控制摄像机位置）
		TransformComponent get_camera_viewer_transform();

#pragma region DEBUG 用函数

		// 把玩家的身高，碰撞体等全打印出来
		void print_player_details() const;

#pragma endregion

	private:

		// 人类的高度为 1.8 宽度为 0.6
		// 摄像机不需要 AABB Collider，玩家角色需要

		float character_height_ = 1.8f; // 人类的身高
		float character_width_ = 0.6f; // 人类的宽度

		// TODO: 把 Camera 放在这里
		glm::vec3 spawn_point_ = glm::vec3 {0, 0, 0};
		TransformComponent camera_viewer_transform_;

		// GameBaseCamera 类在 Render App 中，不能放在这里
		BaseGameObject character_game_obj_; // 不能上下旋转，只能左右旋转
		BaseGameObject camera_game_obj_; // 摄像机用的游戏物体，用于给摄像机上下旋转

		KeyboardMovementController keyboard_move_controller_;
		MouseRotateController mouse_rotate_controller_;

		AABBCollider character_collider_ =
		{
			character_game_obj_.transform_,
			character_game_obj_.get_id()
		};

		// TODO: 这个东西要放在物理线程中循环
		Rigidbody character_rigidbody_;
	};
}
