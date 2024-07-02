
#pragma once

#include "../game_physics_system/physics_aabb.h"
#include "../managers/game_object_manager.h" // 游戏对象管理器

// std
#include <iostream>
#include <memory>

// 重力系统
namespace vulkancraft
{
	class Rigidbody
	{
	public:
		Rigidbody();
		~Rigidbody();

		void ground_check(AABBCollider& aabb_collider, float delta_time, float max_length); // 因重力下落的 AABB Collider
		void free_falling(float delta_time, glm::vec3& target_pos); // 返回一个自由落体用的向量

	private:

		const float kGravity = 9.8f; // 全局重力
		const float kEpsilon = 0.002f; // 防止检测到自己的极小值

		float accumulator_ = 0.0f; // 时间差累积

		std::shared_ptr<GameObjectManager> game_object_manager_; // 游戏公共对象管理单例
		bool is_ground_ = true; // 默认角色踩在地上
	};
}
