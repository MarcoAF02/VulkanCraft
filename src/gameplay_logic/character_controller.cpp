
#include "character_controller.h"

namespace vulkancraft
{
	CharacterController::CharacterController() { }
	CharacterController::~CharacterController() { }

	TransformComponent CharacterController::get_camera_viewer_transform() {return camera_viewer_transform_;}

	void CharacterController::init_character_controller(glm::vec3 spawn_point)
	{
		spawn_point_ = spawn_point;

		character_game_obj_ = BaseGameObject::create_game_object(false); // ������ɫ��Ϸ����
		camera_game_obj_ = BaseGameObject::create_game_object(false); // �����������Ϸ����

		// ��ʼ�����վλ
		character_game_obj_.transform_.translation = spawn_point_;
		character_game_obj_.transform_.rotation = { 0, 0, 0 };

		// ����Ϊ��ҽ�ɫ��߲���
		character_game_obj_.transform_.scale = { character_width_, character_height_, character_width_ };

		// ��ʼ���������վλ
		camera_game_obj_.transform_.translation = spawn_point_ + glm::vec3{ 0, character_height_, 0 };
		camera_game_obj_.transform_.rotation = character_game_obj_.transform_.rotation; // ��ת����ҽ�ɫ����һ��
		camera_game_obj_.transform_.scale = { 1, 1, 1 }; // ��������Ų��øı�

		// ��ʼ�� AABB Collider��������Բ��ô���

		// TODO: ������������ AABB Collider �ĳߴ�
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
		// ���״̬
		printf("��ҳ�����Ϊ��%.2f %.2f %.2f\n", spawn_point_.x, spawn_point_.y, spawn_point_.z);
		printf("��ҵ�ǰ�ڵĵ�λΪ��%.2f %.2f %.2f\n", character_game_obj_.transform_.translation.x, character_game_obj_.transform_.translation.y, character_game_obj_.transform_.translation.z);
		printf("��ҵ�ǰ����ת�Ƕ�Ϊ��%.2f %.2f %.2f\n", character_game_obj_.transform_.rotation.x, character_game_obj_.transform_.rotation.y, character_game_obj_.transform_.rotation.z);

		printf("��������������Ϊ %.2f %.2f %.2f\n", camera_game_obj_.transform_.translation.x, camera_game_obj_.transform_.translation.y, camera_game_obj_.transform_.translation.z);

		std::cout << "��� Collider ����������ֱ��ڣ�"
			<< character_collider_.get_aabb_range().first.x << ", "
			<< character_collider_.get_aabb_range().first.y << ", "
			<< character_collider_.get_aabb_range().first.z << " �� "
			<< character_collider_.get_aabb_range().second.x << ", "
			<< character_collider_.get_aabb_range().second.y << ", "
			<< character_collider_.get_aabb_range().second.z << std::endl;
	}

	void CharacterController::move(float fixed_delta_time, GLFWwindow* glfw_window)
	{
		// TODO: ���������������ƶ��ٶȣ��ӽ�ת���ٶ�
		// ����ֻ�����ƶ��������������ת
		keyboard_move_controller_.player_move(glfw_window, fixed_delta_time, character_game_obj_);
		keyboard_move_controller_.player_move(glfw_window, fixed_delta_time, camera_game_obj_);

		// ͬ�� AABB Collider����ת����ͬ��
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
