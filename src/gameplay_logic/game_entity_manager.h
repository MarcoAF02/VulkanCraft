
#pragma once

#include "../gameplay_logic/character_controller.h" // 玩家角色控制器

// std
#include <vector>
#include <memory>
#include <condition_variable> // 条件变量，用于不同线程间的通信

// 单例类：游戏实体管理器
namespace vulkancraft
{
	class GameEntityManager
	{
	public:
		GameEntityManager();
		GameEntityManager(GLFWwindow* glfw_window);
		~GameEntityManager();

		GameEntityManager(const GameEntityManager&) = delete;
		GameEntityManager& operator = (const GameEntityManager&) = delete;

		void create_player();
		static GLFWwindow* get_glfw_window();

		// 得到单例，但是不用对指针赋值
		static std::shared_ptr<GameEntityManager> get_instance_no_glfw_ptr();

		// 得到单例，但是需要对指针赋值
		static std::shared_ptr<GameEntityManager> get_instance(GLFWwindow* glfw_window);
		static void wait_until_window_initialized(); // 等待 glfw_window 指针不为空

	private:
		static std::once_flag init_instance_flag_;
		static std::shared_ptr<GameEntityManager> instance_;

		mutable std::mutex mutex_; // 用于保护线程安全的互斥锁

		static std::condition_variable cv_; // 条件变量
		static std::mutex cv_mutex_; // 与条件变量配套的互斥锁
		static bool glfw_window_initialized_; // 标记 GLFWwindow 是否已初始化

		std::shared_ptr<CharacterController> character_controller_; // 玩家角色控制器
		static GLFWwindow* glfw_window_;

		// 初始化单例
		static void init_singleton();
	};

}
