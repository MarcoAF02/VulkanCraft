
#include "game_window.h"

namespace vulkancraft
{
	GameWindow::GameWindow(int width, int height, std::string name) : width_ {width}, height_ {height}, window_name_ {name}
	{
		if (init_sdl_window())
		{
			std::cout << "========================================" << std::endl;
			std::cout << "GameWindow ���ʼ���ɹ�" << std::endl;
			std::cout << "========================================" << std::endl;
		}
		else
		{
			throw std::runtime_error("GameWindow ���ʼ��ʧ��");
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
		// ��ʼ�� SDL ���ڲ�����
		if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
		{
			throw std::runtime_error("SDL ϵͳ��ʼ��ʧ��");
			return false;
		}
		else
		{
			// HACK: ������ Vulkan ��ͷ�ļ��ͱ���Ҫʹ�� SDL_WINDOW_VULKAN
			sdl_window_ = SDL_CreateWindow(window_name_.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width_, height_, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE | SDL_WINDOW_VULKAN);

			// sdl_surface_ = SDL_GetWindowSurface(sdl_window_); // �������� sdl_window_ ָ�������Ȩ

			create_sdl_renderer(); // ���� SDL Renderer ��������Ļ

			return true;
		}
	}

	void GameWindow::create_vulkan_window(VkInstance instance, VkSurfaceKHR* surface)
	{
		if (!SDL_Vulkan_CreateSurface(sdl_window_, instance, surface))
		{
			throw std::runtime_error("Vulkan ������ SDL ʧ��");
		}

		std::cout << "Vulkan �ɹ������� SDL" << std::endl;
	}

	void GameWindow::create_sdl_renderer()
	{
		sdl_renderer_ = SDL_CreateRenderer(sdl_window_, -1, 0);

		if (sdl_renderer_ == nullptr)
		{
			throw std::runtime_error("SDL Renderer ����ʧ��");
		}

		test_draw_sdl_rect();
	}

	void GameWindow::test_draw_sdl_rect()
	{
		SDL_SetRenderDrawColor(sdl_renderer_, 0, 0, 0, 255); // ���ñ�����ɫ
		SDL_RenderClear(sdl_renderer_); // �����Ļ
		SDL_SetRenderDrawColor(sdl_renderer_, 0, 255, 0, 255); // ������һ֡Ҫ��Ⱦ����ɫ

		int current_width = 0;
		int current_height = 0;

		SDL_GetWindowSize(sdl_window_, &current_width, &current_height);

		SDL_Rect test_rect = { current_width / 4, current_height / 4, 200, 200 }; // ���� Rect �����ô�С
		SDL_RenderFillRect(sdl_renderer_, &test_rect);

		SDL_RenderPresent(sdl_renderer_); // ������ʾ
	}

	void GameWindow::recreate_sdl_surface()
	{
		test_draw_sdl_rect();

		// TODO: �ؽ� Vulkan ������
	}

}
