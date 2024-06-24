
#include "character_controller.h"

namespace vulkancraft
{
	CharacterController::CharacterController() { }
	CharacterController::~CharacterController() { }

	TransformComponent CharacterController::get_camera_viewer_transform() {return camera_viewer_transform_;}

	void CharacterController::init_character_controller(glm::vec3 spawn_point)
	{
		spawn_point_ = spawn_point;

		character_game_obj_ = BaseGameObject::create_game_object(false); // 创建角色游戏物体
		camera_game_obj_ = BaseGameObject::create_game_object(false); // 创建摄像机游戏物体

		// 初始化玩家站位
		character_game_obj_.transform_.translation = spawn_point_;
		character_game_obj_.transform_.rotation = { 0, 0, 0 };

		// 缩放为玩家角色身高参数
		character_game_obj_.transform_.scale = { character_width_, character_height_, character_width_ };

		// 初始化相机物体站位
		camera_game_obj_.transform_.translation = spawn_point_ + glm::vec3{ 0, character_height_, 0 };
		camera_game_obj_.transform_.rotation = character_game_obj_.transform_.rotation; // 旋转和玩家角色保持一致
		camera_game_obj_.transform_.scale = { 1, 1, 1 }; // 相机的缩放不用改变

		// 初始化 AABB Collider，这个可以不用存入

		// TODO: 这里重新设置 AABB Collider 的尺寸
		character_collider_.set_id(character_game_obj_.get_id());
		character_collider_.collider_transform_component_ = character_game_obj_.transform_;

		print_player_details();
	}

	void CharacterController::update_camera_viewer_transform()
	{
		camera_viewer_transform_ = camera_game_obj_.transform_;
	}

	void CharacterController::print_player_details() const
	{
		// 玩家状态
		printf("玩家出生点为：%.2f %.2f %.2f\n", spawn_point_.x, spawn_point_.y, spawn_point_.z);
		printf("玩家当前在的点位为：%.2f %.2f %.2f\n", character_game_obj_.transform_.translation.x, character_game_obj_.transform_.translation.y, character_game_obj_.transform_.translation.z);
		printf("玩家当前的旋转角度为：%.2f %.2f %.2f\n", character_game_obj_.transform_.rotation.x, character_game_obj_.transform_.rotation.y, character_game_obj_.transform_.rotation.z);

		printf("玩家摄像机的坐标为 %.2f %.2f %.2f\n", camera_game_obj_.transform_.translation.x, camera_game_obj_.transform_.translation.y, camera_game_obj_.transform_.translation.z);

		std::cout << "玩家 Collider 的两个顶点分别在："
			<< character_collider_.get_aabb_range().first.x << ", "
			<< character_collider_.get_aabb_range().first.y << ", "
			<< character_collider_.get_aabb_range().first.z << " 和 "
			<< character_collider_.get_aabb_range().second.x << ", "
			<< character_collider_.get_aabb_range().second.y << ", "
			<< character_collider_.get_aabb_range().second.z << std::endl;
	}

	void CharacterController::move(float fixed_delta_time, GLFWwindow* glfw_window)
	{
		// TODO: 这里可以设置玩家移动速度，视角转换速度
		// 键盘只控制移动，不控制玩家旋转
		keyboard_move_controller_.player_move(glfw_window, fixed_delta_time, character_game_obj_);
		keyboard_move_controller_.player_move(glfw_window, fixed_delta_time, camera_game_obj_);

		// 同步 AABB Collider，旋转不用同步
		character_collider_.collider_transform_component_.translation = character_game_obj_.transform_.translation;
		character_collider_.collider_transform_component_.scale = character_game_obj_.transform_.scale;
	}

	void CharacterController::rotate(float fixed_delta_time, GLFWwindow* glfw_window)
	{
		mouse_rotate_controller_.rotate_control(glfw_window, fixed_delta_time, character_game_obj_, kRotateY);
		mouse_rotate_controller_.rotate_control(glfw_window, fixed_delta_time, camera_game_obj_, kRotateX);
	}

	void CharacterController::update_player_collision(float delta_time)
	{
		character_rigidbody_.ground_check(character_collider_, delta_time);
		character_rigidbody_.free_falling(delta_time);
	}

}
