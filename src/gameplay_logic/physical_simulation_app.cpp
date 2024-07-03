
#include "physical_simulation_app.h"

namespace vulkancraft
{
	PhysicalSimulationApp::PhysicalSimulationApp()
	{
		try
		{
			game_object_manager_ = GameObjectManager::get_instance();
			thread_state_manager_ = ThreadStateManager::get_instance();
			game_entity_manager_ = GameEntityManager::get_instance(); // �����Ѿ����������

			// �õ�ȫ��ԭ��ָ�룬������������Եȴ���Ⱦ�̴߳������
			while (glfw_window_ == nullptr)
			{
				glfw_window_ = global_glfw_window_ptr.load(std::memory_order_acquire);
				// std::cout << "�����߳�������ͼ��ȡ GLFW ����ָ�룬���������������Ϣ���� GameWindow ���ʼ��ʧ��" << std::endl;
			}
		}
		catch (const std::exception& e)
		{
			throw std::runtime_error("ĳ���������ʼ��ʧ�ܣ�" + std::string(e.what()));
		}

		// HACK: �ӳ�һС��ʱ����ٿ�ʼ������㣬��ֹ��δ��ʼ�����
		// TODO: �����ĸ����У�����ģ��ĳ�����������������ɺ��ٿ�ʼ�ж�
		std::this_thread::sleep_for(std::chrono::milliseconds(1000));

		update_physical_simulation(); // ��������ģ��ѭ��
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
			float accumulator_delta_time = static_cast<float>(accumulator_step_) / 1e9f; // �������߾�������ת���� float ���ͣ���λΪ��

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

				// ==================== HACK ������������ѭ�� ==================== //

				// calculate_aabb_collider(); // ������⶯̬����� Collider �Ƿ�����ײ

				game_entity_manager_->get_character_controller()->move(accumulator_delta_time, glfw_window_);
				game_entity_manager_->get_character_controller()->update_player_physics(accumulator_delta_time);
				game_entity_manager_->get_character_controller()->update_player_collision();

				// ==================== HACK ������������ѭ�� ==================== //

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
