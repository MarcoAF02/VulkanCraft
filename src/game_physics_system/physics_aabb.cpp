
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
			set_character_collider_size(height, width);
		}
		else
		{
			set_aabb_collider_transform(is_default_size, height, width);
		}
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
		glm::vec3 aabb_min = collider_transform_component_.translation - half_size;
		glm::vec3 aabb_max = collider_transform_component_.translation + half_size;

		aabb_range_ = std::make_pair(aabb_min, aabb_max); // ȷ�� AABB �ķ�Χ
	}

	void AABBCollider::set_character_collider_size(const float height, const float width)
	{
		glm::vec3 half_size = glm::vec3(width / 2, height / 2, width / 2); // �����ߴ��С

		glm::vec3 collider_offset = // �����������ƫ�ƣ��� AABB Collider �ײ�������ҽŲȵĵ���
		{
			collider_transform_component_.translation.x,
			collider_transform_component_.translation.y + height / 2,
			collider_transform_component_.translation.z
		};

		// TODO: ͷ����λ�������⣬AABB ���ڴ� AABB �ķ�Χ��ʱ�޷��ж�����
		// TODO: ����������Ժ󣬰�֮ǰ���������߼������ж�ȥ��...
		// TODO: ע�� Y ������·����Ƿ��ģ���ͷ�޸�

		// ���� AABB ����С���������
		glm::vec3 aabb_min = collider_offset - half_size;
		glm::vec3 aabb_max = collider_offset + half_size;

		aabb_range_ = std::make_pair(aabb_min, aabb_max);
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
		if (min1.y > max2.y || max1.y < min2.y) return false;
		if (min1.z > max2.z || max1.z < min2.z) return false;

		// ��������ᶼ����������������AABB����
		return true;
	}

	std::vector<glm::vec3> AABBCollider::get_aabb_bottom_vertices() const
	{
		std::vector<glm::vec3> bottom_vertices_vector(4);

		glm::vec3 min = aabb_range_.first;
		glm::vec3 max = aabb_range_.second;

		float bottom_y = min.y;

		// �ֱ�����ĸ����������
		bottom_vertices_vector[0] = glm::vec3(min.x, bottom_y, min.z); // ����
		bottom_vertices_vector[1] = glm::vec3(max.x, bottom_y, min.z); // �Һ��
		bottom_vertices_vector[2] = glm::vec3(min.x, bottom_y, max.z); // ��ǰ��
		bottom_vertices_vector[3] = glm::vec3(max.x, bottom_y, max.z); // ��ǰ��

		return bottom_vertices_vector;
	}

}
