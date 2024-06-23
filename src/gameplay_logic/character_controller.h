
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

// TODO: �����д���Ժ�render app �е�һЩ��Ϳ��Բ���Ҫ��

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

		CharacterController(GLFWwindow* glfw_window);
		~CharacterController();

		CharacterController(const CharacterController&) = delete;
		CharacterController& operator = (const CharacterController&) = delete;

		// ��ʼ����ɫ���������ڶ�����������Ļ�������
		void init_character_controller(glm::vec3 spawn_point, PlayerCameraView camera_view);

		// TODO: �����̿�����������������BaseGameObject��AABBCollider��RigidbodySystem Camera Ϊһ��

		// ��������������ת��ֻ���������������ת
		void move(float fixed_delta_time);
		void rotate(float fixed_delta_time);

		// TODO: ��������ͷ����
		void update_player_collision(float delta_time); // �����������

		// ��������͵�����������ģ�ײǽ�ж��� character controller ��

		// �����ᱻι��ʵ�����������������Ⱦ��ģ���̴߳���ǰ��������ʵ�����������ָ�룩��Ϊ�������ݸ����� App

	private:

		// ����ĸ߶�Ϊ 1.8 ���Ϊ 0.6
		// ���������Ҫ AABB Collider����ҽ�ɫ��Ҫ

		float character_height_ = 1.8f; // ��������
		float character_width_ = 0.6f; // ����Ŀ��

		BaseGameObject character_game_obj_; // ����������ת��ֻ��������ת
		BaseGameObject camera_game_obj_; // ������õ���Ϸ���壬���ڸ������������ת
		GameBaseCamera fps_camera_;

		KeyboardMovementController keyboard_move_controller_;

		MouseRotateController mouse_rotate_controller_ =
		{
			glfw_window_
		};

		AABBCollider character_collider_ =
		{
			character_game_obj_.transform_,
			character_game_obj_.get_id()
		};

		// TODO: �������Ҫ���������߳���ѭ��
		Rigidbody character_rigidbody_;
		GLFWwindow* glfw_window_;

	};
}
