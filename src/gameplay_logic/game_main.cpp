
#include "game_window.h"
#include "game_render_app.h"
#include "physical_simulation_app.h"
#include "../managers/game_entity_manager.h"
#include "../managers/thread_state_manager.h"

// std
#include <iostream>
#include <memory>
#include <string>
#include <stdexcept>
#include <thread>
#include <unordered_map>

class vulkancraft::ThreadStateManager;

// HACK: Vulkan 的 NDC 默认使用的 Y 轴向下的右手坐标系以及 Y 轴向下的 FrameBuffer
// TODO: 物理线程还需要一个线程，就是对 Collider 的队列优化
// HACK: 全游戏的入口点

int main(void)
{
	std::shared_ptr<vulkancraft::ThreadStateManager> thread_state_manager = vulkancraft::ThreadStateManager::get_instance();
	thread_state_manager->create_render_thread();
	thread_state_manager->create_physical_thread();
	thread_state_manager->wait_for_threads();

	return 0;
}
