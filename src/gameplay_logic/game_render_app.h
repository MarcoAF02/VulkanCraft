
#pragma once

#include "game_window.h"
#include "../vulkan_base/game_device.h"

// stl
#include <iostream>
#include <memory>
#include <string>

namespace vulkancraft
{
	// 游戏渲染全流程类（Vulkan）
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

		// HACK: SDL 事件处理之所以会出现在这里，是因为渲染循环中有很多其他类和功能，不单单属于 SDL 窗口

		GameWindow game_window_{ kWidth, kHeight, KWindowName };
		GameDevice game_device_{game_window_};
		SDL_Event sdl_event_; // SDL 事件处理
	};
}
