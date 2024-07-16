
#pragma once

#include "../gameplay_logic/game_camera.h"
#include "../input_system/keyboard_input_controller.h"
#include "../input_system/mouse_rotate_controller.h"
#include "../base_tools/details.h"

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
		void init_character_controller(glm::vec3 spawn_point);
		void init_character_physics(BaseGameObject::RenderAppObjMap& game_object_map, std::unordered_map<BaseGameObject::id_t, PhysicsObjectSaveData>& physics_obj_map); // ��ʼ���������ϵͳ

		// ���ò�������������
		void set_player_camera(PlayerCameraView camera_view);

		// �õ���ҹ۲����
		GameBaseCamera& get_player_camera();

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
		float camera_height_ = 1.65f; // ������ĸ߶�

		// HACK: �����ģ����� 0.1f...
		float ground_check_max_length_ = 0.1f; // �ŵ����߼��ĳ���
		float wall_check_max_length_ = 5.0f; // ǽ�����߼��ĳ���

		glm::vec3 spawn_point_ = glm::vec3{ 0, 0, 0 };

		BaseGameObject character_game_obj_; // ����������ת��ֻ��������ת
		BaseGameObject camera_game_obj_; // ������õĹ۲���Ϸ���壬���ڸ������������ת

		GameBaseCamera player_camera_; // ��������

		KeyboardMovementController keyboard_move_controller_;
		MouseRotateController mouse_rotate_controller_;
	};
}
