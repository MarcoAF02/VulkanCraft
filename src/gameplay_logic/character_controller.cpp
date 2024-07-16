
#include "character_controller.h"

namespace vulkancraft
{
	CharacterController::CharacterController() { }
	CharacterController::~CharacterController() { }

	GameBaseCamera& CharacterController::get_player_camera() { return player_camera_; }

	void CharacterController::init_character_controller()
	{
		character_game_obj_ = BaseGameObject::create_game_object(false); // 创建角色游戏物体
		camera_game_obj_ = BaseGameObject::create_game_object(false); // 创建摄像机游戏物体

		character_game_obj_.directly_set_id(player_id_);

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

	void CharacterController::init_character_physics()
	{
		player_collision_ = new btBoxShape(player_collider_size_); // 确定大小
		player_collider_transform_.setIdentity();
		bool is_dynamic = (player_collider_mass_ != 0.0f);

		// 如果是动态物体，设置受力张量矩阵
		if (is_dynamic) player_collision_->calculateLocalInertia(player_collider_mass_, player_collider_local_inertia);
		player_collider_transform_.setOrigin(player_collider_origin_ + btVector3(0, -0.9f, 0));

		player_motion_state_ = new btDefaultMotionState(player_collider_transform_);
		btRigidBody::btRigidBodyConstructionInfo rb_info(player_collider_mass_, player_motion_state_, player_collision_, player_collider_local_inertia);
		player_rigidbody_ = new btRigidBody(rb_info);

		player_rigidbody_->setAngularFactor(btVector3(0, 0, 0)); // 禁止所有轴向上的旋转

		player_physics_data_.rigid_body = player_rigidbody_;
		player_physics_data_.collision_shape = player_collision_;

		// 未来如果有了玩家身体，需要将玩家身体存入渲染
		// 此处不再有 character_game_obj 的所有权
		// render_obj_map.emplace(player_id, std::move(character_game_obj_));
	}

	PhysicsObjectSaveData& CharacterController::get_player_physics_data()
	{
		return player_physics_data_;
	}

	BaseGameObject::id_t CharacterController::get_player_obj_id() const
	{
		return player_id_;
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
		// TODO: 需要解决的问题：窗口失焦后，移动输入就不能用了

		// 键盘只控制移动，不控制玩家旋转
		keyboard_move_controller_.player_move(glfw_window, fixed_delta_time, character_game_obj_, player_rigidbody_);
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

	void CharacterController::sync_player_rigidbody_pos()
	{
		glm::vec3 sync_pos =
		{
			player_rigidbody_->getWorldTransform().getOrigin().getX(),
			player_rigidbody_->getWorldTransform().getOrigin().getY(),
			player_rigidbody_->getWorldTransform().getOrigin().getZ()
		};

		character_game_obj_.transform_.translation = sync_pos;
		camera_game_obj_.transform_.translation = character_game_obj_.transform_.translation + glm::vec3{ 0, -camera_height_, 0 };

		//btVector3 aabb_min, aabb_max;
		//btTransform identity;
		//identity.setIdentity();

		//player_collision_->getAabb(identity, aabb_min, aabb_max);

		//// 打印 AABB 的最小和最大
		//std::cout << "Bottom Y: " << aabb_min.y() << ", Top Y: " << aabb_max.y() << std::endl;
		//std::cout << "Bottom Z: " << aabb_min.z() << ", Top Z: " << aabb_max.z() << std::endl;
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
