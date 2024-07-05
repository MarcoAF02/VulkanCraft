
#pragma once

#include "../gameplay_logic/base_game_object.h"

// std
#include <iostream>
#include <array>
#include <glm/glm.hpp>

// HACK: AABB 碰撞盒的确定逻辑：
// 1. 将生成方块后的具体坐标给到 AABB 盒类
// 2. 1 的具体坐标为 AABB 盒的中心，参数来自对方的 TransformComponent
// AABB 碰撞盒的 id 应该和 GameObject 的 ID 相同

struct vulkancraft::TransformComponent;

namespace vulkancraft
{
	// 记录其他 AABB Collider 在哪一侧发生碰撞
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

	// 记录两个 AABB 的相切信息
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

		// 设置 AABB Collider 的函数，是普通游戏物体就用第一个函数，是角色就用第二个函数

		// 根据传入的 AABB Transform 信息设置 Collider 的 AABB 信息，默认设置为长宽为 1 的方块 Collider
		void set_aabb_collider_transform(const bool is_default_size, const float height, const float width);

		// 设置角色的 AABB Collider
		void set_character_collider(const glm::vec3 cur_pos, const float height, const float width);

		// 基础功能：判断点是否在 AABB 内
		bool is_point_inside_aabb(const glm::vec3 point_pos) const;

		// 基础功能，平板法判断一条射线是否与 AABB 相交，将 AABB 包围盒想象成 6 个平行于坐标轴的无限延展平面
		bool is_ray_intersects_aabb(const glm::vec3 ray_origin, const glm::vec3 ray_direction, const float max_length) const;

		// 基础功能，两个 AABB 是否相交
		bool is_two_aabb_collision(const AABBCollider& other_collider) const;

		// 基础功能，判断两个 AABB 是否相切
		bool is_two_aabb_touching(const AABBCollider& other_collider) const;

		// 基础功能，用法线判断两个 AABB 的相切情况
		AABBContactInfo is_two_aabb_touching_check_normal(const AABBCollider& other_collider) const;

		// 如果是游戏角色的话，会用到下面这两个函数
		std::array<glm::vec3, 4> get_character_aabb_bottom_vertices() const; // 得到角色 AABB Collider 的四个底部顶点
		std::array<glm::vec3, 4> get_character_aabb_top_vertices() const; // 得到角色 AABB Collider 顶部的四个顶点

		glm::vec3 get_face_normal(CollisionSide side) const; // 得到 AABB Collider 每个面的法线

		id_t get_id() { return id_; } // 得到 id
		void set_id(id_t new_id) {id_ = new_id;} // 设置新 id
		std::pair<glm::vec3, glm::vec3> get_aabb_range() const {return aabb_range_;} // 得到 AABB 碰撞盒的范围

	private:

		id_t id_ = 0; // 记录该 AABB 碰撞盒的 ID
		float default_aabb_side_length_ = 1.0f; // 默认的 AABB 碰撞盒边长

		std::pair<glm::vec3, glm::vec3> aabb_range_; // 记录 aabb 碰撞盒的范围
		std::array<glm::vec3, 4> character_bottom_aabb_pos_array_; // 记录角色 AABB 的底部四个点
		std::array<glm::vec3, 4> character_top_aabb_pos_array_; // 记录角色 AABB 的顶部四个点
		std::array<glm::vec3, 6> face_normals_; // 记录 aabb 碰撞盒每个面的法线
	};
}
