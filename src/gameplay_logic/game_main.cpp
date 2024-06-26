
#include "game_window.h"
#include "game_render_app.h"
#include "physical_simulation_app.h"
#include "../managers/game_object_manager.h"
#include "../managers/game_entity_manager.h"

// std
#include <iostream>
#include <memory>
#include <string>
#include <stdexcept>
#include <thread>
#include <unordered_map>

// 注意 render app 类内的 gameobject_map 和单例类里的 gameobject_map 不是一种东西

// 函数声明
void create_render_app(); // 创建渲染线程
void create_physical_app(); // 创建物理线程

// TODO: 物理线程还需要一个线程，就是对 AABB Collider 的队列优化

// HACK: 全游戏的入口点
int main(void)
{
	// TODO: 创建游戏窗口
	// TODO: 游戏窗口管理器必须是个单例类
	// TODO: 游戏实体管理器也必须是一个单例类
	// TODO: 这两个类是单例类，但是其内含的数据在堆内存中，单例类确保的是任何时候指针都指向同一块内存区域

	try
	{
		std::thread render_thread(create_render_app);
		std::thread physical_thread(create_physical_app);

		render_thread.join(); // 等待渲染线程完成
		physical_thread.join(); // 等待物理线程完成
	}
	catch (const std::system_error& e)
	{
		std::cerr << "渲染或物理线程执行失败: " << e.what() << std::endl;
		return -1;
	}

	return 0;
}

void create_render_app()
{
	try
	{
		std::unique_ptr<vulkancraft::GameRenderApp> game_render = std::make_unique<vulkancraft::GameRenderApp>();
		game_render->update_render_window_content();
	}
	catch (const std::exception& e)
	{
		std::cerr << "GameRender App 创建失败：" << e.what() << std::endl;
	}
}

void create_physical_app()
{
	try
	{
		std::unique_ptr<vulkancraft::PhysicalSimulationApp> game_physical = std::make_unique<vulkancraft::PhysicalSimulationApp>();
	}
	catch (const std::exception& e)
	{
		std::cerr << "GamePhysical App 创建失败：" << e.what() << std::endl;
	}
}
