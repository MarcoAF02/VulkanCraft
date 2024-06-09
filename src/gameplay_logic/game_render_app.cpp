
#include "game_render_app.h"

namespace vulkancraft
{
	GameRender::GameRender()
	{
		create_global_pool();
	}

	GameRender::~GameRender() { }

	// 创建全局描述符池
	void GameRender::create_global_pool()
	{
		global_pool_ = VulkanBaseDescriptorPool::Builder(game_device_)
			.set_max_sets(GameSwapChain::kMaxFramesInFlight)
			.add_pool_size(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, GameSwapChain::kMaxFramesInFlight)
			.add_pool_size(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, GameSwapChain::kMaxFramesInFlight)
			.build();
	}

	// 初始化渲染系统
	void initialize_render_system()
	{
		std::vector<std::unique_ptr<GameBuffer>> ubo_buffer_vector(GameSwapChain::kMaxFramesInFlight);

	}

	// 加载游戏对象
	void GameRender::load_game_object()
	{

	}

	// 更新渲染主循环
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

					// 窗口大小改变事件
					if (game_window_.sdl_event_.window.event == SDL_WINDOWEVENT_SIZE_CHANGED)
					{
						std::cout << "窗口大小改变" << std::endl;

						game_window_.recreate_sdl_surface();

						// TODO: 在这里重建 Vulkan SwapChain
					}

					break;
				}
			}
		}
	}

}
