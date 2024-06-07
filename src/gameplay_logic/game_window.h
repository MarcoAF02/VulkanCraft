
#pragma once

// �ú�ȡ�� SDL �� main �����Ľӹ�
#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>
#include <SDL2/SDL_vulkan.h>

// Vulkan
#include <vulkan/vulkan.h>

// stl
#include <iostream>
#include <string>
#include <stdexcept>

// TODO: �ؽ����潻����

namespace vulkancraft
{
	class GameWindow
	{
	public:

		int width_ = 0; // ���ڴ�С�Ķ����� game_main ���У�����ֻ����ʱ����
		int height_ = 0;

		std::string window_name_ = nullptr;
		SDL_Window* sdl_window_ = nullptr;
		SDL_Surface* sdl_surface_ = nullptr;
		SDL_Renderer* sdl_renderer_ = nullptr; // SDL ������Ⱦ�� ��ע������Ҫ��ʲô��ĺ������ߣ�

		GameWindow(int width, int height, std::string name);
		~GameWindow();

		// C++ 11 ��ȫ�Բ���
		GameWindow(const GameWindow&) = delete;
		GameWindow& operator = (const GameWindow&) = delete;

		// HACK: ���� Vulkan Surface
		void create_vulkan_window(VkInstance instance, VkSurfaceKHR* surface);

		// �ؽ� SDL ����
		void recreate_sdl_surface();

	private:

		// ��ʼ�� SDL ����
		bool init_sdl_window();

		// ���� SDL ��Ⱦ��
		void create_sdl_renderer();

		// ���� SDL �� Rect ����
		void test_draw_sdl_rect();
	};
}
