
#pragma once

#include "../gameplay_logic/base_game_object.h"

// stl
#include <iostream>
#include <glm/glm.hpp>

// HACK: AABB 碰撞盒的确定逻辑：
// 1. 将生成方块后的具体坐标给到 AABB 盒类
// 2. 1 的具体坐标为 AABB 盒的中心，参数来自对方的 TransformComponent
// AABB 碰撞盒的 id 应该和 GameObject 的 ID 相同

struct vulkancraft::TransformComponent;

namespace vulkancraft
{
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

		// 设置 AABB Collider 的函数，是普通游戏物体就用第一个函数，是角色就用第二个函数

		// 根据传入的 AABB Transform 信息设置 Collider 的 AABB 信息，默认设置为长宽为 1 的方块 Collider
		void set_aabb_collider_transform(const bool is_default_size, const float height, const float width);

		// 设置角色的 AABB Collider
		void set_character_collider_size(const float height, const float width);

		// 基础功能：判断点是否在 AABB 内
		bool is_point_inside_aabb(const glm::vec3 point_pos);

		// 基础功能，平板法判断一条射线是否与 AABB 相交，将 AABB 包围盒想象成 6 个平行于坐标轴的无限延展平面
		bool is_ray_intersects_aabb(const glm::vec3 ray_origin, const glm::vec3 ray_direction) const;

		// 基础功能，判断一个 AABB 碰撞盒是否与另一个 AABB 碰撞盒相交
		bool is_two_aabb_collision(AABBCollider& other_collider);

		std::vector<glm::vec3> get_aabb_bottom_vertices() const; // 得到 AABB Collider 的四个落脚点

		id_t get_id() { return id_; } // 得到 id
		void set_id(id_t new_id) {id_ = new_id;} // 设置新 id
		std::pair<glm::vec3, glm::vec3> get_aabb_range() const {return aabb_range_;} // 得到 AABB 碰撞盒的范围

	private:

		id_t id_ = 0; // 记录该 AABB 碰撞盒的 ID
		float default_aabb_side_length_ = 1.0f; // 默认的 AABB 碰撞盒边长

		std::pair<glm::vec3, glm::vec3> aabb_range_; // 记录 aabb 碰撞盒的范围
	};
}
