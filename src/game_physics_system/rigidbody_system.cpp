
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

	void Rigidbody::ground_check(AABBCollider& aabb_collider, float max_length)
	{
		glm::vec3 ray_direction(0.0f, +1.0f, 0.0f); // 射线发射的方向（和长度）

		// 遍历角色底部的四个点
		for (int i = 0; i < aabb_collider.get_aabb_bottom_vertices().size(); i++)
		{
			// std::cout << aabb_collider.get_aabb_bottom_vertices()[i].x << ", " << aabb_collider.get_aabb_bottom_vertices()[i].y << ", " << aabb_collider.get_aabb_bottom_vertices()[i].z << std::endl;

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

	void Rigidbody::wall_checking(AABBCollider& aabb_collider, float max_length)
	{
		glm::vec3 ray_direction_front(0.0f, 0.0f, +1.0f); // 前向射线发射的方向（和长度）
		glm::vec3 ray_direction_back(0.0f, 0.0f, -1.0f); // 后向射线发射的方向（和长度）
		glm::vec3 ray_direction_left(+1.0f, 0.0f, 0.0f); // 左侧射线发射的方向（和长度）
		glm::vec3 ray_direction_right(-1.0f, 0.0f, 0.0f); // 右侧射线发射的方向（和长度）

		// 遍历角色前面的四个点
		for (int i = 0; i < aabb_collider.get_aabb_front_vertices().size(); i++)
		{
			// std::cout << aabb_collider.get_aabb_bottom_vertices()[i].x << ", " << aabb_collider.get_aabb_bottom_vertices()[i].y << ", " << aabb_collider.get_aabb_bottom_vertices()[i].z << std::endl;

			// 射线发射点
			glm::vec3 ray_origin(aabb_collider.get_aabb_front_vertices()[i].x, aabb_collider.get_aabb_front_vertices()[i].y + kEpsilon, aabb_collider.get_aabb_front_vertices()[i].z);

			// TODO: 这里先遍历所有 AABB Collider，之后再用一个队列优化性能
			for (int j = 0; j < game_object_manager_->get_physical_obj_vector().size(); j++)
			{
				if (game_object_manager_ == nullptr) return;
				bool front_wall = game_object_manager_->get_physical_obj_vector()[j]->aabb_collider_.is_ray_intersects_aabb(ray_origin, ray_direction_front, max_length);
				if (front_wall) std::cout << "玩家前面顶墙了" << std::endl;
			}
		}

		// 遍历角色后面的四个点
		for (int i = 0; i < aabb_collider.get_aabb_back_vertices().size(); i++)
		{
			// std::cout << aabb_collider.get_aabb_bottom_vertices()[i].x << ", " << aabb_collider.get_aabb_bottom_vertices()[i].y << ", " << aabb_collider.get_aabb_bottom_vertices()[i].z << std::endl;

			// 射线发射点
			glm::vec3 ray_origin(aabb_collider.get_aabb_back_vertices()[i].x, aabb_collider.get_aabb_back_vertices()[i].y + kEpsilon, aabb_collider.get_aabb_back_vertices()[i].z);

			// TODO: 这里先遍历所有 AABB Collider，之后再用一个队列优化性能
			for (int j = 0; j < game_object_manager_->get_physical_obj_vector().size(); j++)
			{
				if (game_object_manager_ == nullptr) return;
				bool front_wall = game_object_manager_->get_physical_obj_vector()[j]->aabb_collider_.is_ray_intersects_aabb(ray_origin, ray_direction_back, max_length);
				if (front_wall) std::cout << "玩家后面顶墙了" << std::endl;
			}
		}

		// 遍历角色左侧的四个点
		for (int i = 0; i < aabb_collider.get_aabb_left_vertices().size(); i++)
		{
			// std::cout << aabb_collider.get_aabb_bottom_vertices()[i].x << ", " << aabb_collider.get_aabb_bottom_vertices()[i].y << ", " << aabb_collider.get_aabb_bottom_vertices()[i].z << std::endl;

			// 射线发射点
			glm::vec3 ray_origin(aabb_collider.get_aabb_left_vertices()[i].x, aabb_collider.get_aabb_left_vertices()[i].y + kEpsilon, aabb_collider.get_aabb_left_vertices()[i].z);

			// TODO: 这里先遍历所有 AABB Collider，之后再用一个队列优化性能
			for (int j = 0; j < game_object_manager_->get_physical_obj_vector().size(); j++)
			{
				if (game_object_manager_ == nullptr) return;
				bool front_wall = game_object_manager_->get_physical_obj_vector()[j]->aabb_collider_.is_ray_intersects_aabb(ray_origin, ray_direction_left, max_length);
				if (front_wall) std::cout << "玩家右侧顶墙了" << std::endl;
			}
		}

		// 遍历角色前面的四个点
		for (int i = 0; i < aabb_collider.get_aabb_left_vertices().size(); i++)
		{
			// std::cout << aabb_collider.get_aabb_bottom_vertices()[i].x << ", " << aabb_collider.get_aabb_bottom_vertices()[i].y << ", " << aabb_collider.get_aabb_bottom_vertices()[i].z << std::endl;

			// 射线发射点
			glm::vec3 ray_origin(aabb_collider.get_aabb_left_vertices()[i].x, aabb_collider.get_aabb_left_vertices()[i].y + kEpsilon, aabb_collider.get_aabb_left_vertices()[i].z);

			// TODO: 这里先遍历所有 AABB Collider，之后再用一个队列优化性能
			for (int j = 0; j < game_object_manager_->get_physical_obj_vector().size(); j++)
			{
				if (game_object_manager_ == nullptr) return;
				bool front_wall = game_object_manager_->get_physical_obj_vector()[j]->aabb_collider_.is_ray_intersects_aabb(ray_origin, ray_direction_right, max_length);
				if (front_wall) std::cout << "玩家左侧顶墙了" << std::endl;
			}
		}
	}

	void Rigidbody::free_falling(float delta_time, glm::vec3& target_pos)
	{
		//glm::vec3 gravity_velocity(0.0f, 0.0f, 0.0f);
		//float gravity_speed = 0.0f;

		//if (is_ground_) // 在地上
		//{
		//	accumulator_ = 0.0f; // 时间累积归零
		//	gravity_velocity = glm::vec3(0.0f, 0.0f, 0.0f);
		//	gravity_speed = 0.0f;
		//}
		//else // 在空中
		//{
		//	accumulator_ += delta_time; // 累加时间
		//	gravity_velocity += glm::vec3(0.0f, -1.0f, 0.0f) * kGravity * accumulator_ * delta_time; // 应用重力加速度到速度上
		//	target_pos -= gravity_velocity; // 更新物体的垂直位置

		//	// std::cout << accumulator_ << std::endl;
		//}
	}
}
