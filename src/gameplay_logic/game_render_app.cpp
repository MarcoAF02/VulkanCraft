
#include "game_render_app.h"

namespace vulkancraft
{
	GameRednerApp::GameRednerApp()
	{
		update_render_window_content();
	}

	GameRednerApp::~GameRednerApp() { }

	void GameRednerApp::update_render_window_content()
	{
		while (true)
		{
			if (SDL_PollEvent(&sdl_event_))
			{
				switch (sdl_event_.type)
				{
				case SDL_QUIT: return;

				case SDL_WINDOWEVENT:

					// ���ڴ�С�ı��¼�
					if (sdl_event_.window.event == SDL_WINDOWEVENT_SIZE_CHANGED)
					{
						std::cout << "���ڴ�С�ı�" << std::endl;

						game_window_.recreate_sdl_surface();

						// TODO: �������ؽ� Vulkan SwapChain
					}

					break;
				}
			}
		}
	}

}
