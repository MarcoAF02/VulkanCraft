
#pragma once

#include "render_system_include.h"
#include "game_camera.h"

namespace vulkancraft
{
	// 游戏渲染全流程类（Vulkan）
	class GameRender
	{
	public:

		GameRender();
		~GameRender();

		void update_render_window_content(); // 渲染窗口主循环
		void load_game_object(); // 加载游戏对象

		const int kWidth = 1280;
		const int kHeight = 800;

		const std::string kWindowName = "VulkanCraft";

	private:

		GameWindow game_window_{ kWidth, kHeight, kWindowName };
		GameDevice game_device_{game_window_};
		GameRenderer game_renderer_{game_window_, game_device_};

		std::unique_ptr<VulkanBaseDescriptorPool> global_pool_ {}; // 全局描述符池
		std::unique_ptr<GameTexture> game_base_texture_ {}; // 游戏贴图
		BaseGameObject::Map game_object_map_; // 以字典储存的 game_object

	};
}
