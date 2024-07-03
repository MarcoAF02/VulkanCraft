
#include "physical_simulation_app.h"

namespace vulkancraft
{
	PhysicalSimulationApp::PhysicalSimulationApp()
	{
		try
		{
			game_object_manager_ = GameObjectManager::get_instance();
			thread_state_manager_ = ThreadStateManager::get_instance();
			game_entity_manager_ = GameEntityManager::get_instance(); // 这里已经生成玩家了

			// 得到全局原子指针，这里进行阻塞以等待渲染线程创建完成
			while (glfw_window_ == nullptr)
			{
				glfw_window_ = global_glfw_window_ptr.load(std::memory_order_acquire);
				// std::cout << "物理线程正在试图获取 GLFW 窗口指针，如果持续看到该信息，则 GameWindow 类初始化失败" << std::endl;
			}
		}
		catch (const std::exception& e)
		{
			throw std::runtime_error("某个单例类初始化失败：" + std::string(e.what()));
		}

		// HACK: 延迟一小段时间后再开始物理计算，防止类未初始化完毕
		// TODO: 后续的更新中，物理模拟改成阻塞到地形生成完成后再开始判断
		std::this_thread::sleep_for(std::chrono::milliseconds(1000));

		update_physical_simulation(); // 计算物理模拟循环
	}

	PhysicalSimulationApp::~PhysicalSimulationApp() { }

	void PhysicalSimulationApp::update_physical_simulation()
	{
		while (true)
		{
			if (thread_state_manager_->get_render_thread_state_to_phy())
			{
				std::cout << std::endl << "====== 检测到物理追赶结束，物理线程已自动结束 ======" << std::endl;
				break;
			}

			// 计算追赶时间
			auto current_time = Clock::now(); // 记录现在的时间
			auto elapsed_time = current_time - previous_time_; // 自上一帧以来的经过时间
			previous_time_ = current_time; // 更新上一帧的时间点

			accumulator_ += elapsed_time.count(); // 累计时间差

			// 计算更新时间间隔
			auto current_step = Clock::now(); // 记录现在的时间
			auto elapsed_step = current_step - previous_step_; // 自上一次以来的间隔时间
			previous_step_ = current_step;

			accumulator_step_ += elapsed_step.count(); // 累加间隔时间
			float accumulator_delta_time = static_cast<float>(accumulator_step_) / 1e9f; // 物理间隔高精度纳秒转换回 float 类型，单位为秒

			// if (accumulator_step_ <= kTimeStep) continue; // 大锁：CD 时间没到，放弃追赶更新

			// 追赶更新
			while (accumulator_ >= kTimePerUpdate)
			{
				if (thread_state_manager_->get_render_thread_state_to_phy())
				{
					std::cout << std::endl << "====== 检测到渲染线程结束，物理追赶已自动结束 ======" << std::endl;
					break;
				}

				// std::cout << accumulator_step_ << std::endl;

				if (accumulator_step_ <= kTimeStep) break; // CD 时间没到就不循环

				// ==================== HACK 这下面是物理循环 ==================== //

				// calculate_aabb_collider(); // 两两检测动态物体的 Collider 是否有碰撞

				game_entity_manager_->get_character_controller()->move(accumulator_delta_time, glfw_window_);
				game_entity_manager_->get_character_controller()->update_player_physics(accumulator_delta_time);
				game_entity_manager_->get_character_controller()->update_player_collision();

				// ==================== HACK 这上面是物理循环 ==================== //

				accumulator_ -= kTimePerUpdate; // 更新 accumulator（默认减去 1/60）
				accumulator_step_ = 0.0f; // 重置更新 CD 时间
			}

			// 帧率控制：计算剩余时间并延时
			double excess_time = accumulator_; // 这是累积的超出一个时间步的额外时间
			accumulator_ = std::fmod(accumulator_, kTimePerUpdate); // 更新 accumulator，去除完整的时间步，仅保留余数

			if (excess_time > 0)
			{
				// 如果有完整的时间步被处理掉（excess_time > 0），则不需要额外延时
				// 这里不需要sleep，因为已经在追赶更新中处理了时间
			}
			else
			{
				// 如果没有完整的时间步要处理（ 即 excess_time <= 0），则计算剩余到下一个更新的时间并延时
				double remaining_time = kTimePerUpdate - accumulator_;

				if (remaining_time > 0)
				{
					std::this_thread::sleep_for(std::chrono::duration<double>(remaining_time));
				}
				else
				{
					remaining_time = 0; // 确保 remaining_time 非负，避免潜在的负睡眠时间
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
					// std::cout << game_object_manager_->get_physical_obj_vector()[i]->get_pub_id() << " 号方块与 " << game_object_manager_->get_physical_obj_vector()[j]->get_pub_id() << " 号方块发生碰撞" << std::endl;
				}
			}
		}
	}

}
