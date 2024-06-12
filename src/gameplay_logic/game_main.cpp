
#include "game_window.h"
#include "game_render_app.h"

// stl
#include <iostream>
#include <memory>
#include <string>
#include <stdexcept>
#include <thread>

// ��������
void create_render_app();

// HACK: ȫ��Ϸ����ڵ�
int main(void)
{
	try
	{
		std::thread render_thread(create_render_app);
		render_thread.join(); // �ȴ����߳����
	}
	catch (const std::system_error& e)
	{
		std::cerr << "�̴߳���ʧ��: " << e.what() << std::endl;
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
		std::cout << "GameRender App ����ʧ�ܣ�" << e.what() << std::endl;
	}
}
