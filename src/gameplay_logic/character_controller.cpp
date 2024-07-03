
#include "character_controller.h"

namespace vulkancraft
{
	CharacterController::CharacterController()
	{
		try
		{
			game_object_manager_ = GameObjectManager::get_instance();
		}
		catch (std::exception& e)
		{
			std::cerr << "Character Controller 上的某个单例类创建失败：" << e.what() << std::endl;
		}
	}

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
camera_game_obj_.transform_.translation = spawn_point_ + glm::vec3{ 0, -character_height_, 0 };
camera_game_obj_.transform_.rotation = character_game_obj_.transform_.rotation; // 旋转和玩家角色保持一致
camera_game_obj_.transform_.scale = { 1, 1, 1 }; // 相机的缩放不用改变

character_collider_.set_id(character_game_obj_.get_id());
character_collider_.collider_transform_component_.translation = character_game_obj_.transform_.translation; // 位置一样
character_collider_.set_character_collider_size(character_height_, character_width_);

print_player_details();
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
			<< -camera_game_obj_.transform_.translation.y << ", "
			<< camera_game_obj_.transform_.translation.z << std::endl;

		std::cout << "玩家摄像机的旋转角度为："
			<< camera_game_obj_.transform_.rotation.x << ", "
			<< camera_game_obj_.transform_.rotation.y << ", "
			<< camera_game_obj_.transform_.rotation.z << std::endl;

		std::cout << "玩家 Collider 的旋转角度为："
			<< character_collider_.collider_transform_component_.rotation.x << ", "
			<< character_collider_.collider_transform_component_.rotation.y << ", "
			<< character_collider_.collider_transform_component_.rotation.z << std::endl;

		std::cout << "玩家 Collider 的最小，最大顶点分别在："
			<< character_collider_.get_aabb_range().first.x << ", "
			<< character_collider_.get_aabb_range().first.y << ", "
			<< character_collider_.get_aabb_range().first.z << " 和 "
			<< character_collider_.get_aabb_range().second.x << ", "
			<< character_collider_.get_aabb_range().second.y << ", "
			<< character_collider_.get_aabb_range().second.z << std::endl;
	}

	void CharacterController::move(float fixed_delta_time, GLFWwindow* glfw_window)
	{
		// TODO: 这里应该可以设置玩家移动速度，视角转换速度

		// 键盘只控制移动，不控制玩家旋转
		keyboard_move_controller_.player_move(glfw_window, fixed_delta_time, character_game_obj_);
		camera_game_obj_.transform_.translation = character_game_obj_.transform_.translation + glm::vec3{ 0, -character_height_, 0 };

		// 同步 AABB Collider，旋转不用同步
		character_collider_.collider_transform_component_.translation = character_game_obj_.transform_.translation;
		character_collider_.collider_transform_component_.scale = character_game_obj_.transform_.scale;
		character_collider_.set_character_collider_size(character_height_, character_width_);

		camera_game_obj_.transform_.scale = { 1, 1, 1 }; // 相机的缩放不用改变
	}

	void CharacterController::init_mouse_rotate(GLFWwindow* glfw_window)
	{
		mouse_rotate_controller_.initialize(glfw_window);
	}

	void CharacterController::rotate(float fixed_delta_time, GLFWwindow* glfw_window)
	{
		mouse_rotate_controller_.rotate_control(glfw_window, fixed_delta_time, character_game_obj_, kRotateAll);

		camera_game_obj_.transform_.translation = character_game_obj_.transform_.translation + glm::vec3{ 0, -character_height_, 0 };
		camera_game_obj_.transform_.rotation = character_game_obj_.transform_.rotation;
		camera_game_obj_.transform_.scale = { 1, 1, 1 }; // 相机的缩放不用改变
	}

	void CharacterController::update_player_physics(float delta_time)
	{
		character_rigidbody_.ground_check(character_collider_, delta_time, ground_check_max_length_); // 地面检测

		// 计算重力下落
		character_rigidbody_.free_falling(delta_time, character_game_obj_.transform_.translation);
		character_rigidbody_.free_falling(delta_time, camera_game_obj_.transform_.translation);
	}

	void CharacterController::update_player_collision()
	{
		for (int i = 0; i < game_object_manager_->get_physical_obj_vector().size(); i++)
		{
			if (character_collider_.is_two_aabb_touching(game_object_manager_->get_physical_obj_vector()[i]->aabb_collider_))
			{
				handle_collision(game_object_manager_->get_physical_obj_vector()[i]->aabb_collider_);
			}
		}
	}

	void CharacterController::handle_collision(AABBCollider& wall_collider)
	{
		// 获取角色当前位置
		glm::vec3 player_collider_pos = character_collider_.collider_transform_component_.translation;

		// 获取墙体的最小和最大坐标
		glm::vec3 wall_collider_min = wall_collider.get_aabb_range().first;
		glm::vec3 wall_max = wall_collider.get_aabb_range().second;

		if (player_collider_pos.x <= wall_collider_min.x)
		{
			player_collider_pos.x = wall_collider_min.x + std::numeric_limits<float>::epsilon();
			std::cout << "玩家与某 Collider 相切了" << std::endl;
		}
		else if (player_collider_pos.x >= wall_max.x)
		{
			player_collider_pos.x = wall_max.x - std::numeric_limits<float>::epsilon();
			std::cout << "玩家与某 Collider 相切了" << std::endl;
		}
		else if (player_collider_pos.z <= wall_collider_min.z)
		{
			player_collider_pos.z = wall_collider_min.z + std::numeric_limits<float>::epsilon();
			std::cout << "玩家与某 Collider 相切了" << std::endl;
		}
		else if (player_collider_pos.z >= wall_max.z)
		{
			player_collider_pos.z = wall_max.z - std::numeric_limits<float>::epsilon();
			std::cout << "玩家与某 Collider 相切了" << std::endl;
		}

		//// 更新角色位置
		//character_collider_.collider_transform_component_.translation = player_collider_pos;
		//character_game_obj_.transform_.translation = player_collider_pos;
		//camera_game_obj_.transform_.translation = player_collider_pos;
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
