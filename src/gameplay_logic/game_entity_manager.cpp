
#include "game_entity_manager.h"

namespace vulkancraft
{
	// ��ʼ����̬��Ա
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
		// ȷ����ֻ��ִ����һ��
		std::call_once(init_instance_flag_, GameEntityManager::init_singleton);
		return instance_;
	}

	std::shared_ptr<CharacterController> GameEntityManager::get_character_controller() const
	{
		return character_controller_; // ������ֻ���ģ����ü���
	}

}
