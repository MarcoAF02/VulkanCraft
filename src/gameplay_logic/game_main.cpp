
#include "game_window.h"
#include "game_render_app.h"
#include "game_object_manager.h"

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

// HACK: 全游戏的入口点
int main(void)
{
	try
	{
		std::thread render_thread(create_render_app);
		render_thread.join(); // 等待新线程完成
	}
	catch (const std::system_error& e)
	{
		std::cerr << "线程创建失败: " << e.what() << std::endl;
		return -1;
	}

	return 0;
}

void create_render_app()
{
	try
	{
		std::unique_ptr<vulkancraft::GameRender> game_main = std::make_unique<vulkancraft::GameRender>();
		game_main->update_render_window_content();
	}
	catch (const std::exception& e)
	{
		std::cout << "GameRender App 创建失败：" << e.what() << std::endl;
	}
}
