
#pragma once

#include "../gameplay_logic/base_game_object.h"

// stl
#include <iostream>
#include <glm/glm.hpp>

// HACK: AABB 碰撞盒的确定逻辑：
// 1. 将生成方块后的具体坐标给到 AABB 盒类
// 2. 1 的具体坐标为 AABB 盒的中心，参数来自对方的 TransformComponent
// 3. 创建函数：AABB 是否和 AABB 相交，点是否和 AABB 相交
// AABB 碰撞盒的 id 应该和 GameObject 的 ID 相同

struct vulkancraft::TransformComponent;

namespace vulkancraft
{
	class AABBCollider
	{
	public:

		using id_t = unsigned _int64;
		TransformComponent collider_transform_component_{};

		AABBCollider(const TransformComponent& transform_component, const id_t& id);

		// 根据传入的 AABB Transform 信息设置 Collider 的 AABB 信息
		void set_aabb_collider_transform();

		// 基础功能：判断点是否在 AABB 内
		bool is_point_inside_aabb(const glm::vec3 point_pos);

		// 基础功能，判断一条射线是否与 AABB 相交
		bool is_ray_intersects_aabb(const glm::vec3 ray_origin, const glm::vec3 ray_direction);

		// 基础功能，判断一个 AABB 碰撞盒是否与另一个 AABB 碰撞盒相交
		bool is_two_aabb_collision(AABBCollider& other_collider);

		id_t get_id() { return id_; } // 得到 id
		void set_id(id_t new_id) {id_ = new_id;} // 设置新 id
		std::pair<glm::vec3, glm::vec3> get_aabb_range() {return aabb_range_;} // 得到 AABB 碰撞盒的范围

	private:

		id_t id_ = 0; // 记录该 AABB 碰撞盒的 ID
		float default_aabb_side_length_ = 1.0f; // 默认的 AABB 碰撞盒边长

		// aabb_min, aabb_max
		std::pair<glm::vec3, glm::vec3> aabb_range_; // 记录 aabb 碰撞盒的范围
	};
}
