
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
			throw std::runtime_error("ĳ���������ʼ��ʧ�ܣ�" + std::string(e.what()));
		}
	}

	Rigidbody::~Rigidbody() { }

	void Rigidbody::ground_check(AABBCollider& aabb_collider, float delta_time, float max_length)
	{
		is_ground_ = true; // HACK: ��ʱ�ر�����
		return;

		glm::vec3 ray_direction(0.0f, +1.0f, 0.0f); // ���߷���ķ��򣨺ͳ��ȣ�

		// ������ɫ�ײ����ĸ���
		for (int i = 0; i < aabb_collider.get_aabb_bottom_vertices().size(); i++)
		{
			// ���߷����
			glm::vec3 ray_origin(aabb_collider.get_aabb_bottom_vertices()[i].x, aabb_collider.get_aabb_bottom_vertices()[i].y + kEpsilon, aabb_collider.get_aabb_bottom_vertices()[i].z);

			// TODO: �����ȱ������� AABB Collider��֮������һ�������Ż�����
			for (int j = 0; j < game_object_manager_->get_physical_obj_vector().size(); j++)
			{
				if (game_object_manager_ == nullptr) return;

				is_ground_ = game_object_manager_->get_physical_obj_vector()[j]->aabb_collider_.is_ray_intersects_aabb(ray_origin, ray_direction, max_length);

				if (is_ground_)
				{
					// std::cout << "��⵽������" << std::endl;
					return; // �Ѿ���⵽���棬���ټ���
				}
			}
		}

		is_ground_ = false; // ������û���κ� AABB ����ҵײ�������ײ�������Ҳ��ڵ���
		// std::cout << "û�м�⵽����" << std::endl;
	}

	void Rigidbody::free_falling(float delta_time, glm::vec3& target_pos)
	{
		glm::vec3 gravity_velocity(0.0f, 0.0f, 0.0f);
		float gravity_speed = 0.0f;

		if (is_ground_) // �ڵ���
		{
			accumulator_ = 0.0f; // ʱ���ۻ�����
			gravity_velocity = glm::vec3(0.0f, 0.0f, 0.0f);
			gravity_speed = 0.0f;
		}
		else // �ڿ���
		{
			accumulator_ += delta_time; // �ۼ�ʱ��
			gravity_velocity += glm::vec3(0.0f, -1.0f, 0.0f) * kGravity * accumulator_ * delta_time; // Ӧ���������ٶȵ��ٶ���
			target_pos -= gravity_velocity; // ��������Ĵ�ֱλ��

			// std::cout << accumulator_ << std::endl;
		}
	}
}
