
#include "character_controller.h"

namespace vulkancraft
{
	CharacterController::CharacterController() { }
	CharacterController::~CharacterController() { }

	GameBaseCamera& CharacterController::get_player_camera() { return player_camera_; }

	void CharacterController::init_character_controller(glm::vec3 spawn_point)
	{
		spawn_point_ = spawn_point;

		character_game_obj_ = BaseGameObject::create_game_object(false); // ������ɫ��Ϸ����
		camera_game_obj_ = BaseGameObject::create_game_object(false); // �����������Ϸ����

		// ��ʼ�����վλ
		character_game_obj_.transform_.translation = spawn_point_;
		character_game_obj_.transform_.rotation = { 0, 0, 0 };
		character_game_obj_.transform_.scale = { 1, 1, 1 }; // ��ҵ����Ų��øı�

		// ��ʼ���������վλ
		camera_game_obj_.transform_.translation = spawn_point_ + glm::vec3{ 0, -character_height_, 0 };
		camera_game_obj_.transform_.rotation = character_game_obj_.transform_.rotation; // ��ת����ҽ�ɫ����һ��
		camera_game_obj_.transform_.scale = { 1, 1, 1 }; // ��������Ų��øı�

		character_collider_.set_id(character_game_obj_.get_id());
		character_collider_.collider_transform_component_.translation = character_game_obj_.transform_.translation; // λ��һ��
		character_collider_.set_character_collider_size(character_height_, character_width_);

		print_player_details();
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
			<< -camera_game_obj_.transform_.translation.y << ", "
			<< camera_game_obj_.transform_.translation.z << std::endl;

		std::cout << "������������ת�Ƕ�Ϊ��"
			<< camera_game_obj_.transform_.rotation.x << ", "
			<< camera_game_obj_.transform_.rotation.y << ", "
			<< camera_game_obj_.transform_.rotation.z << std::endl;

		std::cout << "��� Collider ����ת�Ƕ�Ϊ��"
			<< character_collider_.collider_transform_component_.rotation.x << ", "
			<< character_collider_.collider_transform_component_.rotation.y << ", "
			<< character_collider_.collider_transform_component_.rotation.z << std::endl;

		std::cout << "��� Collider ����С����󶥵�ֱ��ڣ�"
			<< character_collider_.get_aabb_range().first.x << ", "
			<< character_collider_.get_aabb_range().first.y << ", "
			<< character_collider_.get_aabb_range().first.z << " �� "
			<< character_collider_.get_aabb_range().second.x << ", "
			<< character_collider_.get_aabb_range().second.y << ", "
			<< character_collider_.get_aabb_range().second.z << std::endl;
	}

	void CharacterController::move(float fixed_delta_time, GLFWwindow* glfw_window)
	{
		// TODO: ����Ӧ�ÿ�����������ƶ��ٶȣ��ӽ�ת���ٶ�
		
		// ����ֻ�����ƶ��������������ת
		keyboard_move_controller_.player_move(glfw_window, fixed_delta_time, character_game_obj_);
		camera_game_obj_.transform_.translation = character_game_obj_.transform_.translation + glm::vec3{ 0, -character_height_, 0 };

		// ͬ�� AABB Collider����ת����ͬ��
		character_collider_.collider_transform_component_.translation = character_game_obj_.transform_.translation;
		character_collider_.collider_transform_component_.scale = character_game_obj_.transform_.scale;
		character_collider_.set_character_collider_size(character_height_, character_width_);

		camera_game_obj_.transform_.scale = { 1, 1, 1 }; // ��������Ų��øı�
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
		camera_game_obj_.transform_.scale = { 1, 1, 1 }; // ��������Ų��øı�
	}

	void CharacterController::update_player_physics(float delta_time)
	{
		character_rigidbody_.ground_check(character_collider_, delta_time, ground_check_max_length_); // ������

		// ������������
		character_rigidbody_.free_falling(delta_time, character_game_obj_.transform_.translation);
		character_rigidbody_.free_falling(delta_time, camera_game_obj_.transform_.translation);
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
