
#pragma once

#include "../gameplay_logic/character_controller.h" // 玩家角色控制器

// std
#include <vector>
#include <memory>

namespace vulkancraft
{
	// 这里储存各个实体的引用计数智能指针的引用，并负责各种实体的管理
	// 在游戏开始运行时，这个类作为参数传递给不同线程
	class GameEntityManager
	{
	public:

		std::shared_ptr<CharacterController> character_controller_;
	};

}
