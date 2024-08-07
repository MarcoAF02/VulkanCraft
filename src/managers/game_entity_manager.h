
#pragma once

#include "../gameplay_logic/gameplay_frame/finite_state_machine.h"

// std
#include <vector>
#include <memory>
#include <thread>
#include <mutex>

// 单例类：游戏实体管理器
namespace vulkancraft
{
	class GameEntityManager
	{
	public:
		GameEntityManager();
		~GameEntityManager();

		GameEntityManager(const GameEntityManager&) = delete;
		GameEntityManager& operator = (const GameEntityManager&) = delete;

		void create_player();

		static std::shared_ptr<GameEntityManager> get_instance();
		std::shared_ptr<CharacterController> get_character_controller();

	private:
		static std::once_flag init_instance_flag_;
		static std::shared_ptr<GameEntityManager> instance_;

		mutable std::mutex mutex_; // 用于保护线程安全的互斥锁

		std::shared_ptr<CharacterController> character_controller_; // 玩家角色控制器

		static void init_singleton(); // 初始化单例
	};

}
