
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

// TODO: 这个类写好以后，render app 中的一些类就可以不需要了

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

		CharacterController(GLFWwindow* glfw_window);
		~CharacterController();

		CharacterController(const CharacterController&) = delete;
		CharacterController& operator = (const CharacterController&) = delete;

		// 初始化角色控制器，第二个参数是屏幕长宽比例
		void init_character_controller(glm::vec3 spawn_point, PlayerCameraView camera_view);

		// TODO: 集键盘控制器，鼠标控制器，BaseGameObject，AABBCollider，RigidbodySystem Camera 为一体

		// 玩家身体会左右旋转，只有摄像机会上下旋转
		void move(float fixed_delta_time);
		void rotate(float fixed_delta_time);

		// TODO: 跳起来顶头回落
		void update_player_collision(float delta_time); // 自由落体更新

		// 自由落体和地面检测是物理的，撞墙判断是 character controller 的

		// 这个类会被喂给实体管理器，最终在渲染，模拟线程创建前被创建，实体管理器（的指针）作为参数传递给两个 App

	private:

		// 人类的高度为 1.8 宽度为 0.6
		// 摄像机不需要 AABB Collider，玩家角色需要

		float character_height_ = 1.8f; // 人类的身高
		float character_width_ = 0.6f; // 人类的宽度

		BaseGameObject character_game_obj_; // 不能上下旋转，只能左右旋转
		BaseGameObject camera_game_obj_; // 摄像机用的游戏物体，用于给摄像机上下旋转
		GameBaseCamera fps_camera_;

		KeyboardMovementController keyboard_move_controller_;

		MouseRotateController mouse_rotate_controller_ =
		{
			glfw_window_
		};

		AABBCollider character_collider_ =
		{
			character_game_obj_.transform_,
			character_game_obj_.get_id()
		};

		// TODO: 这个东西要放在物理线程中循环
		Rigidbody character_rigidbody_;
		GLFWwindow* glfw_window_;

	};
}
