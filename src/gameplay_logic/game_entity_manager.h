
#pragma once

#include "../gameplay_logic/character_controller.h" // ��ҽ�ɫ������

// std
#include <vector>
#include <memory>

namespace vulkancraft
{
	// TODO: ����������ɵ�����
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
