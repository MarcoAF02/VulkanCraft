
#pragma once

#include "../game_physics_system/physics_aabb.h"

#include <iostream>
#include <memory>

// 重力系统

namespace vulkancraft
{
	class Rigidbody
	{
	public:

		void object_falling(AABBCollider& aabb_collider); // 因重力下落的 AABB Collider

	private:

		const float kGravity = 9.8f; // 全局重力
		bool is_ground_ = true; // 默认角色踩在地上
	};
}
