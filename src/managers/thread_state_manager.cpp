
#include "thread_state_manager.h"

namespace vulkancraft
{
	std::once_flag ThreadStateManager::init_instance_flag_;
	std::shared_ptr<ThreadStateManager> ThreadStateManager::instance_ = nullptr;

	ThreadStateManager::ThreadStateManager() { } // 啥也没有
	ThreadStateManager::~ThreadStateManager() { } // 啥也没有

	void ThreadStateManager::init_singleton()
	{
		instance_ = std::make_shared<ThreadStateManager>();
	}

	std::shared_ptr<ThreadStateManager> ThreadStateManager::get_instance()
	{
		std::call_once(init_instance_flag_, ThreadStateManager::init_singleton);
		return instance_;
	}

	void ThreadStateManager::create_render_thread()
	{
		// 创建一个线程，执行 create_render_app 函数
		render_thread_ = std::thread(&ThreadStateManager::create_render_app, this);
	}

	void ThreadStateManager::create_physical_thread()
	{
		// 创建物理模拟 App
		physical_simulation_app_ = std::make_shared<PhysicalSimulationApp>();
	}

	void ThreadStateManager::create_render_app()
	{
		game_render_app_ = std::make_shared<GameRenderApp>();
	}

	void ThreadStateManager::wait_for_threads()
	{
		if (render_thread_.joinable()) render_thread_.join();
	}

	void ThreadStateManager::set_render_thread_state_to_phy(bool new_state)
	{
		std::lock_guard<std::mutex> lock(mutex_); // 确保线程安全
		render_thread_finished_to_phy_ = new_state;
	}

	bool& ThreadStateManager::get_render_thread_state_to_phy()
	{
		std::lock_guard<std::mutex> lock(mutex_); // 确保线程安全
		return render_thread_finished_to_phy_;
	}
}
