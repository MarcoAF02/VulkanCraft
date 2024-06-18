
#include "physical_simulation_app.h"

namespace vulkancraft
{
	PhysicalSimulationApp::PhysicalSimulationApp()
	{
		try
		{
			game_object_manager_ = GameObjectManager::get_instance();
			thread_state_manager_ = ThreadStateManager::get_instance();
		}
		catch (const std::exception& e)
		{
			throw std::runtime_error("ĳ���������ʼ��ʧ�ܣ�" + std::string(e.what()));
		}

		update_physical_simulation(); // һ�����ͼ���
	}

	PhysicalSimulationApp::~PhysicalSimulationApp() { }

	void PhysicalSimulationApp::update_physical_simulation()
	{
		while (true)
		{
			if (thread_state_manager_->get_render_thread_state_to_phy())
			{
				std::cout << std::endl << "====== ��⵽����׷�Ͻ����������߳����Զ����� ======" << std::endl;
				break;
			}

			// ����׷��ʱ��
			auto current_time = Clock::now(); // ��¼���ڵ�ʱ��
			auto elapsed_time = current_time - previous_time_; // ����һ֡�����ľ���ʱ��
			previous_time_ = current_time; // ������һ֡��ʱ���

			accumulator_ += elapsed_time.count(); // �ۼ�ʱ���

			// �������ʱ����
			auto current_step = Clock::now(); // ��¼���ڵ�ʱ��
			auto elapsed_step = current_step - previous_step_; // ����һ�������ļ��ʱ��
			previous_step_ = current_step;

			accumulator_step_ += elapsed_step.count(); // �ۼӼ��ʱ��

			// if (accumulator_step_ <= kTimeStep) continue; // ������CD ʱ��û��������׷�ϸ���

			// ׷�ϸ���
			while (accumulator_ >= kTimePerUpdate)
			{
				if (thread_state_manager_->get_render_thread_state_to_phy())
				{
					std::cout << std::endl << "====== ��⵽��Ⱦ�߳̽���������׷�����Զ����� ======" << std::endl;
					break;
				}

				// std::cout << accumulator_step_ << std::endl;

				if (accumulator_step_ <= kTimeStep) break; // CD ʱ��û���Ͳ�ѭ��

				calculate_aabb_collider();

				accumulator_ -= kTimePerUpdate; // ���� accumulator��Ĭ�ϼ�ȥ 1/60��
				accumulator_step_ = 0.0f; // ���ø��� CD ʱ��
			}

			// ֡�ʿ��ƣ�����ʣ��ʱ�䲢��ʱ
			double excess_time = accumulator_; // �����ۻ��ĳ���һ��ʱ�䲽�Ķ���ʱ��
			accumulator_ = std::fmod(accumulator_, kTimePerUpdate); // ���� accumulator��ȥ��������ʱ�䲽������������

			if (excess_time > 0)
			{
				// �����������ʱ�䲽���������excess_time > 0��������Ҫ������ʱ
				// ���ﲻ��Ҫsleep����Ϊ�Ѿ���׷�ϸ����д�����ʱ��
			}
			else
			{
				// ���û��������ʱ�䲽Ҫ���� �� excess_time <= 0���������ʣ�ൽ��һ�����µ�ʱ�䲢��ʱ
				double remaining_time = kTimePerUpdate - accumulator_;

				if (remaining_time > 0)
				{
					std::this_thread::sleep_for(std::chrono::duration<double>(remaining_time));
				}
				else
				{
					remaining_time = 0; // ȷ�� remaining_time �Ǹ�������Ǳ�ڵĸ�˯��ʱ��
				}
			}
		}
	}

	void PhysicalSimulationApp::calculate_aabb_collider()
	{
		for (int i = 0; i < game_object_manager_->get_physical_obj_vector().size(); i++)
		{
			for (int j = i + 1; j < game_object_manager_->get_physical_obj_vector().size(); j++)
			{
				if (game_object_manager_->get_physical_obj_vector()[i]->
					aabb_collider_.is_two_aabb_collision(game_object_manager_->get_physical_obj_vector()[j]->aabb_collider_))
				{
					// std::cout << game_object_manager_->get_physical_obj_vector()[i]->get_pub_id() << " �ŷ����� " << game_object_manager_->get_physical_obj_vector()[j]->get_pub_id() << " �ŷ��鷢����ײ" << std::endl;
				}
			}
		}
	}

}
