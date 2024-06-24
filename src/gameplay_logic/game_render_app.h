
#pragma once

#include "render_system_include.h"

namespace vulkancraft
{
	// 游戏渲染全流程类（Vulkan）
	class GameRenderApp
	{
	public:

		GameRenderApp();
		~GameRenderApp();

		GameRenderApp(const GameRenderApp&) = delete;
		GameRenderApp& operator = (const GameRenderApp&) = delete;

		const int kWidth = 1280;
		const int kHeight = 800;
		const std::string kWindowName = "VulkanCraft";

		void initialize_render_system(); // 初始化渲染系统
		void update_render_window_content(); // 渲染窗口主循环
		void load_game_object(); // 加载游戏对象
		void load_object_texture(); // 加载纹理贴图

#pragma region 测试用函数

		void test_load_viking_room();
		void test_load_viking_room_texture();

#pragma endregion

	private:

		std::shared_ptr<GameObjectManager> game_object_manager_; // 游戏公共对象管理单例
		std::shared_ptr<ThreadStateManager> thread_state_manager_; // 线程监视器
		std::shared_ptr<GameEntityManager> game_entity_manager_;

		GameWindow game_window_{ kWidth, kHeight, kWindowName }; // 游戏窗口
		GameDevice game_device_{ game_window_ };
		GameRenderer game_renderer_{ game_window_, game_device_ };

		std::unique_ptr<VulkanBaseDescriptorPool> global_pool_ {}; // 全局描述符池
		std::unique_ptr<GameTexture> game_base_texture_ {}; // 游戏贴图
		BaseGameObject::Map game_object_map_; // 以字典储存的 game_object

		PlayerCameraView player_camera_view_;
		glm::vec3 player_spawn_point_ = {0.0f, 0.0f, -2.5f};

		// TODO: 下面这四个会被 character controller 自带
		// TODO: 这两个东西重新搬到 character controller 里去
		GameBaseCamera viewer_camera_; // 观察摄像机
		BaseGameObject viewer_object_; // 观察者游戏物体，控制器通过这个控制创建出来的摄像机

		//MouseRotateController mouse_rotate_comtroller_ = // 鼠标控制器
		//{
		//	game_window_.get_glfw_window()
		//};

		//KeyboardMovementController camera_controller_; // 键盘控制器
		// TODO: 上面这四个会被 character controller 自带

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
