
#pragma once

// Bullet Physics
#include "btBulletDynamicsCommon.h"

// ����������������õ�����
struct PhysicsObjectSaveData
{
	btCollisionShape* collision_shape;
	btRigidBody* rigid_body;
};
