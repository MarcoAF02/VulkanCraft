
#include "game_render_app.h"

class PhysicalSimulationApp;
struct PhysicsObjectCreateData;

namespace vulkancraft
{
	GameRenderApp::GameRenderApp()
	{
		create_global_pool();
		initialize_render_system();

		try
		{
			thread_state_manager_ = ThreadStateManager::get_instance();
			game_entity_manager_ = GameEntityManager::get_instance(); // 这里已经生成玩家了
		}
		catch (const std::exception& e)
		{
			throw std::runtime_error("某个单例类初始化失败：" + std::string(e.what()));
		}

		terrain_generation_ = std::make_shared<TerrainGeneration>
		(
			game_device_,
			*(thread_state_manager_->get_physical_simulation_app_ptr()),
			game_object_map_
		);

		update_render_window_content();
	}

	GameRenderApp::~GameRenderApp() { }

	void GameRenderApp::create_global_pool()
	{
		global_pool_ =
			VulkanBaseDescriptorPool::Builder(game_device_)
			.set_max_sets(GameSwapChain::kMaxFramesInFlight)
			.add_pool_size(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, GameSwapChain::kMaxFramesInFlight)
			.add_pool_size(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, GameSwapChain::kMaxFramesInFlight)
			.build();
	}

	void GameRenderApp::initialize_render_system()
	{
		ubo_buffer_vector_.resize((GameSwapChain::kMaxFramesInFlight));

		for (int i = 0; i < ubo_buffer_vector_.size(); i++)
		{
			ubo_buffer_vector_[i] = std::make_unique<GameBuffer>
				(
					game_device_,
					sizeof(GlobalUbo),
					1,
					VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
					VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
				);

			ubo_buffer_vector_[i]->map();
		}

		load_object_texture();

		global_set_layout_ =
			VulkanBaseDescriptorSetLayout::Builder(game_device_)
			.add_binding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS)
			.add_binding(1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT)
			.build();

		global_descriptor_set_vector_.resize((GameSwapChain::kMaxFramesInFlight));

		for (int i = 0; i < global_descriptor_set_vector_.size(); i++)
		{
			VkDescriptorBufferInfo buffer_info = ubo_buffer_vector_[i]->descriptor_info();

			VulkanBaseDescriptorWriter(*global_set_layout_, *global_pool_)
				.write_buffer(0, &buffer_info)
				.write_image(1, &image_info_)
				.build(global_descriptor_set_vector_[i]);
		}

		simple_render_system_ = std::make_unique<SimpleRenderSystem>
			(
				game_device_,
				game_renderer_.get_swap_chain_render_pass(),
				global_set_layout_->get_descriptor_set_layout()
			);

