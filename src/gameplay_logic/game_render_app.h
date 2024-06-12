
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

		void initialize_render_system(); // 初始化渲染系统
		void update_render_window_content(); // 渲染窗口主循环
		void load_game_object(); // 加载游戏对象
		void load_object_texture(); // 加载纹理贴图

		const int kWidth = 1280;
		const int kHeight = 800;

		const std::string kWindowName = "VulkanCraft";

#pragma region 测试用函数

		void test_load_viking_room();
		void test_load_viking_room_texture();

#pragma endregion

	private:

		GameWindow game_window_{ kWidth, kHeight, kWindowName };
		GameDevice game_device_{game_window_};
		GameRenderer game_renderer_{game_window_, game_device_};

		std::unique_ptr<VulkanBaseDescriptorPool> global_pool_ {}; // 全局描述符池
		std::unique_ptr<GameTexture> game_base_texture_ {}; // 游戏贴图
		BaseGameObject::Map game_object_map_; // 以字典储存的 game_object

		GameBaseCamera viewer_camera_; // 观察摄像机
		BaseGameObject viewer_object_; // 观察者游戏物体，控制器通过这个控制创建出来的摄像机
		KeyboardMovementController camera_controller_; // 键盘控制器

		std::unique_ptr<SimpleRenderSystem> simple_render_system_;
		std::unique_ptr<PointLightSystem> point_light_system_; // 渲染系统放在这里

		std::vector<std::unique_ptr<GameBuffer>> ubo_buffer_vector_;
		std::vector<VkDescriptorSet> global_descriptor_set_vector_;
		std::unique_ptr<VulkanBaseDescriptorSetLayout> global_set_layout_;
		VkDescriptorImageInfo image_info_;

		// 创建系统描述符池
		void create_global_pool();

	};
}
