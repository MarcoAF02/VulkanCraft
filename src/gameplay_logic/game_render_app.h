
#pragma once

#include "render_system_include.h"
#include "../base_tools/details.h"

class ThreadStateManager;
class TerrainGeneration;
struct BlockGenerateData;
struct PhysicsObjectTransData;

namespace vulkancraft
{
	// ��Ϸ��Ⱦȫ�����ࣨVulkan��
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

		void initialize_render_system(); // ��ʼ����Ⱦϵͳ
		void update_render_window_content(); // ��Ⱦ������ѭ��
		void create_terrain(); // ������Ϸ���󣨵��Σ�
		void load_object_texture(); // ����������ͼ

#pragma region ��Ϸ����������

		// ��̬����������
		void single_block_creator(BlockGenerateData block_data);
		void create_plane(int length, int width); // ֱ�Ӵ���һ��ƽ��
		void create_wall(int height, int width); // ����һ��ǽ

#pragma endregion

#pragma region ͬ�������������

		void sync_trans_form_phy_obj();

#pragma endregion

#pragma region �����ú����ͱ���

		void test_load_falling_cube(BlockGenerateData cube_data); // ����һ����Ȼ�����������
		void test_load_big_point_light(); // ����һ���޴�ĵ��Դ
		void test_load_rotate_light(); // ����Ӳ�������ת�ƹ�
		void test_load_viking_room();
		void test_load_viking_room_texture();

#pragma endregion

	private:
		// ģ���ļ�·��
		std::string model_file_path_ = "models/block.obj";

		std::shared_ptr<ThreadStateManager> thread_state_manager_;
		std::shared_ptr<GameEntityManager> game_entity_manager_;

		GameWindow game_window_{ kWidth, kHeight, kWindowName }; // ��Ϸ����
		GameDevice game_device_{ game_window_ };
		GameRenderer game_renderer_{ game_window_, game_device_ };

		std::unique_ptr<VulkanBaseDescriptorPool> global_pool_{}; // ȫ����������
		std::unique_ptr<GameTexture> game_base_texture_{}; // ��Ϸ��ͼ
		BaseGameObject::RenderAppObjMap game_object_map_; // ���ֵ䴢��� game_object

		PlayerCameraView player_camera_view_; // �����������۲����

		GameBaseCamera viewer_camera_; // �۲������

		std::unique_ptr<SimpleRenderSystem> simple_render_system_;
		std::unique_ptr<PointLightSystem> point_light_system_; // ��Ⱦϵͳ��������

		std::vector<std::unique_ptr<GameBuffer>> ubo_buffer_vector_;
		std::vector<VkDescriptorSet> global_descriptor_set_vector_;
		std::unique_ptr<VulkanBaseDescriptorSetLayout> global_set_layout_;
		VkDescriptorImageInfo image_info_;

		// ����ϵͳ��������
		void create_global_pool();

	};
}
