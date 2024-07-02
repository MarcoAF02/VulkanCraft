
#pragma once

#include "../game_physics_system/physics_aabb.h"
#include "../managers/game_object_manager.h" // ��Ϸ���������

// std
#include <iostream>
#include <memory>

// ����ϵͳ
namespace vulkancraft
{
	class Rigidbody
	{
	public:
		Rigidbody();
		~Rigidbody();

		void ground_check(AABBCollider& aabb_collider, float delta_time, float max_length); // ����������� AABB Collider
		void free_falling(float delta_time, glm::vec3& target_pos); // ����һ�����������õ�����

	private:

		const float kGravity = 9.8f; // ȫ������
		const float kEpsilon = 0.002f; // ��ֹ��⵽�Լ��ļ�Сֵ

		float accumulator_ = 0.0f; // ʱ����ۻ�

		std::shared_ptr<GameObjectManager> game_object_manager_; // ��Ϸ�������������
		bool is_ground_ = true; // Ĭ�Ͻ�ɫ���ڵ���
	};
}
