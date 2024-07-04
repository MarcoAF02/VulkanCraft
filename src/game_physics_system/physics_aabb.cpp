
#include "physics_aabb.h"

namespace vulkancraft
{
	AABBCollider::AABBCollider
	(
		const bool is_character_collider,
		const TransformComponent& transform_component,
		const id_t& id,
		const bool is_default_size,
		const float height,
		const float width
	)
	{
		collider_transform_component_.translation = transform_component.translation;
		collider_transform_component_.rotation = transform_component.rotation;
		collider_transform_component_.scale = transform_component.scale;

		set_id(id);

		if (is_character_collider)
		{
			set_character_collider(transform_component.translation, height, width);
		}
		else
		{
			set_aabb_collider_transform(is_default_size, height, width);
		}

		// ��ʼ�� AABB ���淨��
		face_normals_[0] = glm::vec3(1, 0, 0); // �� X ��
		face_normals_[1] = glm::vec3(-1, 0, 0); // �� X ��
		face_normals_[2] = glm::vec3(0, -1, 0); // �� Y �棨ע�� Y �᷽��
		face_normals_[3] = glm::vec3(0, 1, 0); // �� Y �棨ע�� Y �᷽��
		face_normals_[4] = glm::vec3(0, 0, 1); // �� Z ��
		face_normals_[5] = glm::vec3(0, 0, -1); // �� Z ��
	}

	void AABBCollider::set_aabb_collider_transform(const bool is_default_size, const float height, const float width)
	{
		glm::vec3 half_size;

		if (is_default_size) // Ĭ�ϳߴ��С
		{
			half_size = glm::vec3(default_aabb_side_length_ / 2, default_aabb_side_length_ / 2, default_aabb_side_length_ / 2);
		}
		else // �Զ���ߴ��С
		{
			half_size = glm::vec3(width / 2, height / 2, width / 2);
		}

		// ���� AABB ����С���������
		glm::vec3 aabb_min = collider_transform_component_.translation - glm::vec3(half_size.x, -half_size.y, half_size.z);
		glm::vec3 aabb_max = collider_transform_component_.translation + glm::vec3(half_size.x, -half_size.y, half_size.z);

		//std::cout << aabb_min.x << ", " << aabb_min.y << ", " << aabb_min.z << std::endl;
		//std::cout << aabb_max.x << ", " << aabb_max.y << ", " << aabb_max.z << std::endl;

		aabb_range_ = std::make_pair(aabb_min, aabb_max); // ȷ�� AABB �ķ�Χ
	}

	void AABBCollider::set_character_collider(const glm::vec3 cur_pos, const float height, const float width)
	{
		float half_width = width / 2.0f;

		glm::vec3 aabb_min = {cur_pos.x - half_width, cur_pos.y, cur_pos.z - half_width};
		glm::vec3 aabb_max = {cur_pos.x + half_width, cur_pos.y - height, cur_pos.z + half_width};

		aabb_range_.first = aabb_min;
		aabb_range_.second = aabb_max;

		// X ����������Z ��������ǰ

		// ��¼�ײ��ĸ�����
		glm::vec3 bottom_left_front = {cur_pos.x + half_width, cur_pos.y, cur_pos.z + half_width};
		glm::vec3 bottom_right_front = {cur_pos.x - half_width, cur_pos.y, cur_pos.z + half_width};
		glm::vec3 bottom_left_back = {cur_pos.x + half_width, cur_pos.y, cur_pos.z - half_width};
		glm::vec3 bottom_right_back = {cur_pos.x - half_width, cur_pos.y, cur_pos.z - half_width};

		// ��¼�����ĸ�����
		glm::vec3 top_left_front = {cur_pos.x + half_width, cur_pos.y - height, cur_pos.z + half_width};
		glm::vec3 top_right_front = {cur_pos.x - half_width, cur_pos.y - height, cur_pos.z + half_width};
		glm::vec3 top_left_back = {cur_pos.x + half_width, cur_pos.y - height, cur_pos.z - half_width};
		glm::vec3 top_right_back = {cur_pos.x - half_width, cur_pos.y - height, cur_pos.z - half_width};

		character_top_aabb_pos_array_[0] = top_left_front;
		character_top_aabb_pos_array_[1] = top_right_front;
		character_top_aabb_pos_array_[2] = top_left_back;
		character_top_aabb_pos_array_[3] = top_right_back;

		character_bottom_aabb_pos_array_[0] = bottom_left_front;
		character_bottom_aabb_pos_array_[1] = bottom_right_front;
		character_bottom_aabb_pos_array_[2] = bottom_left_back;
		character_bottom_aabb_pos_array_[3] = bottom_right_back;
	}

	bool AABBCollider::is_point_inside_aabb(const glm::vec3 point_pos) const
	{
		return
		{
			(point_pos.x >= aabb_range_.first.x && point_pos.x <= aabb_range_.second.x) &&
			(point_pos.y >= aabb_range_.first.y && point_pos.y <= aabb_range_.second.y) &&
			(point_pos.z >= aabb_range_.first.z && point_pos.z <= aabb_range_.second.z)
		};
	}

