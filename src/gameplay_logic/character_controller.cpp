
#include "character_controller.h"

namespace vulkancraft
{
	CharacterController::CharacterController() { }
	CharacterController::~CharacterController() { }

	GameBaseCamera& CharacterController::get_player_camera() { return player_camera_; }

	void CharacterController::init_character_controller(glm::vec3 spawn_point)
	{
		spawn_point_ = spawn_point;

		character_game_obj_ = BaseGameObject::create_game_object(false); // 创建角色游戏物体
		camera_game_obj_ = BaseGameObject::create_game_object(false); // 创建摄像机游戏物体

		// 初始化玩家站位
		character_game_obj_.transform_.translation = spawn_point_;
		character_game_obj_.transform_.rotation = { 0, 0, 0 };
		character_game_obj_.transform_.scale = { 1, 1, 1 }; // 玩家的缩放不用改变

		// 初始化相机物体站位
		camera_game_obj_.transform_.translation = spawn_point_ + glm::vec3{ 0, -camera_height_, 0 };
		camera_game_obj_.transform_.rotation = character_game_obj_.transform_.rotation; // 旋转和玩家角色保持一致
		camera_game_obj_.transform_.scale = { 1, 1, 1 }; // 相机的缩放不用改变

		print_player_details();
	}

	void CharacterController::init_character_physics(BaseGameObject::RenderAppObjMap& game_object_map, std::unordered_map<BaseGameObject::id_t, PhysicsObjectSaveData>& physics_obj_map)
	{
		float half_height = character_height_ / 2;
		float half_width = character_width_ / 2;

		BlockGenerateData collider_data =
		{
			character_game_obj_.transform_.translation, // 位置设在出生点
			character_game_obj_.transform_.rotation,
			character_game_obj_.transform_.scale,

			1.0f // 质量
		};


	}

	void CharacterController::print_player_details() const
	{
		// TODO: 打印 Collider 和摄像机的旋转

		// 玩家状态
		std::cout << "玩家出生点为："
			<< spawn_point_.x << ", "
			<< spawn_point_.y << ", "
			<< spawn_point_.z << std::endl;

		std::cout << "玩家当前在的点位为："
			<< character_game_obj_.transform_.translation.x << ", "
			<< character_game_obj_.transform_.translation.y << ", "
			<< character_game_obj_.transform_.translation.z << std::endl;

		std::cout << "玩家当前的旋转角度为："
			<< character_game_obj_.transform_.rotation.x << ", "
			<< character_game_obj_.transform_.rotation.y << ", "
			<< character_game_obj_.transform_.rotation.z << std::endl;

		std::cout << "玩家摄像机的坐标为："
			<< camera_game_obj_.transform_.translation.x << ", "
			<< camera_game_obj_.transform_.translation.y << ", "
			<< camera_game_obj_.transform_.translation.z << std::endl;

		std::cout << "玩家摄像机的旋转角度为："
			<< camera_game_obj_.transform_.rotation.x << ", "
			<< camera_game_obj_.transform_.rotation.y << ", "
			<< camera_game_obj_.transform_.rotation.z << std::endl;
	}

	void CharacterController::move(float fixed_delta_time, GLFWwindow* glfw_window)
	{
		// TODO: 这里应该可以设置玩家移动速度，视角转换速度

		// 键盘只控制移动，不控制玩家旋转
		keyboard_move_controller_.player_move(glfw_window, fixed_delta_time, character_game_obj_);
		camera_game_obj_.transform_.translation = character_game_obj_.transform_.translation + glm::vec3{ 0, -camera_height_, 0 };

		camera_game_obj_.transform_.scale = { 1, 1, 1 }; // 相机的缩放不用改变
	}

	void CharacterController::init_mouse_rotate(GLFWwindow* glfw_window)
	{
		mouse_rotate_controller_.initialize(glfw_window);
	}

	void CharacterController::rotate(float fixed_delta_time, GLFWwindow* glfw_window)
	{
		mouse_rotate_controller_.rotate_control(glfw_window, fixed_delta_time, character_game_obj_, kRotateAll);

		camera_game_obj_.transform_.translation = character_game_obj_.transform_.translation + glm::vec3{ 0, -camera_height_, 0 };
		camera_game_obj_.transform_.rotation = character_game_obj_.transform_.rotation;
		camera_game_obj_.transform_.scale = { 1, 1, 1 }; // 相机的缩放不用改变
	}

	void CharacterController::set_player_camera(PlayerCameraView camera_view)
	{
		// 关联摄像机观测对象
		player_camera_.set_view_yxz(camera_game_obj_.transform_.translation, camera_game_obj_.transform_.rotation);

		// 设置摄像机视图
		player_camera_.set_perspective_projection
		(
			camera_view.fovy,
			camera_view.aspect,
			camera_view.near,
			camera_view.far
		);
	}

}
