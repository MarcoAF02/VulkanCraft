
#pragma once

// 渲染，物理 APP
#include "../gameplay_logic/game_render_app.h"
#include "../gameplay_logic/physical_simulation_app.h"

// stl
#include <iostream>
#include <memory>
#include <stdexcept>
#include <atomic>
#include <mutex>

class GameRenderApp;
class PhysicalSimulationApp;

// TODO: 程序启动时先创建这个单例类，单例类分别创建 物理 APP，渲染 APP

namespace vulkancraft
{
	// 线程管理器
	class ThreadStateManager
	{
	public:

		ThreadStateManager();
		~ThreadStateManager();

		ThreadStateManager(const ThreadStateManager&) = delete;
		ThreadStateManager& operator = (const ThreadStateManager&) = delete;

		static std::shared_ptr<ThreadStateManager> get_instance(); // 得到单例

		void create_render_thread(); // 创建渲染线程
		void create_physical_thread(); // 创建物理线程
		void wait_for_threads(); // 等待线程执行完成

		std::shared_ptr<PhysicalSimulationApp> get_physical_simulation_app_ptr() const {return physical_simulation_app_;}

		void set_render_thread_state_to_phy(bool new_state);
		bool& get_render_thread_state_to_phy();

	private:

		static std::once_flag init_instance_flag_;
		static std::shared_ptr<ThreadStateManager> instance_;

		std::thread render_thread_; // 渲染 APP 的线程
		std::thread physical_thread_; // 物理 APP 的线程

		// 指向两个线程的智能指针
		std::shared_ptr<GameRenderApp> game_render_app_;
		std::shared_ptr<PhysicalSimulationApp> physical_simulation_app_;

		// TODO: 这个可能要去掉
		bool render_thread_finished_to_phy_ = false; // 记录渲染线程是否结束
		mutable std::mutex mutex_; // 用于保护线程安全的互斥锁

		// 初始化单例
		static void init_singleton();

		void create_render_app(); // 创建渲染 App
		void create_physical_app(); // 创建物理 App
	};
}
