
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

	void Rigidbody::ground_check(AABBCollider& aabb_collider, float delta_time, float max_length)
	{
		is_ground_ = true; // HACK: 暂时关闭重力
		return;

		glm::vec3 ray_direction(0.0f, +1.0f, 0.0f); // 射线发射的方向（和长度）

		// 遍历角色底部的四个点
		for (int i = 0; i < aabb_collider.get_aabb_bottom_vertices().size(); i++)
		{
			// 射线发射点
			glm::vec3 ray_origin(aabb_collider.get_aabb_bottom_vertices()[i].x, aabb_collider.get_aabb_bottom_vertices()[i].y + kEpsilon, aabb_collider.get_aabb_bottom_vertices()[i].z);

			// TODO: 这里先遍历所有 AABB Collider，之后再用一个队列优化性能
			for (int j = 0; j < game_object_manager_->get_physical_obj_vector().size(); j++)
			{
				if (game_object_manager_ == nullptr) return;

				is_ground_ = game_object_manager_->get_physical_obj_vector()[j]->aabb_collider_.is_ray_intersects_aabb(ray_origin, ray_direction, max_length);

				if (is_ground_)
				{
					// std::cout << "检测到地面了" << std::endl;
					return; // 已经检测到地面，不再计算
				}
			}
		}

		is_ground_ = false; // 场景中没有任何 AABB 与玩家底部发生碰撞，因此玩家不在地上
		// std::cout << "没有检测到地面" << std::endl;
	}

	void Rigidbody::free_falling(float delta_time, glm::vec3& target_pos)
	{
		glm::vec3 gravity_velocity(0.0f, 0.0f, 0.0f);
		float gravity_speed = 0.0f;

		if (is_ground_) // 在地上
		{
			accumulator_ = 0.0f; // 时间累积归零
			gravity_velocity = glm::vec3(0.0f, 0.0f, 0.0f);
			gravity_speed = 0.0f;
		}
		else // 在空中
		{
			accumulator_ += delta_time; // 累加时间
			gravity_velocity += glm::vec3(0.0f, -1.0f, 0.0f) * kGravity * accumulator_ * delta_time; // 应用重力加速度到速度上
			target_pos -= gravity_velocity; // 更新物体的垂直位置

			// std::cout << accumulator_ << std::endl;
		}
	}
}
