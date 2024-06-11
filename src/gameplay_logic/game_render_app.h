
#pragma once

#include "render_system_include.h"
#include "game_camera.h"

namespace vulkancraft
{
	// ��Ϸ��Ⱦȫ�����ࣨVulkan��
	class GameRender
	{
	public:

		GameRender();
		~GameRender();

		void update_render_window_content(); // ��Ⱦ������ѭ��
		void load_game_object(); // ������Ϸ����

		const int kWidth = 1280;
		const int kHeight = 800;

		const std::string kWindowName = "VulkanCraft";

	private:

		GameWindow game_window_{ kWidth, kHeight, kWindowName };
		GameDevice game_device_{game_window_};
		GameRenderer game_renderer_{game_window_, game_device_};

		std::unique_ptr<VulkanBaseDescriptorPool> global_pool_ {}; // ȫ����������
		std::unique_ptr<GameTexture> game_base_texture_ {}; // ��Ϸ��ͼ
		BaseGameObject::Map game_object_map_; // ���ֵ䴢��� game_object

	};
}
