
#include "character_controller.h"

namespace vulkancraft
{
	CharacterController::CharacterController(GLFWwindow* glfw_window)
	{
		glfw_window_ = glfw_window;
	}

	CharacterController::~CharacterController() { }

	void CharacterController::init_character_controller(glm::vec3 spawn_point, PlayerCameraView camera_view)
	{
		character_game_obj_ = BaseGameObject::create_game_object(false); // 创建角色游戏物体
		camera_game_obj_ = BaseGameObject::create_game_object(false); // 创建摄像机游戏物体

		// 初始化玩家站位
		character_game_obj_.transform_.translation = spawn_point;
		character_game_obj_.transform_.rotation = { 0, 0, 0 };

		// 缩放为玩家角色身高参数
		character_game_obj_.transform_.scale = { character_width_, character_height_, character_width_ };

		// 初始化相机物体站位
		camera_game_obj_.transform_.translation = spawn_point + glm::vec3{ 0, character_height_, 0 };
		camera_game_obj_.transform_.rotation = character_game_obj_.transform_.rotation; // 旋转和玩家角色保持一致
		camera_game_obj_.transform_.scale = { 1, 1, 1 }; // 相机的缩放不用改变

		// 初始化 AABB Collider，这个可以不用存入
		character_collider_.set_id(character_game_obj_.get_id());
		character_collider_.collider_transform_component_ = character_game_obj_.transform_;

		// 设置摄像机观察矩阵
		fps_camera_.set_perspective_projection(camera_view.fovy, camera_view.aspect, camera_view.near, camera_view.far);

		// 关联摄像机观察对象
		fps_camera_.set_view_yxz(camera_game_obj_.transform_.translation, camera_game_obj_.transform_.rotation);
	}

	void CharacterController::move(float fixed_delta_time)
	{
		// TODO: 这里可以设置玩家移动速度，视角转换速度
		// 键盘只控制移动，不控制玩家旋转
		keyboard_move_controller_.player_move(glfw_window_, fixed_delta_time, character_game_obj_);
		keyboard_move_controller_.player_move(glfw_window_, fixed_delta_time, camera_game_obj_);

		// 同步 AABB Collider，旋转不用同步
		character_collider_.collider_transform_component_.translation = character_game_obj_.transform_.translation;
		character_collider_.collider_transform_component_.scale = character_game_obj_.transform_.scale;
	}

	void CharacterController::rotate(float fixed_delta_time)
	{
		mouse_rotate_controller_.rotate_control(glfw_window_, fixed_delta_time, character_game_obj_, kRotateY);
		mouse_rotate_controller_.rotate_control(glfw_window_, fixed_delta_time, camera_game_obj_, kRotateX);
	}

	void CharacterController::update_player_collision(float delta_time)
	{
		character_rigidbody_.ground_check(character_collider_, delta_time);
		character_rigidbody_.free_falling(delta_time);
	}

}
