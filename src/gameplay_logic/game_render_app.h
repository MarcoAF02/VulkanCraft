
#pragma once

#include "render_system_include.h"

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
		void load_game_object(); // ������Ϸ����
		void load_object_texture(); // ����������ͼ

#pragma region �����ú���

		void test_load_viking_room();
		void test_load_viking_room_texture();

#pragma endregion

	private:

		std::shared_ptr<GameObjectManager> game_object_manager_; // ��Ϸ�������������
		std::shared_ptr<ThreadStateManager> thread_state_manager_; // �̼߳�����
		std::shared_ptr<GameEntityManager> game_entity_manager_;

		GameWindow game_window_{ kWidth, kHeight, kWindowName }; // ��Ϸ����
		GameDevice game_device_{ game_window_ };
		GameRenderer game_renderer_{ game_window_, game_device_ };

		std::unique_ptr<VulkanBaseDescriptorPool> global_pool_ {}; // ȫ����������
		std::unique_ptr<GameTexture> game_base_texture_ {}; // ��Ϸ��ͼ
		BaseGameObject::Map game_object_map_; // ���ֵ䴢��� game_object

		PlayerCameraView player_camera_view_;
		glm::vec3 player_spawn_point_ = {0.0f, 0.0f, -2.5f};

		// TODO: �������ĸ��ᱻ character controller �Դ�
		// TODO: �������������°ᵽ character controller ��ȥ
		GameBaseCamera viewer_camera_; // �۲������
		BaseGameObject viewer_object_; // �۲�����Ϸ���壬������ͨ��������ƴ��������������

		//MouseRotateController mouse_rotate_comtroller_ = // ��������
		//{
		//	game_window_.get_glfw_window()
		//};

		//KeyboardMovementController camera_controller_; // ���̿�����
		// TODO: �������ĸ��ᱻ character controller �Դ�

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
