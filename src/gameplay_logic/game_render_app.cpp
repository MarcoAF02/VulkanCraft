
#include "game_render_app.h"

namespace vulkancraft
{
	GameRender::GameRender()
	{
		create_global_pool();
	}

	GameRender::~GameRender() { }

	// ����ȫ����������
	void GameRender::create_global_pool()
	{
		global_pool_ = VulkanBaseDescriptorPool::Builder(game_device_)
			.set_max_sets(GameSwapChain::kMaxFramesInFlight)
			.add_pool_size(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, GameSwapChain::kMaxFramesInFlight)
			.add_pool_size(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, GameSwapChain::kMaxFramesInFlight)
			.build();
	}

	// ��ʼ����Ⱦϵͳ
	void initialize_render_system()
	{
		std::vector<std::unique_ptr<GameBuffer>> ubo_buffer_vector(GameSwapChain::kMaxFramesInFlight);

	}

	// ������Ϸ����
	void GameRender::load_game_object()
	{

	}

	// ������Ⱦ��ѭ��
	void GameRender::update_render_window_content()
	{
		while (true)
		{
			if (SDL_PollEvent(&game_window_.sdl_event_))
			{
				switch (game_window_.sdl_event_.type)
				{
				case SDL_QUIT: return;

				case SDL_WINDOWEVENT:

					// ���ڴ�С�ı��¼�
					if (game_window_.sdl_event_.window.event == SDL_WINDOWEVENT_SIZE_CHANGED)
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
