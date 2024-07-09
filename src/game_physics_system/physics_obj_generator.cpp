
#include "physics_obj_generator.h"

namespace vulkancraft
{
	// 构造和析构
	PhysicsObjectGenerator::PhysicsObjectGenerator(btDiscreteDynamicsWorld* dynamics_world, btAlignedObjectArray<btCollisionShape*> collision_shape_array_ptr) : dynamics_world_{dynamics_world}, collision_shape_array_ptr_{collision_shape_array_ptr} { }
	PhysicsObjectGenerator::~PhysicsObjectGenerator()
	{
		dynamics_world_ = nullptr;
	}

	void PhysicsObjectGenerator::create_single_physics_block(PhysicsObjectCreateData data)
	{
		// TODO: 用 map 记录这个游戏对象
		btCollisionShape* single_block = new btBoxShape(data.obj_size); // 确定大小

		// TODO: 这里访问不到
		collision_shape_array_ptr_.push_back(single_block);

		data.transform.setIdentity();
		bool is_dynamic = (data.mass != 0.0f);

		// 如果是动态物体，设置受力张量矩阵
		if (is_dynamic) single_block -> calculateLocalInertia(data.mass, data.local_inertia);

		data.transform.setOrigin(data.obj_origin);

		btDefaultMotionState* block_motion_state = new btDefaultMotionState(data.transform);
		btRigidBody::btRigidBodyConstructionInfo rb_info(data.mass, block_motion_state, single_block, data.local_inertia);
		btRigidBody* body = new btRigidBody(rb_info);

		dynamics_world_->addRigidBody(body);
	}
}
