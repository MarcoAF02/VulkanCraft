
#pragma once

// Bullet Physics
#include "btBulletDynamicsCommon.h"

namespace vulkancraft
{
	struct PhysicsObjectData
	{
		btVector3 obj_size{}; // Box Collider 的尺寸
		btVector3 obj_origin{}; // Box Collider 的起点
		btTransform transform{}; // 这个需要用 setOrigin() 函数来初始化位置
		btScalar mass{}; // 质量
		btVector3 local_inertia{};
	};

	// TODO: 把物理计算的结果同步回渲染
	// 物理对象生成类：这个类只负责游戏物体的物理计算
	class PhysicsObjectGenerator
	{
	public:
		PhysicsObjectGenerator(btDiscreteDynamicsWorld* dynamics_world, btAlignedObjectArray<btCollisionShape*>& collision_shape_array_ref);
		~PhysicsObjectGenerator();

		PhysicsObjectGenerator(const PhysicsObjectGenerator&) = delete;
		PhysicsObjectGenerator& operator = (const PhysicsObjectGenerator&) = delete;
		
		// 创建单个物理方块
		void create_single_physics_block(PhysicsObjectData data);

	private:

		// 这里存一份物理线程内指针的副本
		btDiscreteDynamicsWorld* dynamics_world_ = nullptr;
		btAlignedObjectArray<btCollisionShape*>& collision_shape_array_ref_;

	};
}
