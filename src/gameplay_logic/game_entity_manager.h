
#pragma once

#include "../gameplay_logic/character_controller.h" // 玩家角色控制器

// std
#include <vector>
#include <memory>

namespace vulkancraft
{
	// TODO: 这个必须做成单例类
	class GameEntityManager
	{
	public:
		GameEntityManager(std::shared_ptr<GameWindow> game_window);
		~GameEntityManager();

		std::shared_ptr<CharacterController> character_controller_;

	private:
		std::shared_ptr<GameWindow> game_window_;
	};

}