		point_light_system_ = std::make_unique<PointLightSystem>
			(
				game_device_,
				game_renderer_.get_swap_chain_render_pass(),
				global_set_layout_->get_descriptor_set_layout()
			);

	}

	void GameRenderApp::update_render_window_content()
	{
		// 测试用的数据
		PhysicsObjectCreateData new_data =
		{
			{1, 1, 1},
			{2000, 2000, 2000},
			{},
			0,
			{}
		};

		// HACK: 这里用掉一个键
		std::cout << thread_state_manager_ -> get_physical_simulation_app_ptr() << std::endl;
		thread_state_manager_->get_physical_simulation_app_ptr()->create_single_physics_block(124000, new_data);

		create_terrain();
		std::chrono::steady_clock::time_point current_time = std::chrono::high_resolution_clock::now();

		// HACK: 初始化玩家
		game_entity_manager_->get_character_controller()->init_character_controller(player_spawn_point_);

		while (!game_window_.should_close())
		{
			glfwPollEvents();

			std::chrono::steady_clock::time_point new_time = std::chrono::high_resolution_clock::now();
			float frame_time = std::chrono::duration<float, std::chrono::seconds::period>(new_time - current_time).count();
			current_time = new_time;

			// HACK: 打印玩家数据
			// game_entity_manager_->get_character_controller() -> print_player_details();

			// HACK: 设置观察摄像机
			viewer_camera_ = game_entity_manager_->get_character_controller()->get_player_camera();

			player_camera_view_.aspect = game_renderer_.get_aspect_ratio();
			player_camera_view_.fovy = glm::radians(60.0f);
			player_camera_view_.near = 0.1f;
			player_camera_view_.far = 200.0f;

			game_entity_manager_->get_character_controller()->set_player_camera(player_camera_view_);
			game_entity_manager_->get_character_controller()->init_mouse_rotate(game_window_.get_glfw_window());

			if (VkCommandBuffer_T* command_buffer = game_renderer_.begin_frame())
			{
				int frame_index = game_renderer_.get_frame_index();
				FrameInfo frame_info
				{
					frame_index,
					frame_time,
					command_buffer,
					viewer_camera_,
					global_descriptor_set_vector_[frame_index],
					game_object_map_
				};

				// 更新主相机渲染内容
				GlobalUbo ubo;
				ubo.projection = viewer_camera_.get_projection();
				ubo.view = viewer_camera_.get_view();
				ubo.inverse_view = viewer_camera_.get_inverse_view();

				// std::cout << sizeof(ubo) << std::endl;

				point_light_system_->update(frame_info, ubo);
				ubo_buffer_vector_[frame_index]->write_to_buffer(&ubo);
				ubo_buffer_vector_[frame_index]->flush();

				// 走完 render pass 的生命周期
				game_renderer_.begin_swap_chain_render_pass(command_buffer);

				// order here matters
				simple_render_system_->render_game_objects(frame_info);
				point_light_system_->render(frame_info);

				game_renderer_.end_swap_chain_render_pass(command_buffer);
				game_renderer_.end_frame();
			}
		}

		vkDeviceWaitIdle(game_device_.get_vulkan_device());

		std::cout << std::endl << "====== 渲染线程结束 ======" << std::endl;
	}

	void GameRenderApp::create_terrain()
	{
		//int plane_length = 20, plane_width = 20;
		//int wall_height = 6, wall_width = 6;

		//terrain_generation_->create_plane(plane_length, plane_width);
		//terrain_generation_->create_wall(wall_height, wall_width);

				// 生成方块物理数据
		PhysicsObjectCreateData obj_data = // 不知道传什么就打花括号
		{
			{1, 1, 1},
			{0, 0, 0},
			{},
			1,
			{}
		};

		// physical_simulation_app_->create_single_physics_block(obj_data);

		test_load_big_point_light();
	}

	void GameRenderApp::load_object_texture()
	{
		game_base_texture_ = std::make_unique<GameTexture>(game_device_, "textures/cobblestone.png");

		image_info_.sampler = game_base_texture_->get_sampler();
		image_info_.image_view = game_base_texture_->get_image_view();
		image_info_.image_layout = game_base_texture_->get_image_layout();
	}

#pragma region 测试用函数实现

	void GameRenderApp::test_load_viking_room_texture()
	{
		game_base_texture_ = std::make_unique<GameTexture>(game_device_, "textures/viking_room.png");

		image_info_.sampler = game_base_texture_->get_sampler();
		image_info_.image_view = game_base_texture_->get_image_view();
		image_info_.image_layout = game_base_texture_->get_image_layout();
	}

	void GameRenderApp::test_load_viking_room()
	{
		std::shared_ptr<GameModel> lve_model = GameModel::create_model_from_file(game_device_, "models/viking_room.obj");
		auto viking_room = BaseGameObject::create_game_object(true);
		viking_room.model_ = lve_model;
		viking_room.transform_.translation = { 0.0f, 0.6f, 0.0f };
		viking_room.transform_.rotation = { 3.1415926f / 2.0f, 3.1415926f, 0.0f };
		viking_room.transform_.scale = { 1.0f, 1.0f, 1.0f };
		game_object_map_.emplace(viking_room.get_id(), std::move(viking_room));
	}

	void GameRenderApp::test_load_big_point_light()
	{
		BaseGameObject point_light = BaseGameObject::make_point_light(6.0f);
		point_light.color_ = glm::vec3(1.0f, 0.8f, 0.6f);
		point_light.transform_.translation = { 6.0f, -4.0f, 4.0f };
		game_object_map_.emplace(point_light.get_id(), std::move(point_light));
	}

	void GameRenderApp::test_load_rotate_light()
	{
		// 硬编码灯光
		std::vector<glm::vec3> light_color_vector
		{
			{1.f, .1f, .1f},
			{.1f, .1f, 1.f},
			{.1f, 1.f, .1f},
			{1.f, 1.f, .1f},
			{.1f, 1.f, 1.f},
			{1.f, 1.f, 1.f}
		};

		for (int i = 0; i < light_color_vector.size(); i++)
		{
			BaseGameObject point_light = BaseGameObject::make_point_light(2.0f);
			point_light.color_ = light_color_vector[i];

			auto rotate_light = glm::rotate
			(
				glm::mat4(1.f),
				(i * glm::two_pi<float>()) / light_color_vector.size(),
				{ 0.f, -1.f, 0.f }
			);

			point_light.transform_.translation = glm::vec3(rotate_light * glm::vec4(-1.f, -1.f, -1.f, 1.f));
			game_object_map_.emplace(point_light.get_id(), std::move(point_light));
		}
	}

#pragma endregion

}
