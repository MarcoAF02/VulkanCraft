
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

		// 这个类会被喂给实体管理器，最终在渲染，模拟线程创建前被创建，实体管理器（的指针）作为参数传递给两个 App

		// TODO: 让玩家撞到方块就停下来，包括移动，上升，下落等
		// 对于重力下落，玩家从脚下四个点向下发射射线，任意一条射线检测到了 AABB Collider，无论它是不是静态的，都算在地上。
		// 对于移动撞墙，将玩家移动方向的向量转换为世界空间向量，计算这个向量与碰到的 AABB Collider 的夹角，大于某个值就阻止玩家移动

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

		Rigidbody character_rigidbody_;
		GLFWwindow* glfw_window_;

	};
}
