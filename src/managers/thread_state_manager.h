
#pragma once

// stl
#include <iostream>
#include <memory>
#include <stdexcept>
#include <atomic>
#include <mutex>

namespace vulkancraft
{
	// 线程状态监视器
	class ThreadStateManager
	{
	public:

		ThreadStateManager();
		~ThreadStateManager();

		ThreadStateManager(const ThreadStateManager&) = delete;
		ThreadStateManager& operator = (const ThreadStateManager&) = delete;

		static std::shared_ptr<ThreadStateManager> get_instance(); // 得到单例

		void set_render_thread_state_to_phy(bool new_state);
		bool& get_render_thread_state_to_phy();

	private:

		static std::once_flag init_instance_flag_;
		static std::shared_ptr<ThreadStateManager> instance_;

		bool render_thread_finished_to_phy_ = false; // 记录渲染线程是否结束
		mutable std::mutex mutex_; // 用于保护线程安全的互斥锁

		// 初始化单例
		static void init_singleton();
	};
}
