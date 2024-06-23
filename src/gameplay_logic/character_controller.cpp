
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
		character_game_obj_ = BaseGameObject::create_game_object(false); // ������ɫ��Ϸ����
		camera_game_obj_ = BaseGameObject::create_game_object(false); // �����������Ϸ����

		// ��ʼ�����վλ
		character_game_obj_.transform_.translation = spawn_point;
		character_game_obj_.transform_.rotation = { 0, 0, 0 };

		// ����Ϊ��ҽ�ɫ��߲���
		character_game_obj_.transform_.scale = { character_width_, character_height_, character_width_ };

		// ��ʼ���������վλ
		camera_game_obj_.transform_.translation = spawn_point + glm::vec3{ 0, character_height_, 0 };
		camera_game_obj_.transform_.rotation = character_game_obj_.transform_.rotation; // ��ת����ҽ�ɫ����һ��
		camera_game_obj_.transform_.scale = { 1, 1, 1 }; // ��������Ų��øı�

		// ��ʼ�� AABB Collider��������Բ��ô���
		character_collider_.set_id(character_game_obj_.get_id());
		character_collider_.collider_transform_component_ = character_game_obj_.transform_;

		// ����������۲����
		fps_camera_.set_perspective_projection(camera_view.fovy, camera_view.aspect, camera_view.near, camera_view.far);

		// ����������۲����
		fps_camera_.set_view_yxz(camera_game_obj_.transform_.translation, camera_game_obj_.transform_.rotation);
	}

	void CharacterController::move(float fixed_delta_time)
	{
		// TODO: ���������������ƶ��ٶȣ��ӽ�ת���ٶ�
		// ����ֻ�����ƶ��������������ת
		keyboard_move_controller_.player_move(glfw_window_, fixed_delta_time, character_game_obj_);
		keyboard_move_controller_.player_move(glfw_window_, fixed_delta_time, camera_game_obj_);

		// ͬ�� AABB Collider����ת����ͬ��
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
