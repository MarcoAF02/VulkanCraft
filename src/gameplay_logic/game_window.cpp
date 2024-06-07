
#include "game_window.h"

namespace vulkancraft
{
	GameWindow::GameWindow(int width, int height, std::string name) : width_ {width}, height_ {height}, window_name_ {name}
	{
		if (init_sdl_window())
		{
			std::cout << "========================================" << std::endl;
			std::cout << "GameWindow 类初始化成功" << std::endl;
			std::cout << "========================================" << std::endl;
		}
		else
		{
			throw std::runtime_error("GameWindow 类初始化失败");
		}
	}

	GameWindow::~GameWindow()
	{
		SDL_DestroyWindow(sdl_window_);
		SDL_DestroyRenderer(sdl_renderer_);
		SDL_Quit();
	}

	bool GameWindow::init_sdl_window()
	{
		// 初始化 SDL 窗口并启动
		if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
		{
			throw std::runtime_error("SDL 系统初始化失败");
			return false;
		}
		else
		{
			// HACK: 调用了 Vulkan 的头文件就必须要使用 SDL_WINDOW_VULKAN
			sdl_window_ = SDL_CreateWindow(window_name_.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width_, height_, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE | SDL_WINDOW_VULKAN);

			// sdl_surface_ = SDL_GetWindowSurface(sdl_window_); // 这里会带走 sdl_window_ 指针的所有权

			create_sdl_renderer(); // 创建 SDL Renderer 并绘制屏幕

			return true;
		}
	}

	void GameWindow::create_vulkan_window(VkInstance instance, VkSurfaceKHR* surface)
	{
		if (!SDL_Vulkan_CreateSurface(sdl_window_, instance, surface))
		{
			throw std::runtime_error("Vulkan 关联至 SDL 失败");
		}

		std::cout << "Vulkan 成功关联至 SDL" << std::endl;
	}

	void GameWindow::create_sdl_renderer()
	{
		sdl_renderer_ = SDL_CreateRenderer(sdl_window_, -1, 0);

		if (sdl_renderer_ == nullptr)
		{
			throw std::runtime_error("SDL Renderer 创建失败");
		}

		test_draw_sdl_rect();
	}

	void GameWindow::test_draw_sdl_rect()
	{
		SDL_SetRenderDrawColor(sdl_renderer_, 0, 0, 0, 255); // 设置背景颜色
		SDL_RenderClear(sdl_renderer_); // 清除屏幕
		SDL_SetRenderDrawColor(sdl_renderer_, 0, 255, 0, 255); // 设置下一帧要渲染的颜色

		int current_width = 0;
		int current_height = 0;

		SDL_GetWindowSize(sdl_window_, &current_width, &current_height);

		SDL_Rect test_rect = { current_width / 4, current_height / 4, 200, 200 }; // 创建 Rect 并设置大小
		SDL_RenderFillRect(sdl_renderer_, &test_rect);

		SDL_RenderPresent(sdl_renderer_); // 上屏显示
	}

	void GameWindow::recreate_sdl_surface()
	{
		test_draw_sdl_rect();

		// TODO: 重建 Vulkan 交换链
	}

}
