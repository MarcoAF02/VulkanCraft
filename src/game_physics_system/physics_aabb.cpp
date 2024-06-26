
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

		if (is_default_size) // 默认尺寸大小
		{
			half_size = glm::vec3(default_aabb_side_length_ / 2, default_aabb_side_length_ / 2, default_aabb_side_length_ / 2);
		}
		else // 自定义尺寸大小
		{
			half_size = glm::vec3(width / 2, height / 2, width / 2);
		}

		// 更新 AABB 的最小和最大坐标
		glm::vec3 aabb_min = collider_transform_component_.translation - half_size;
		glm::vec3 aabb_max = collider_transform_component_.translation + half_size;

		aabb_range_ = std::make_pair(aabb_min, aabb_max); // 确定 AABB 的范围
	}

	void AABBCollider::set_character_collider_size(const float height, const float width)
	{
		glm::vec3 half_size = glm::vec3(width / 2, height / 2, width / 2); // 计算半尺寸大小

		glm::vec3 collider_offset = // 计算玩家坐标偏移，让 AABB Collider 底部等于玩家脚踩的地面
		{
			collider_transform_component_.translation.x,
			collider_transform_component_.translation.y + height / 2,
			collider_transform_component_.translation.z
		};

		// 更新 AABB 的最小和最大坐标
		glm::vec3 aabb_min = collider_offset - half_size;
		glm::vec3 aabb_max = collider_offset + half_size;

		aabb_range_ = std::make_pair(aabb_min, aabb_max);
	}

	bool AABBCollider::is_point_inside_aabb(const glm::vec3 point_pos)
	{
		return
		{
			(point_pos.x >= aabb_range_.first.x && point_pos.x <= aabb_range_.second.x) &&
			(point_pos.y >= aabb_range_.first.y && point_pos.y <= aabb_range_.second.y) &&
			(point_pos.z >= aabb_range_.first.z && point_pos.z <= aabb_range_.second.z)
		};
	}

	bool AABBCollider::is_ray_intersects_aabb(const glm::vec3 ray_origin, const glm::vec3 ray_direction) const
	{
		// 获取 AABB 的最小和最大顶点
		const glm::vec3& min = aabb_range_.first;
		const glm::vec3& max = aabb_range_.second;

		glm::vec3 inv_dir; // 射线的方向向量分量的倒数，用于后续计算

		if (!glm::all(glm::notEqual(ray_direction, glm::vec3(0)))) return false; // 避免除以 0 的情况

		inv_dir.x = 1.0f / ray_direction.x;
		inv_dir.y = 1.0f / ray_direction.y;
		inv_dir.z = 1.0f / ray_direction.z;

		// 计算射线在三个坐标轴方向上的参数
		float tmin = -(ray_origin.x * inv_dir.x + ray_origin.y * inv_dir.y + ray_origin.z * inv_dir.z);
		float tmax = tmin;
		float tymin = tmin;
		float tymax = tmax;
		float tzmin = tmin;
		float tzmax = tmax;

		// 根据射线方向的不同正负，调整 tmin 和 tmax 的值
		if (inv_dir.x < 0.0f) std::swap(tmin, tmax);
		if (inv_dir.y < 0.0f) std::swap(tymin, tymax);
		if (inv_dir.z < 0.0f) std::swap(tzmin, tzmax);

		// 计算射线与 AABB 边界在x, y, z轴上的交点参数，取最大最小值以确定交点范围
		tmin = std::max({ tmin, tymin, tzmin });
		tmax = std::min({ tmax, tymax, tzmax });

		if (tmax < 0) return false; // 如果 tmax < 0，表示射线朝向背离 AABB，不相交

		// 如果 tmin > tmax，表示射线穿过 AABB 的边缘，不相交
		// 或者说，如果 tmin > tmax，射线在离开 AABB 后才与 AABB 的边界相交
		if (tmin > tmax) return false;

		return tmin >= 0; // 如果 tmin >= 0，则射线与 AABB 相交
	}

	bool AABBCollider::is_two_aabb_collision(AABBCollider& other_collider)
	{
		return
		{
			// 第一个的最左侧小于或等于第二个的最右侧（想象一下），就算相交
			(get_aabb_range().first.x <= other_collider.get_aabb_range().second.x &&
			get_aabb_range().second.x >= other_collider.get_aabb_range().first.x) &&
			(get_aabb_range().first.y <= other_collider.get_aabb_range().second.y &&
			get_aabb_range().second.y >= other_collider.get_aabb_range().first.y) &&
			(get_aabb_range().first.z <= other_collider.get_aabb_range().second.z &&
			get_aabb_range().second.z >= other_collider.get_aabb_range().first.z)
		};
	}

	std::vector<glm::vec3> AABBCollider::get_aabb_bottom_vertices() const
	{
		std::vector<glm::vec3> bottom_vertices_vector(4);

		glm::vec3 min = aabb_range_.first;
		glm::vec3 max = aabb_range_.second;

		float bottomY = min.y;

		// 分别计算四个顶点的坐标
		bottom_vertices_vector[0] = glm::vec3(min.x, bottomY, min.z); // 左后角
		bottom_vertices_vector[1] = glm::vec3(max.x, bottomY, min.z); // 右后角
		bottom_vertices_vector[2] = glm::vec3(min.x, bottomY, max.z); // 左前角
		bottom_vertices_vector[3] = glm::vec3(max.x, bottomY, max.z); // 右前角

		return bottom_vertices_vector;
	}

}
