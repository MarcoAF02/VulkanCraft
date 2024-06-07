
#pragma once

// 该宏取消 SDL 对 main 函数的接管
#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>
#include <SDL2/SDL_vulkan.h>

// Vulkan
#include <vulkan/vulkan.h>

// stl
#include <iostream>
#include <string>
#include <stdexcept>

// TODO: 重建表面交换链

namespace vulkancraft
{
	class GameWindow
	{
	public:

		int width_ = 0; // 窗口大小的定义在 game_main 类中，这里只是临时变量
		int height_ = 0;

		std::string window_name_ = nullptr;
		SDL_Window* sdl_window_ = nullptr;
		SDL_Surface* sdl_surface_ = nullptr;
		SDL_Renderer* sdl_renderer_ = nullptr; // SDL 基础渲染器 （注意它不要被什么别的函数带走）

		GameWindow(int width, int height, std::string name);
		~GameWindow();

		// C++ 11 安全性操作
		GameWindow(const GameWindow&) = delete;
		GameWindow& operator = (const GameWindow&) = delete;

		// HACK: 创建 Vulkan Surface
		void create_vulkan_window(VkInstance instance, VkSurfaceKHR* surface);

		// 重建 SDL 表面
		void recreate_sdl_surface();

	private:

		// 初始化 SDL 窗口
		bool init_sdl_window();

		// 创建 SDL 渲染器
		void create_sdl_renderer();

		// 测试 SDL 的 Rect 绘制
		void test_draw_sdl_rect();
	};
}
