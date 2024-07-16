
#include "character_controller.h"

namespace vulkancraft
{
	CharacterController::CharacterController() { }
	CharacterController::~CharacterController() { }

	GameBaseCamera& CharacterController::get_player_camera() { return player_camera_; }

	void CharacterController::init_character_controller()
	{
		character_game_obj_ = BaseGameObject::create_game_object(false); // ������ɫ��Ϸ����
		camera_game_obj_ = BaseGameObject::create_game_object(false); // �����������Ϸ����

		character_game_obj_.directly_set_id(player_id_);

		// ��ʼ�����վλ
		character_game_obj_.transform_.translation = spawn_point_;
		character_game_obj_.transform_.rotation = { 0, 0, 0 };
		character_game_obj_.transform_.scale = { 1, 1, 1 }; // ��ҵ����Ų��øı�

		// ��ʼ���������վλ
		camera_game_obj_.transform_.translation = spawn_point_ + glm::vec3{ 0, -camera_height_, 0 };
		camera_game_obj_.transform_.rotation = character_game_obj_.transform_.rotation; // ��ת����ҽ�ɫ����һ��
		camera_game_obj_.transform_.scale = { 1, 1, 1 }; // ��������Ų��øı�

		print_player_details();
	}

	void CharacterController::init_character_physics()
	{
		player_collision_ = new btBoxShape(player_collider_size_); // ȷ����С
		player_collider_transform_.setIdentity();
		bool is_dynamic = (player_collider_mass_ != 0.0f);

		// ����Ƕ�̬���壬����������������
		if (is_dynamic) player_collision_->calculateLocalInertia(player_collider_mass_, player_collider_local_inertia);
		player_collider_transform_.setOrigin(player_collider_origin_ + btVector3(0, -0.9f, 0));

		player_motion_state_ = new btDefaultMotionState(player_collider_transform_);
		btRigidBody::btRigidBodyConstructionInfo rb_info(player_collider_mass_, player_motion_state_, player_collision_, player_collider_local_inertia);
		player_rigidbody_ = new btRigidBody(rb_info);

		player_rigidbody_->setAngularFactor(btVector3(0, 0, 0)); // ��ֹ���������ϵ���ת

		player_physics_data_.rigid_body = player_rigidbody_;
		player_physics_data_.collision_shape = player_collision_;

		// δ���������������壬��Ҫ��������������Ⱦ
		// �˴������� character_game_obj ������Ȩ
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
		// TODO: ��ӡ Collider �����������ת

		// ���״̬
		std::cout << "��ҳ�����Ϊ��"
			<< spawn_point_.x << ", "
			<< spawn_point_.y << ", "
			<< spawn_point_.z << std::endl;

		std::cout << "��ҵ�ǰ�ڵĵ�λΪ��"
			<< character_game_obj_.transform_.translation.x << ", "
			<< character_game_obj_.transform_.translation.y << ", "
			<< character_game_obj_.transform_.translation.z << std::endl;

		std::cout << "��ҵ�ǰ����ת�Ƕ�Ϊ��"
			<< character_game_obj_.transform_.rotation.x << ", "
			<< character_game_obj_.transform_.rotation.y << ", "
			<< character_game_obj_.transform_.rotation.z << std::endl;

		std::cout << "��������������Ϊ��"
			<< camera_game_obj_.transform_.translation.x << ", "
			<< camera_game_obj_.transform_.translation.y << ", "
			<< camera_game_obj_.transform_.translation.z << std::endl;

		std::cout << "������������ת�Ƕ�Ϊ��"
			<< camera_game_obj_.transform_.rotation.x << ", "
			<< camera_game_obj_.transform_.rotation.y << ", "
			<< camera_game_obj_.transform_.rotation.z << std::endl;
	}

	void CharacterController::move(float fixed_delta_time, GLFWwindow* glfw_window)
	{
		// TODO: ����Ӧ�ÿ�����������ƶ��ٶȣ��ӽ�ת���ٶ�
		// TODO: ��Ҫ��������⣺����ʧ�����ƶ�����Ͳ�������

		// ����ֻ�����ƶ��������������ת
		keyboard_move_controller_.player_move(glfw_window, fixed_delta_time, character_game_obj_, player_rigidbody_);
		camera_game_obj_.transform_.translation = character_game_obj_.transform_.translation + glm::vec3{ 0, -camera_height_, 0 };

		camera_game_obj_.transform_.scale = { 1, 1, 1 }; // ��������Ų��øı�
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
		camera_game_obj_.transform_.scale = { 1, 1, 1 }; // ��������Ų��øı�
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

		//// ��ӡ AABB ����С�����
		//std::cout << "Bottom Y: " << aabb_min.y() << ", Top Y: " << aabb_max.y() << std::endl;
		//std::cout << "Bottom Z: " << aabb_min.z() << ", Top Z: " << aabb_max.z() << std::endl;
	}

	void CharacterController::set_player_camera(PlayerCameraView camera_view)
	{
		// ����������۲����
		player_camera_.set_view_yxz(camera_game_obj_.transform_.translation, camera_game_obj_.transform_.rotation);

		// �����������ͼ
		player_camera_.set_perspective_projection
		(
			camera_view.fovy,
			camera_view.aspect,
			camera_view.near,
			camera_view.far
		);
	}

}
