
#pragma once

#include "../gameplay_logic/base_game_object.h"

// stl
#include <iostream>
#include <glm/glm.hpp>

// HACK: AABB ��ײ�е�ȷ���߼���
// 1. �����ɷ����ľ���������� AABB ����
// 2. 1 �ľ�������Ϊ AABB �е����ģ��߳�Ϊ default_aabb_side_length
// 3. ����������AABB �Ƿ�� AABB �ཻ�����Ƿ�� AABB �ཻ
// AABB ��ײ�е� id Ӧ�ú� GameObject �� ID ��ͬ

namespace vulkancraft
{
	// ��ײ�е��ƶ����
	struct ColliderTransformComponent
	{
		glm::vec3 translation{};
		glm::vec3 scale{ 1.f, 1.f, 1.f };
		glm::vec3 rotation{};

		// Matrix corrsponds to Translate * Ry * Rx * Rz * Scale
		// Rotations correspond to Tait-bryan angles of Y(1), X(2), Z(3)
		// https://en.wikipedia.org/wiki/Euler_angles#Rotation_matrix

		// AABB ��ײ���������Լ�����ת���Ų�������
		glm::mat4 mat4();

		glm::mat3 normal_matrix();
	};

	class AABBCollider
	{
	public:

		using id_t = unsigned _int64;

		ColliderTransformComponent collider_transform_component_{};

		// ��ȫ�Բ���
		AABBCollider(const AABBCollider&) = delete;
		AABBCollider& operator = (const AABBCollider&) = delete;

		AABBCollider(TransformComponent& transform_component, unsigned _int64& id);

		// ���ݴ���� AABB Transform ��Ϣ���� Collider �� AABB ��Ϣ
		void set_aabb_collider_transform();

		// �������ܣ��жϵ��Ƿ��� AABB ��
		bool is_point_inside_aabb(glm::vec3 point_pos);

		// �������ܣ��ж�һ�������Ƿ��� AABB �ཻ
		bool is_ray_intersects_aabb(glm::vec3 ray_origin, glm::vec3 ray_direction);

		// �������ܣ��ж�һ�� AABB ��ײ���Ƿ�����һ�� AABB ��ײ���ཻ
		bool is_two_aabb_collision(AABBCollider& other_collider);

		id_t get_id() { return id_; } // �õ� id
		void set_id(id_t new_id) {id_ = new_id;} // ������ id
		std::pair<glm::vec3, glm::vec3> get_aabb_range() {return aabb_range_;} // �õ� AABB ��ײ�еķ�Χ

	private:

		id_t id_ = 0; // ��¼�� AABB ��ײ�е� ID
		float default_aabb_side_length_ = 1.0f; // Ĭ�ϵ� AABB ��ײ�б߳�

		// aabb_min, aabb_max
		std::pair<glm::vec3, glm::vec3> aabb_range_; // ��¼ aabb ��ײ�еķ�Χ
	};
}
