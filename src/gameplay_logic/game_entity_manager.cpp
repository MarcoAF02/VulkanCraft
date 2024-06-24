
#include "game_entity_manager.h"

namespace vulkancraft
{
	// 初始化静态成员
	std::once_flag GameEntityManager::init_instance_flag_;
	std::shared_ptr<GameEntityManager> GameEntityManager::instance_ = nullptr;

	GameEntityManager::GameEntityManager(GLFWwindow* glfw_window) { }
	GameEntityManager::GameEntityManager() { }

	GameEntityManager::~GameEntityManager() { }

	void GameEntityManager::create_player()
	{
		character_controller_ = std::make_shared<CharacterController>();
	}

	void GameEntityManager::init_singleton()
	{
		instance_ = std::make_shared<GameEntityManager>();
	}

	std::shared_ptr<GameEntityManager> GameEntityManager::get_instance()
	{
		// 确保它只被执行了一次
		std::call_once(init_instance_flag_, GameEntityManager::init_singleton);
		return instance_;
	}

	std::shared_ptr<CharacterController> GameEntityManager::get_character_controller() const
	{
		return character_controller_; // 这里是只读的，不用加锁
	}

}
