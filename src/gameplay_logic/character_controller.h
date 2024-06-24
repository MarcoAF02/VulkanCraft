
#pragma once

#include "../gameplay_logic/game_camera.h"
#include "../game_physics_system/physics_aabb.h"
#include "../game_physics_system/rigidbody_system.h"
#include "../input_system/keyboard_input_controller.h"
#include "../input_system/mouse_rotate_controller.h"

// std
#include <iostream>

// glm ��ѧ��
#include <glm/glm.hpp>

struct vulkancraft::TransformComponent;
class vulkancraft::AABBCollider; // AABB ��ײ��
class vulkancraft::Rigidbody; // ����ϵͳ
class vulkancraft::KeyboardMovementController; // ���̿�����
class vulkancraft::MouseRotateController; // �����ת������

namespace vulkancraft
{
	// ��������Ұ�Ĳ���
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

		void init_character_controller(glm::vec3 spawn_point);

		// ��������۲��ߵ�λ��
		void update_camera_viewer_transform();

		// ��������������ת��ֻ���������������ת
		void move(float fixed_delta_time, GLFWwindow* glfw_window);
		void rotate(float fixed_delta_time, GLFWwindow* glfw_window);

		// TODO: ��������ͷ����
		void update_player_collision(float delta_time); // �����������

		// �õ�����۲���������ֻ꣨������Ϳ����ˣ����㹻���������λ�ã�
		TransformComponent get_camera_viewer_transform();

#pragma region DEBUG �ú���

		// ����ҵ���ߣ���ײ���ȫ��ӡ����
		void print_player_details() const;

#pragma endregion

	private:

		// ����ĸ߶�Ϊ 1.8 ���Ϊ 0.6
		// ���������Ҫ AABB Collider����ҽ�ɫ��Ҫ

		float character_height_ = 1.8f; // ��������
		float character_width_ = 0.6f; // ����Ŀ��

		// TODO: �� Camera ��������
		glm::vec3 spawn_point_ = glm::vec3 {0, 0, 0};
		TransformComponent camera_viewer_transform_;

		// GameBaseCamera ���� Render App �У����ܷ�������
		BaseGameObject character_game_obj_; // ����������ת��ֻ��������ת
		BaseGameObject camera_game_obj_; // ������õ���Ϸ���壬���ڸ������������ת

		KeyboardMovementController keyboard_move_controller_;
		MouseRotateController mouse_rotate_controller_;

		AABBCollider character_collider_ =
		{
			character_game_obj_.transform_,
			character_game_obj_.get_id()
		};

		// TODO: �������Ҫ���������߳���ѭ��
		Rigidbody character_rigidbody_;
	};
}
