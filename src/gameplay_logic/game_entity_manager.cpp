
#include "game_entity_manager.h"

namespace vulkancraft
{
	GameEntityManager::GameEntityManager(std::shared_ptr<GameWindow> game_window)
	{
		game_window_ = game_window;
		character_controller_ = std::make_shared<CharacterController>(game_window_ -> get_glfw_window());
	}

	GameEntityManager::~GameEntityManager() { }
}
