
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
		// 这里的内存可能因为物理线程先初始化完成导致访问不到
		const glm::vec3& min = aabb_range_.first;
		const glm::vec3& max = aabb_range_.second;

		glm::vec3 inv_dir;

		// 避免除以 0 的情况
		if (ray_direction.x == 0 && ray_direction.y == 0 && ray_direction.z == 0) return false;

		// 设置射线方向分量的倒数，若分量为 0，则设为 INFINITY
		inv_dir.x = ray_direction.x != 0 ? 1.0f / ray_direction.x : INFINITY;
		inv_dir.y = ray_direction.y != 0 ? 1.0f / ray_direction.y : INFINITY;
		inv_dir.z = ray_direction.z != 0 ? 1.0f / ray_direction.z : INFINITY;

		// 计算射线在三个坐标轴上的参数
		float t1 = (min.x - ray_origin.x) * inv_dir.x;
		float t2 = (max.x - ray_origin.x) * inv_dir.x;
		float t3 = (max.y - ray_origin.y) * inv_dir.y;
		float t4 = (min.y - ray_origin.y) * inv_dir.y;
		float t5 = (min.z - ray_origin.z) * inv_dir.z;
		float t6 = (max.z - ray_origin.z) * inv_dir.z;

		float tmin = std::max(std::min(t1, t2), std::max(std::min(t3, t4), std::min(t5, t6)));
		float tmax = std::min(std::max(t1, t2), std::min(std::max(t3, t4), std::max(t5, t6)));

		if (max_length < 0) // 传入的参数如果小于 0 则表示没有最大长度限制
		{
			return tmin <= tmax && tmin >= 0; // 判断射线是否与 AABB 相交
		}
		else // 应用最大长度限制
		{
			tmax = std::min(tmax, max_length);
			tmin = std::max(tmin, 0.0f); // 确保 tmin 在有效范围内

			return tmin <= tmax && tmin >= 0; // 判断射线是否与 AABB 相交
		}
	}

	bool AABBCollider::is_two_aabb_collision(AABBCollider& other_collider) const
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

	// TODO: 问一下通义千问这些代码的原理是什么，到底是怎么算的
	// TODO: 这里的碰撞判断范围可能小了，要扩大判断范围
	CollisionSide AABBCollider::get_collision_side_with(AABBCollider& other_collider) const
	{
		// 获取两个 AABB 的范围
		auto this_range = get_aabb_range();
		auto other_range = other_collider.get_aabb_range();

		// 检查是否真的发生了碰撞。注意这里含有对 Y 轴的判断，属于是有但不需要作为
		if (!is_two_aabb_collision(other_collider)) return CollisionSide::None;

		glm::vec3 center_diff = (other_range.first + other_range.second) / 2.0f - (this_range.first + this_range.second) / 2.0f;

		// 确定碰撞的方向
		if (std::abs(center_diff.x) > std::abs(center_diff.y) && std::abs(center_diff.x) > std::abs(center_diff.z))
		{
			return center_diff.x > 0 ? CollisionSide::Right : CollisionSide::Left;
		}
		else if (std::abs(center_diff.y) > std::abs(center_diff.z))
		{
			return center_diff.y > 0 ? CollisionSide::Top : CollisionSide::Bottom;
		}
		else
		{
			return center_diff.z > 0 ? CollisionSide::Front : CollisionSide::Back;
		}
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
