
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

class vulkancraft::BaseGameObject;
struct vulkancraft::BlockGenerateData;

// 全局类型别名
using id_t = unsigned _int64;
using RenderAppObjMap = std::unordered_map<id_t, vulkancraft::BaseGameObject>;
using ObjectPublicDataMap = std::unordered_map<id_t, vulkancraft::GameObjectPublicData>;

// 函数声明
void create_render_app(); // 创建渲染线程

// HACK: Vulkan 的 NDC 默认使用的 Y 轴向下的右手坐标系以及 Y 轴向下的 FrameBuffer
// TODO: 物理线程还需要一个线程，就是对 AABB Collider 的队列优化
// HACK: 全游戏的入口点

int main(void)
{
	try
	{
		std::thread render_thread(create_render_app);
		render_thread.join(); // 等待渲染线程完成
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