	bool AABBCollider::is_ray_intersects_aabb(const glm::vec3 ray_origin, const glm::vec3 ray_direction, const float max_length) const
	{
		// ������ڴ���ܷ��ʲ���������Ч����
		const glm::vec3& min = aabb_range_.first;
		const glm::vec3& max = aabb_range_.second;

		glm::vec3 inv_dir;

		// ������� 0 �����
		if (ray_direction.x == 0 && ray_direction.y == 0 && ray_direction.z == 0) return false;

		// �������߷�������ĵ�����������Ϊ 0������Ϊ INFINITY
		inv_dir.x = ray_direction.x != 0 ? 1.0f / ray_direction.x : INFINITY;
		inv_dir.y = ray_direction.y != 0 ? 1.0f / ray_direction.y : INFINITY;
		inv_dir.z = ray_direction.z != 0 ? 1.0f / ray_direction.z : INFINITY;

		// ���������������������ϵĲ���
		float t1 = (min.x - ray_origin.x) * inv_dir.x;
		float t2 = (max.x - ray_origin.x) * inv_dir.x;
		float t3 = (max.y - ray_origin.y) * inv_dir.y;
		float t4 = (min.y - ray_origin.y) * inv_dir.y;
		float t5 = (min.z - ray_origin.z) * inv_dir.z;
		float t6 = (max.z - ray_origin.z) * inv_dir.z;

		float tmin = std::max(std::min(t1, t2), std::max(std::min(t3, t4), std::min(t5, t6)));
		float tmax = std::min(std::max(t1, t2), std::min(std::max(t3, t4), std::max(t5, t6)));

		if (max_length < 0) // ����Ĳ������С�� 0 ���ʾû����󳤶�����
		{
			return tmin <= tmax && tmin >= 0; // �ж������Ƿ��� AABB �ཻ
		}
		else // Ӧ����󳤶�����
		{
			tmax = std::min(tmax, max_length);
			tmin = std::max(tmin, 0.0f); // ȷ�� tmin ����Ч��Χ��

			return tmin <= tmax && tmin >= 0; // �ж������Ƿ��� AABB �ཻ
		}
	}

	bool AABBCollider::is_two_aabb_collision(const AABBCollider& other_collider) const
	{
		return
		{
			// ��һ���������С�ڻ���ڵڶ��������Ҳࣨ����һ�£��������ཻ
			(get_aabb_range().first.x <= other_collider.get_aabb_range().second.x &&
			get_aabb_range().second.x >= other_collider.get_aabb_range().first.x) &&
			(get_aabb_range().first.y <= other_collider.get_aabb_range().second.y &&
			get_aabb_range().second.y >= other_collider.get_aabb_range().first.y) &&
			(get_aabb_range().first.z <= other_collider.get_aabb_range().second.z &&
			get_aabb_range().second.z >= other_collider.get_aabb_range().first.z)
		};
	}

	bool AABBCollider::is_two_aabb_touching(const AABBCollider& other_collider) const
	{
		const glm::vec3& min1 = aabb_range_.first;
		const glm::vec3& max1 = aabb_range_.second;
		const glm::vec3& min2 = other_collider.aabb_range_.first;
		const glm::vec3& max2 = other_collider.aabb_range_.second;

		// �ֱ��� x��y��z �ᣨ�Ȳ��������Ҳ������С�ڣ����ǵ��ڣ�
		if (min1.x > max2.x || max1.x < min2.x) return false;
		if (min1.y < max2.y || max1.y > min2.y) return false;
		if (min1.z > max2.z || max1.z < min2.z) return false;

		// ��������ᶼ����������������AABB����
		return true;
	}

	std::array<glm::vec3, 4> AABBCollider::get_character_aabb_bottom_vertices() const
	{
		std::array<glm::vec3, 4> bottom_vertices;

		glm::vec3 min = aabb_range_.first;
		glm::vec3 max = aabb_range_.second;

		float bottom_y = min.y;

		// �ֱ�����ĸ����������
		bottom_vertices[0] = glm::vec3(min.x, bottom_y, min.z); // ����
		bottom_vertices[1] = glm::vec3(max.x, bottom_y, min.z); // �Һ��
		bottom_vertices[2] = glm::vec3(min.x, bottom_y, max.z); // ��ǰ��
		bottom_vertices[3] = glm::vec3(max.x, bottom_y, max.z); // ��ǰ��

		return bottom_vertices;
	}

	std::array<glm::vec3, 4> AABBCollider::get_character_aabb_top_vertices() const
	{
		std::array<glm::vec3, 4> top_vertices;

		glm::vec3 min = aabb_range_.first;
		glm::vec3 max = aabb_range_.second;

		float top_y = max.y;

		// �ֱ�����ĸ����������
		top_vertices[0] = glm::vec3(min.x, top_y, min.z); // ����
		top_vertices[1] = glm::vec3(max.x, top_y, min.z); // �Һ��
		top_vertices[2] = glm::vec3(min.x, top_y, max.z); // ��ǰ��
		top_vertices[3] = glm::vec3(max.x, top_y, max.z); // ��ǰ��

		return top_vertices;
	}

	glm::vec3 AABBCollider::get_face_normal(CollisionSide side) const
	{
		switch (side)
		{
		case CollisionSide::Left:
			return face_normals_[1];
		case CollisionSide::Right:
			return face_normals_[0];
		case CollisionSide::Top:
			return face_normals_[3]; // Y �ᵹ�ã�Top ��Ӧԭ���ĸ� Y ��
		case CollisionSide::Bottom:
			return face_normals_[2]; // Y �ᵹ�ã�Bottom ��Ӧԭ������ Y ��
		case CollisionSide::Front:
			return face_normals_[5];
		case CollisionSide::Back:
			return face_normals_[4];
		default:
			return glm::vec3(0, 0, 0); // û��ƥ��Ĳ���
		}
	}

}
