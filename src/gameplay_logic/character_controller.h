
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

// TODO List:
// 1. 玩家控制器实现地面检测
// 2. 玩家控制器重力下落，配合 1 实现落在地上
// 3. 玩家撞墙时停止移动，可以通过获取碰撞目标 Collider 面的法线实现
// 4. 玩家跳跃，跳跃顶头取消向上的向量重新下落
// 5. 移动插值平滑阻尼和撞墙重置移动速度
// 6. 思考怎么实现材质系统，不同的方块显示不同的纹理

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
		void init_character_controller(glm::vec3 spawn_point);

		// 设置并更新玩家主相机
		void set_player_camera(PlayerCameraView camera_view);

		// 得到玩家观察相机
		GameBaseCamera& get_player_camera();

		// 初始化鼠标视角控制器
		void init_mouse_rotate(GLFWwindow* glfw_window);

		// 玩家身体会左右旋转，只有摄像机会上下旋转
		void move(float fixed_delta_time, GLFWwindow* glfw_window);
		void rotate(float fixed_delta_time, GLFWwindow* glfw_window);

		// TODO: 跳起来顶头回落
		void update_player_collision(float delta_time); // 自由落体更新

#pragma region DEBUG 用函数

		// 把玩家的身高，碰撞体等全打印出来
		void print_player_details() const;

#pragma endregion

	private:

		// 人类的高度为 1.8 宽度为 0.6
		// 摄像机不需要 AABB Collider，玩家角色需要

		float character_height_ = 1.8f; // 人类的身高
		float character_width_ = 0.6f; // 人类的宽度

		glm::vec3 spawn_point_ = glm::vec3 {0, 0, 0};

		BaseGameObject character_game_obj_; // 不能上下旋转，只能左右旋转
		BaseGameObject camera_game_obj_; // 摄像机用的观察游戏物体，用于给摄像机上下旋转

		GameBaseCamera player_camera_; // 玩家摄像机

		KeyboardMovementController keyboard_move_controller_;
		MouseRotateController mouse_rotate_controller_;

		AABBCollider character_collider_ =
		{
			true,
			character_game_obj_.transform_,
			character_game_obj_.get_id(),
			false,
			character_height_,
			character_width_
		};

		// TODO: 这个东西要放在物理线程中循环
		Rigidbody character_rigidbody_;
	};
}
