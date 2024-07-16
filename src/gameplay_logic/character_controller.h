
#pragma once

#include "../gameplay_logic/game_camera.h"
#include "../input_system/keyboard_input_controller.h"
#include "../input_system/mouse_rotate_controller.h"
#include "../base_tools/details.h"
#include "../base_tools/physics_object_save_data.h"

// std
#include <iostream>

// glm ��ѧ��
#include <glm/glm.hpp>

// Bullet 3 ����ϵͳ
#include "btBulletDynamicsCommon.h"

struct vulkancraft::TransformComponent;
class vulkancraft::KeyboardMovementController; // ���̿�����
class vulkancraft::MouseRotateController; // �����ת������
struct BlockGenerateData;
struct PhysicsObjectTransData;
struct PhysicsObjectSaveData;

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

		// ��ʼ�� character controller
		void init_character_controller();

		// ��ʼ���������ϵͳ
		void init_character_physics();

		// ע���������ϵͳ
		PhysicsObjectSaveData& get_player_physics_data();
		BaseGameObject::id_t get_player_obj_id() const;

		// ���ò�������������
		void set_player_camera(PlayerCameraView camera_view);

		// �õ���ҹ۲����
		GameBaseCamera& get_player_camera();

		void sync_player_rigidbody_pos(); // ͬ����ҽ�ɫλ�����������

		// ��ʼ������ӽǿ�����
		void init_mouse_rotate(GLFWwindow* glfw_window);

		// ��������������ת��ֻ���������������ת
		void move(float fixed_delta_time, GLFWwindow* glfw_window);
		void rotate(float fixed_delta_time, GLFWwindow* glfw_window);

		// ������ײ����ֹͣ����ƶ�
		// void stop_moving(CollisionSide side);

#pragma region DEBUG �ú���

		// ����ҵ���ߣ���ײ���ȫ��ӡ����
		void print_player_details() const;

#pragma endregion

	private:

		// ����ĸ߶�Ϊ 1.8 ���Ϊ 0.6
		// ���������Ҫ AABB Collider����ҽ�ɫ��Ҫ

		float character_height_ = 1.8f; // ��������
		float character_width_ = 0.6f; // ����Ŀ��
		float camera_height_ = 0.83f; // ������ĸ߶�

		float half_height_ = character_height_ / 2;
		float half_width_ = character_width_ / 2;

		BaseGameObject::id_t player_id_ = 2560; // ��ҵ�Ӳ���� ID

		glm::vec3 spawn_point_ = { 0.0f, -10.0f, 0.0f };

		BaseGameObject character_game_obj_; // ����������ת��ֻ��������ת
		BaseGameObject camera_game_obj_; // ������õĹ۲���Ϸ���壬���ڸ������������ת

		GameBaseCamera player_camera_; // ��������

		KeyboardMovementController keyboard_move_controller_;
		MouseRotateController mouse_rotate_controller_;

#pragma region ������������ز���

		btTransform player_collider_transform_{};
		btScalar player_collider_mass_{1.0f};
		btVector3 player_collider_local_inertia{};

		btCollisionShape* player_collision_;
		btDefaultMotionState* player_motion_state_;
		btRigidBody* player_rigidbody_;

		BlockGenerateData player_collider_data_ =
		{
			character_game_obj_.transform_.translation, // λ�����ڳ�����
			character_game_obj_.transform_.rotation,
			character_game_obj_.transform_.scale,

			1.0f // ����
		};

		btVector3 player_collider_size_ = // �ߴ�
		{
			half_width_,
			half_height_,
			half_width_
		};

		btVector3 player_collider_origin_ = // collider ������
		{
			spawn_point_.x,
			spawn_point_.y,
			spawn_point_.z
		};

		PhysicsObjectSaveData player_physics_data_ {};
	
#pragma endregion

	};
}
