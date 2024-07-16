
#pragma once

#include "../gameplay_logic/game_camera.h"
#include "../input_system/keyboard_input_controller.h"
#include "../input_system/mouse_rotate_controller.h"
#include "../base_tools/details.h"

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
		void init_character_controller(glm::vec3 spawn_point);
		void init_character_physics(BaseGameObject::RenderAppObjMap& game_object_map, std::unordered_map<BaseGameObject::id_t, PhysicsObjectSaveData>& physics_obj_map); // 初始化玩家物理系统

		// 设置并更新玩家主相机
		void set_player_camera(PlayerCameraView camera_view);

		// 得到玩家观察相机
		GameBaseCamera& get_player_camera();

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
		float camera_height_ = 1.65f; // 摄像机的高度

		// HACK: 这个别改，保持 0.1f...
		float ground_check_max_length_ = 0.1f; // 脚底射线检测的长度
		float wall_check_max_length_ = 5.0f; // 墙面射线检测的长度

		glm::vec3 spawn_point_ = glm::vec3{ 0, 0, 0 };

		BaseGameObject character_game_obj_; // 不能上下旋转，只能左右旋转
		BaseGameObject camera_game_obj_; // 摄像机用的观察游戏物体，用于给摄像机上下旋转

		GameBaseCamera player_camera_; // 玩家摄像机

		KeyboardMovementController keyboard_move_controller_;
		MouseRotateController mouse_rotate_controller_;
	};
}
