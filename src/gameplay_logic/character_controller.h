
#pragma once

#include "../gameplay_logic/game_camera.h"
#include "../input_system/keyboard_input_controller.h"
#include "../input_system/mouse_rotate_controller.h"
#include "../base_tools/details.h"
#include "../base_tools/physics_object_save_data.h"

// std
#include <iostream>

// glm 数学库
#include <glm/glm.hpp>

// Bullet 3 物理系统
#include "btBulletDynamicsCommon.h"

struct vulkancraft::TransformComponent;
class vulkancraft::KeyboardMovementController; // 键盘控制器
class vulkancraft::MouseRotateController; // 鼠标旋转控制器
struct BlockGenerateData;
struct PhysicsObjectTransData;
struct PhysicsObjectSaveData;

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

		// 初始化 character controller
		void init_character_controller();

		// 初始化玩家物理系统
		void init_character_physics();

		// 注册玩家物理系统
		PhysicsObjectSaveData& get_player_physics_data();
		BaseGameObject::id_t get_player_obj_id() const;

		// 设置并更新玩家主相机
		void set_player_camera(PlayerCameraView camera_view);

		// 得到玩家观察相机
		GameBaseCamera& get_player_camera();

		void sync_player_rigidbody_pos(); // 同步玩家角色位置至物理组件

		// 初始化鼠标视角控制器
		void init_mouse_rotate(GLFWwindow* glfw_window);

		// 玩家身体会左右旋转，只有摄像机会上下旋转
		void move(float fixed_delta_time, GLFWwindow* glfw_window);
		void rotate(float fixed_delta_time, GLFWwindow* glfw_window);

		// 根据碰撞方向停止玩家移动
		// void stop_moving(CollisionSide side);

#pragma region DEBUG 用函数

		// 把玩家的身高，碰撞体等全打印出来
		void print_player_details() const;

#pragma endregion

	private:

		// 人类的高度为 1.8 宽度为 0.6
		// 摄像机不需要 AABB Collider，玩家角色需要

		float character_height_ = 1.8f; // 人类的身高
		float character_width_ = 0.6f; // 人类的宽度
		float camera_height_ = 0.83f; // 摄像机的高度

		float half_height_ = character_height_ / 2;
		float half_width_ = character_width_ / 2;

		BaseGameObject::id_t player_id_ = 2560; // 玩家的硬编码 ID

		glm::vec3 spawn_point_ = { 0.0f, -10.0f, 0.0f };

		BaseGameObject character_game_obj_; // 不能上下旋转，只能左右旋转
		BaseGameObject camera_game_obj_; // 摄像机用的观察游戏物体，用于给摄像机上下旋转

		GameBaseCamera player_camera_; // 玩家摄像机

		KeyboardMovementController keyboard_move_controller_;
		MouseRotateController mouse_rotate_controller_;

#pragma region 玩家物理组件相关参数

		btTransform player_collider_transform_{};
		btScalar player_collider_mass_{1.0f};
		btVector3 player_collider_local_inertia{};

		btCollisionShape* player_collision_;
		btDefaultMotionState* player_motion_state_;
		btRigidBody* player_rigidbody_;

		BlockGenerateData player_collider_data_ =
		{
			character_game_obj_.transform_.translation, // 位置设在出生点
			character_game_obj_.transform_.rotation,
			character_game_obj_.transform_.scale,

			1.0f // 质量
		};

		btVector3 player_collider_size_ = // 尺寸
		{
			half_width_,
			half_height_,
			half_width_
		};

		btVector3 player_collider_origin_ = // collider 出生点
		{
			spawn_point_.x,
			spawn_point_.y,
			spawn_point_.z
		};

		PhysicsObjectSaveData player_physics_data_ {};
	
#pragma endregion

	};
}
