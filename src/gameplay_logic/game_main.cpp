
#include "game_window.h"
#include "game_render_app.h"
#include "physical_simulation_app.h"
#include "../managers/game_object_manager.h"

// stl
#include <iostream>
#include <memory>
#include <string>
#include <stdexcept>
#include <thread>
#include <unordered_map>

// 注意 render app 类内的 gameobject_map 和单例类里的 gameobject_map 不是一种东西

// 函数声明
void create_render_app();
void create_physical_app();

// HACK: 全游戏的入口点
int main(void)
{
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
		std::unique_ptr<vulkancraft::GameRender> game_render = std::make_unique<vulkancraft::GameRender>();
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
