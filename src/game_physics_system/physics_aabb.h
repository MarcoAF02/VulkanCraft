
#pragma once

#include "../gameplay_logic/base_game_object.h"

// std
#include <iostream>
#include <array>
#include <glm/glm.hpp>

// HACK: AABB ��ײ�е�ȷ���߼���
// 1. �����ɷ����ľ���������� AABB ����
// 2. 1 �ľ�������Ϊ AABB �е����ģ��������ԶԷ��� TransformComponent
// AABB ��ײ�е� id Ӧ�ú� GameObject �� ID ��ͬ

struct vulkancraft::TransformComponent;

namespace vulkancraft
{
	// ��¼���� AABB Collider ����һ�෢����ײ
	enum class CollisionSide
	{
		None,
		Left,
		Right,
		Front,
		Back,
		Top,
		Bottom
	};

	// ��¼���� AABB ��������Ϣ
	struct AABBContactInfo
	{
		bool is_touching;
		glm::vec3 normal;
		glm::vec3 tangent1;
		glm::vec3 tangent2;
		bool tangents_are_parallel;
	};

	class AABBCollider
	{
	public:

		using id_t = unsigned _int64;
		TransformComponent collider_transform_component_{};

		AABBCollider
		(
			const bool is_character_collider,
			const TransformComponent& transform_component,
			const id_t& id,
			const bool is_default_size,
			const float height,
			const float width
		);

		// ���� AABB Collider �ĺ���������ͨ��Ϸ������õ�һ���������ǽ�ɫ���õڶ�������

		// ���ݴ���� AABB Transform ��Ϣ���� Collider �� AABB ��Ϣ��Ĭ������Ϊ����Ϊ 1 �ķ��� Collider
		void set_aabb_collider_transform(const bool is_default_size, const float height, const float width);

		// ���ý�ɫ�� AABB Collider
		void set_character_collider(const glm::vec3 cur_pos, const float height, const float width);

		// �������ܣ��жϵ��Ƿ��� AABB ��
		bool is_point_inside_aabb(const glm::vec3 point_pos) const;

		// �������ܣ�ƽ�巨�ж�һ�������Ƿ��� AABB �ཻ���� AABB ��Χ������� 6 ��ƽ�����������������չƽ��
		bool is_ray_intersects_aabb(const glm::vec3 ray_origin, const glm::vec3 ray_direction, const float max_length) const;

		// �������ܣ����� AABB �Ƿ��ཻ
		bool is_two_aabb_collision(const AABBCollider& other_collider) const;

		// �������ܣ��ж����� AABB �Ƿ�����
		bool is_two_aabb_touching(const AABBCollider& other_collider) const;

		// �������ܣ��÷����ж����� AABB ���������
		AABBContactInfo is_two_aabb_touching_check_normal(const AABBCollider& other_collider) const;

		// �������Ϸ��ɫ�Ļ������õ���������������
		std::array<glm::vec3, 4> get_character_aabb_bottom_vertices() const; // �õ���ɫ AABB Collider ���ĸ��ײ�����
		std::array<glm::vec3, 4> get_character_aabb_top_vertices() const; // �õ���ɫ AABB Collider �������ĸ�����

		glm::vec3 get_face_normal(CollisionSide side) const; // �õ� AABB Collider ÿ����ķ���

		id_t get_id() { return id_; } // �õ� id
		void set_id(id_t new_id) {id_ = new_id;} // ������ id
		std::pair<glm::vec3, glm::vec3> get_aabb_range() const {return aabb_range_;} // �õ� AABB ��ײ�еķ�Χ

	private:

		id_t id_ = 0; // ��¼�� AABB ��ײ�е� ID
		float default_aabb_side_length_ = 1.0f; // Ĭ�ϵ� AABB ��ײ�б߳�

		std::pair<glm::vec3, glm::vec3> aabb_range_; // ��¼ aabb ��ײ�еķ�Χ
		std::array<glm::vec3, 4> character_bottom_aabb_pos_array_; // ��¼��ɫ AABB �ĵײ��ĸ���
		std::array<glm::vec3, 4> character_top_aabb_pos_array_; // ��¼��ɫ AABB �Ķ����ĸ���
		std::array<glm::vec3, 6> face_normals_; // ��¼ aabb ��ײ��ÿ����ķ���
	};
}
