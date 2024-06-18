
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
