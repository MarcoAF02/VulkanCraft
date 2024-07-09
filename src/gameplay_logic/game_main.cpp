
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

// HACK: Vulkan �� NDC Ĭ��ʹ�õ� Y �����µ���������ϵ�Լ� Y �����µ� FrameBuffer
// TODO: �����̻߳���Ҫһ���̣߳����Ƕ� Collider �Ķ����Ż�
// HACK: ȫ��Ϸ����ڵ�

int main(void)
{
	std::shared_ptr<vulkancraft::ThreadStateManager> thread_state_manager = vulkancraft::ThreadStateManager::get_instance();
	thread_state_manager->create_render_thread();
	thread_state_manager->create_physical_thread();
	thread_state_manager->wait_for_threads();

	return 0;
}
