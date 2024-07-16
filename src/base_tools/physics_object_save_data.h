
#pragma once

// Bullet Physics
#include "btBulletDynamicsCommon.h"

// 跟踪物理世界对象用的数据
struct PhysicsObjectSaveData
{
	btCollisionShape* collision_shape;
	btRigidBody* rigid_body;
};
