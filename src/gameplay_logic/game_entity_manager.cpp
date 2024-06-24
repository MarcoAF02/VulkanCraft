
#include "game_entity_manager.h"

namespace vulkancraft
{
	// 初始化静态成员
	std::once_flag GameEntityManager::init_instance_flag_;
	std::shared_ptr<GameEntityManager> GameEntityManager::instance_ = nullptr;
	struct GLFWwindow* vulkancraft::GameEntityManager::glfw_window_ = nullptr;

	std::condition_variable GameEntityManager::cv_;
	std::mutex GameEntityManager::cv_mutex_;
	bool GameEntityManager::glfw_window_initialized_ = false;

	GameEntityManager::GameEntityManager(GLFWwindow* glfw_window) { }
	GameEntityManager::GameEntityManager() { }

	GameEntityManager::~GameEntityManager() { }

	void GameEntityManager::create_player()
	{
		character_controller_ = std::make_shared<CharacterController>(glfw_window_);
	}

	void GameEntityManager::init_singleton()
	{
		instance_ = std::make_shared<GameEntityManager>(glfw_window_);
	}

	std::shared_ptr<GameEntityManager> GameEntityManager::get_instance_no_glfw_ptr()
	{
		// 确保它只被执行了一次
		std::call_once(init_instance_flag_, GameEntityManager::init_singleton);
		return instance_;
	}

	std::shared_ptr<GameEntityManager> GameEntityManager::get_instance(GLFWwindow* glfw_window)
	{
		glfw_window_ = glfw_window;
		glfw_window_initialized_ = true;
		cv_.notify_all(); // 通知所有等待的线程

		// 确保它只被执行了一次
		std::call_once(init_instance_flag_, GameEntityManager::init_singleton);
		return instance_;
	}

	GLFWwindow* GameEntityManager::get_glfw_window()
	{
		return glfw_window_;
	}

	void GameEntityManager::wait_until_window_initialized()
	{
		std::unique_lock<std::mutex> lock(cv_mutex_); // 确保这里的mutex_已经定义
		cv_.wait(lock, [] { return glfw_window_initialized_; });
	}

}
