
#include "rigidbody_system.h"

namespace vulkancraft
{
	void Rigidbody::object_falling(AABBCollider& aabb_collider)
	{
		// ������� Collider �ײ����ĸ���
		for (int i = 0; i < aabb_collider.get_aabb_bottom_vertices().size(); i++)
		{
			// TODO: �����ȱ������� AABB Collider��֮������취��һ�������Ż�����
		}
	}
}
