
#pragma once

// Bullet Physics
#include "btBulletDynamicsCommon.h"
#include <memory>

namespace vulkancraft
{
	// ������Ϸ����Ĵ�������
	struct PhysicsObjectCreateData
	{
		btVector3 obj_size{}; // Box Collider �ĳߴ�
		btVector3 obj_origin{}; // Box Collider �����
		btTransform transform{}; // �����Ҫ�� setOrigin() ��������ʼ��λ��
		btScalar mass{}; // ����
		btVector3 local_inertia{};
	};

	// TODO: ���������Ľ��ͬ������Ⱦ
	// ������������ࣺ�����ֻ������Ϸ������������
	class PhysicsObjectGenerator
	{
	public:
		PhysicsObjectGenerator(btDiscreteDynamicsWorld* dynamics_world, btAlignedObjectArray<btCollisionShape*> collision_shape_array_ptr);
		~PhysicsObjectGenerator();

		PhysicsObjectGenerator(const PhysicsObjectGenerator&) = delete;
		PhysicsObjectGenerator& operator = (const PhysicsObjectGenerator&) = delete;
		
		// TODO: ���� ID ������ɾ��������Ϸ����

		// ��������������
		void create_single_physics_block(PhysicsObjectCreateData data);

	private:

		// �����һ�������߳���ָ��ĸ���
		btDiscreteDynamicsWorld* dynamics_world_ = nullptr;
		btAlignedObjectArray<btCollisionShape*> collision_shape_array_ptr_;

	};
}
