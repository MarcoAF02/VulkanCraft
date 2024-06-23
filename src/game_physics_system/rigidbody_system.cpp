
#include "rigidbody_system.h"

namespace vulkancraft
{
	Rigidbody::Rigidbody()
	{
		try
		{
			game_object_manager_ = GameObjectManager::get_instance();
		}
		catch (const std::exception& e)
		{
			throw std::runtime_error("某个单例类初始化失败：" + std::string(e.what()));
		}
	}

	Rigidbody::~Rigidbody() { }

	void Rigidbody::ground_check(AABBCollider& aabb_collider, float delta_time)
	{
		glm::vec3 ray_direction(0.0f, -1.0f, 0.0f); // 射线发射的方向

		for (int i = 0; i < aabb_collider.get_aabb_bottom_vertices().size(); i++)
		{			
			// 射线发射点
			glm::vec3 ray_origin(aabb_collider.get_aabb_bottom_vertices()[i].x, aabb_collider.get_aabb_bottom_vertices()[i].y - kEpsilon, aabb_collider.get_aabb_bottom_vertices()[i].z);

			// TODO: 这里先遍历所有 AABB Collider，之后再用一个队列优化性能
			for (int j = 0; j < game_object_manager_->get_physical_obj_vector().size(); j ++)
			{
				is_ground_ = game_object_manager_->get_physical_obj_vector()[j] -> aabb_collider_.is_ray_intersects_aabb(ray_origin, ray_direction);
			}
		}
	}

	glm::vec3 Rigidbody::free_falling(float delta_time)
	{
		float displacement_y = 0.5f * kGravity * delta_time * delta_time;
		return glm::vec3(0.0f, -displacement_y, 0.0f); // 返回一个位移向量，只有 y 轴有变化
	}
}
