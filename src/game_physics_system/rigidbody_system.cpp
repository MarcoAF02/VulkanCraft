
#include "rigidbody_system.h"

namespace vulkancraft
{
	void Rigidbody::object_falling(AABBCollider& aabb_collider)
	{
		// 对于这个 Collider 底部的四个点
		for (int i = 0; i < aabb_collider.get_aabb_bottom_vertices().size(); i++)
		{
			// TODO: 这里先遍历所有 AABB Collider，之后再想办法用一个队列优化性能
		}
	}
}
