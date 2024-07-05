
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

// TODO List:
// 1. ��ҿ�����ʵ�ֵ����⣨��ɣ�
// 2. ��ҿ������������䣬��� 1 ʵ�����ڵ��ϣ���ɣ�
// 3. ���ײǽʱֹͣ�ƶ�������ͨ����ȡ��ײĿ�� Collider ��ķ���ʵ��
// 4. �����Ծ����Ծ��ͷȡ�����ϵ������������䣨һ��Ҫ�������ϵ��������߼�⣩
// 5. �ƶ���ֵƽ�������ײǽ�����ƶ��ٶ�
// 6. ˼����ôʵ�ֲ���ϵͳ����ͬ�ķ�����ʾ��ͬ������
// 7. �� esc ������꣬���� alt ������ꡣ�˵������߼��¿�һ������ϵͳ
// TODO: ���� 6 ֮ǰ����ͷ�Ż� 3

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

		// ���ò�������������
		void set_player_camera(PlayerCameraView camera_view);

		// �õ���ҹ۲����
		GameBaseCamera& get_player_camera();

		// �õ���ҵ� AABB Collider
		AABBCollider& get_player_collider();

		// ��ʼ������ӽǿ�����
		void init_mouse_rotate(GLFWwindow* glfw_window);

		// ��������������ת��ֻ���������������ת
		void move(float fixed_delta_time, GLFWwindow* glfw_window);
		void rotate(float fixed_delta_time, GLFWwindow* glfw_window);

		// TODO: ��������ͷ���䣬��Ҫʹ�ö�ͷ�������߼��
		// TODO: ���� AABB �ȼ���ص����ᣬ�ټ������
		// TODO: ��� AABB �ص����ɹ�������Ҫ��ͷ���������߼����
		// TODO: �Ķ�һ���Լ��Ĵ���...
		void update_player_physics(float delta_time); // �����������
		void update_player_collision();
		void handle_collision(AABBCollider& wall_collider); // ������ Collider ��ǽ�����ײ

		// ������ײ����ֹͣ����ƶ�
		// void stop_moving(CollisionSide side);

#pragma region DEBUG �ú���

		// ����ҵ���ߣ���ײ���ȫ��ӡ����
		void print_player_details() const;

#pragma endregion

	private:

		std::shared_ptr<GameObjectManager> game_object_manager_; // ��Ϸ�������������

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

		AABBCollider character_collider_ =
		{
			true,
			character_game_obj_.transform_,
			character_game_obj_.get_id(),
			false,
			character_height_,
			character_width_
		};

		// TODO: �������Ҫ���������߳���ѭ��
		Rigidbody character_rigidbody_;
	};
}
