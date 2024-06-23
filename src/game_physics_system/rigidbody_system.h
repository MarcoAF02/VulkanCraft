
#pragma once

#include "../game_physics_system/physics_aabb.h"

#include <iostream>
#include <memory>

// ����ϵͳ

namespace vulkancraft
{
	class Rigidbody
	{
	public:

		void object_falling(AABBCollider& aabb_collider); // ����������� AABB Collider

	private:

		const float kGravity = 9.8f; // ȫ������
		bool is_ground_ = true; // Ĭ�Ͻ�ɫ���ڵ���
	};
}
