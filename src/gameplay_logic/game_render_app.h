
#pragma once

#include "game_window.h"
#include "../vulkan_base/game_device.h"

// stl
#include <iostream>
#include <memory>
#include <string>

namespace vulkancraft
{
	// ��Ϸ��Ⱦȫ�����ࣨVulkan��
	class GameRednerApp
	{
	public:

		GameRednerApp();
		~GameRednerApp();

		void update_render_window_content();

		const int kWidth = 1280;
		const int kHeight = 800;

		const std::string KWindowName = "VulkanCraft";

	private:

		// HACK: SDL �¼�����֮���Ի�������������Ϊ��Ⱦѭ�����кܶ�������͹��ܣ����������� SDL ����

		GameWindow game_window_{ kWidth, kHeight, KWindowName };
		GameDevice game_device_{game_window_};
		SDL_Event sdl_event_; // SDL �¼�����
	};
}
