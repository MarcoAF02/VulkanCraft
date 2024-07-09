
#include "physics_obj_generator.h"

namespace vulkancraft
{
	// ���������
	PhysicsObjectGenerator::PhysicsObjectGenerator(btDiscreteDynamicsWorld* dynamics_world, btAlignedObjectArray<btCollisionShape*> collision_shape_array_ptr) : dynamics_world_{dynamics_world}, collision_shape_array_ptr_{collision_shape_array_ptr} { }
	PhysicsObjectGenerator::~PhysicsObjectGenerator()
	{
		dynamics_world_ = nullptr;
	}

	void PhysicsObjectGenerator::create_single_physics_block(PhysicsObjectCreateData data)
	{
		// TODO: �� map ��¼�����Ϸ����
		btCollisionShape* single_block = new btBoxShape(data.obj_size); // ȷ����С

		// TODO: ������ʲ���
		collision_shape_array_ptr_.push_back(single_block);

		data.transform.setIdentity();
		bool is_dynamic = (data.mass != 0.0f);

		// ����Ƕ�̬���壬����������������
		if (is_dynamic) single_block -> calculateLocalInertia(data.mass, data.local_inertia);

		data.transform.setOrigin(data.obj_origin);

		btDefaultMotionState* block_motion_state = new btDefaultMotionState(data.transform);
		btRigidBody::btRigidBodyConstructionInfo rb_info(data.mass, block_motion_state, single_block, data.local_inertia);
		btRigidBody* body = new btRigidBody(rb_info);

		dynamics_world_->addRigidBody(body);
	}
}
