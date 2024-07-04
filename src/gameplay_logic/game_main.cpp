
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

// ע�� render app ���ڵ� gameobject_map �͵�������� gameobject_map ����һ�ֶ���

// ��������
void create_render_app(); // ������Ⱦ�߳�
void create_physical_app(); // ���������߳�

// HACK: Vulkan �� NDC Ĭ��ʹ�õ� Y �����µ���������ϵ�Լ� Y �����µ� FrameBuffer

// TODO: �����̻߳���Ҫһ���̣߳����Ƕ� AABB Collider �Ķ����Ż�

// HACK: ȫ��Ϸ����ڵ�
int main(void)
{
	try
	{
		std::thread render_thread(create_render_app);
		std::thread physical_thread(create_physical_app);

		render_thread.join(); // �ȴ���Ⱦ�߳����
		physical_thread.join(); // �ȴ������߳����
	}
	catch (const std::system_error& e)
	{
		std::cerr << "��Ⱦ�������߳�ִ��ʧ��: " << e.what() << std::endl;
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
		std::cerr << "GameRender App ����ʧ�ܣ�" << e.what() << std::endl;
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
		std::cerr << "GamePhysical App ����ʧ�ܣ�" << e.what() << std::endl;
	}
}
