
#pragma once

#include "render_system_include.h"
#include "../base_tools/details.h"

class ThreadStateManager;
class TerrainGeneration;
struct BlockGenerateData;
struct PhysicsObjectTransData;

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
		void create_terrain(); // 加载游戏对象（地形）
		void load_object_texture(); // 加载纹理贴图

#pragma region 游戏地形生成器

		// 静态方块生成器
		void single_block_creator(BlockGenerateData block_data);
		void create_plane(int length, int width); // 直接创建一个平面
		void create_wall(int height, int width); // 创建一面墙

#pragma endregion

#pragma region 同步物理对象数据

		void sync_trans_form_phy_obj();

#pragma endregion

#pragma region 测试用函数和变量

		void test_load_falling_cube(BlockGenerateData cube_data); // 加载一个自然下落的立方体
		void test_load_big_point_light(); // 加载一个巨大的点光源
		void test_load_rotate_light(); // 加载硬编码的旋转灯光
		void test_load_viking_room();
		void test_load_viking_room_texture();

#pragma endregion

	private:
		// 模型文件路径
		std::string model_file_path_ = "models/block.obj";

		std::shared_ptr<ThreadStateManager> thread_state_manager_;
		std::shared_ptr<GameEntityManager> game_entity_manager_;

		GameWindow game_window_{ kWidth, kHeight, kWindowName }; // 游戏窗口
		GameDevice game_device_{ game_window_ };
		GameRenderer game_renderer_{ game_window_, game_device_ };

		std::unique_ptr<VulkanBaseDescriptorPool> global_pool_{}; // 全局描述符池
		std::unique_ptr<GameTexture> game_base_texture_{}; // 游戏贴图
		BaseGameObject::RenderAppObjMap game_object_map_; // 以字典储存的 game_object

		PlayerCameraView player_camera_view_; // 构造玩家相机观察矩阵

		GameBaseCamera viewer_camera_; // 观察摄像机

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
