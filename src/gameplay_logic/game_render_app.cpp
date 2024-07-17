
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
			game_entity_manager_ = GameEntityManager::get_instance(); // �����Ѿ����������
		}
		catch (const std::exception& e)
		{
			throw std::runtime_error("ĳ���������ʼ��ʧ�ܣ�" + std::string(e.what()));
		}

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
		BlockGenerateData cube_data_1 =
		{
			{2.0f, -10.0f, 2.0f},
			{0.0f, 0.0f, 0.0f},
			{1.0f, 1.0f, 1.0f},
			1.0f
		};

		BlockGenerateData cube_data_2 =
		{
			{2.0f, -20.0f, 2.0f},
			{0.0f, 0.0f, 0.0f},
			{1.0f, 1.0f, 1.0f},
			1.0f
		};

		create_terrain();
		test_load_falling_cube(cube_data_1);
		test_load_falling_cube(cube_data_2);
		std::chrono::steady_clock::time_point current_time = std::chrono::high_resolution_clock::now();

		// HACK: ��ʼ�����
		game_entity_manager_->get_character_controller()->init_character_controller();

		while (!game_window_.should_close())
		{
			glfwPollEvents();

			std::chrono::steady_clock::time_point new_time = std::chrono::high_resolution_clock::now();
			float frame_time = std::chrono::duration<float, std::chrono::seconds::period>(new_time - current_time).count();
			current_time = new_time;

			// HACK: ��ӡ�������
			// game_entity_manager_->get_character_controller() -> print_player_details();

			// HACK: ���ù۲������
			viewer_camera_ = game_entity_manager_->get_character_controller()->get_player_camera();

			player_camera_view_.aspect = game_renderer_.get_aspect_ratio();
			player_camera_view_.fovy = glm::radians(60.0f);
			player_camera_view_.near = 0.1f;
			player_camera_view_.far = 200.0f;

			game_entity_manager_->get_character_controller()->set_player_camera(player_camera_view_);
			game_entity_manager_->get_character_controller()->init_mouse_rotate(game_window_.get_glfw_window());
			game_entity_manager_->get_character_controller()->rotate(frame_time, game_window_.get_glfw_window());

			sync_trans_form_phy_obj(); // ͬ�������������������Ⱦ

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

				// �����������Ⱦ����
				GlobalUbo ubo;
				ubo.projection = viewer_camera_.get_projection();
				ubo.view = viewer_camera_.get_view();
				ubo.inverse_view = viewer_camera_.get_inverse_view();

				// std::cout << sizeof(ubo) << std::endl;

				point_light_system_->update(frame_info, ubo);
				ubo_buffer_vector_[frame_index]->write_to_buffer(&ubo);
				ubo_buffer_vector_[frame_index]->flush();

				// ���� render pass ����������
				game_renderer_.begin_swap_chain_render_pass(command_buffer);

				// order here matters
				simple_render_system_->render_game_objects(frame_info);
				point_light_system_->render(frame_info);

				game_renderer_.end_swap_chain_render_pass(command_buffer);
				game_renderer_.end_frame();
			}
		}

		vkDeviceWaitIdle(game_device_.get_vulkan_device());

		std::cout << std::endl << "====== ��Ⱦ�߳̽��� ======" << std::endl;
	}

	void GameRenderApp::create_terrain()
	{
		int plane_length = 20, plane_width = 20;
		int wall_height = 6, wall_width = 6;

		create_plane(plane_length, plane_width);
		create_wall(wall_height, wall_width);

		test_load_big_point_light();
	}

	void GameRenderApp::load_object_texture()
	{
		game_base_texture_ = std::make_unique<GameTexture>(game_device_, "textures/cobblestone.png");

		image_info_.sampler = game_base_texture_->get_sampler();
		image_info_.image_view = game_base_texture_->get_image_view();
		image_info_.image_layout = game_base_texture_->get_image_layout();
	}

#pragma region ��Ϸ����������

	void GameRenderApp::single_block_creator(BlockGenerateData block_data)
	{
		std::shared_ptr<GameModel> stone_model = GameModel::create_model_from_file(game_device_, model_file_path_);
		BaseGameObject stone_obj = BaseGameObject::create_game_object(false);

		stone_obj.model_ = stone_model;
		stone_obj.transform_.translation = block_data.position;
		stone_obj.transform_.rotation = block_data.rotation;
		stone_obj.transform_.scale = block_data.scale;

		// ����ĳߴ�պ�����Ⱦ��һ��
		btVector3 obj_size{ block_data.scale.x / 2, block_data.scale.y / 2, block_data.scale.z / 2 };
		btVector3 obj_origin{ block_data.position.x, block_data.position.y, block_data.position.z };
		float obj_mass = block_data.mass;

		// ���ɷ�����������
		PhysicsObjectCreateData obj_data = // ��֪����ʲô�ʹ�����
		{
			obj_size,
			obj_origin,
			{},
			obj_mass,
			{}
		};

		// ��Ⱦ���ݷ��� game object map
		game_object_map_.emplace(stone_obj.get_id(), std::move(stone_obj));
		thread_state_manager_->get_physical_simulation_app_ptr()->create_single_physics_block(stone_obj.get_id(), obj_data);
	}

	void GameRenderApp::create_plane(int length, int width)
	{
		BlockGenerateData new_data =
		{
			{ 0.0f, 0.0f, 0.0f },
			{ 0.0f, 0.0f, 0.0f },
			{ 1.0f, 1.0f, 1.0f }
		};

		new_data.position.x = 0;

		for (int j = 0; j < width; j++) // ���������һ��
		{
			single_block_creator(new_data);
			new_data.position.z += 1;
		}

		new_data.position.z = 0;

		for (int i = 0; i < length; i++) // ���� plane �ı߳�
		{
			single_block_creator(new_data);
			new_data.position.x += 1;

			for (int j = 0; j < width; j++)
			{
				single_block_creator(new_data);
				new_data.position.z += 1;
			}

			new_data.position.z = 0;
		}
	}

	void GameRenderApp::create_wall(int height, int width)
	{
		BlockGenerateData new_data =
		{
			{ 4.0f, 0.0f, 8.0f },
			{ 0.0f, 0.0f, 0.0f },
			{ 1.0f, 1.0f, 1.0f }
		};

		new_data.position.x = 4.0f;

		for (int j = 0; j < width; j++) // ���������һ��
		{
			single_block_creator(new_data);
			new_data.position.y -= 1;
		}

		new_data.position.y = 0;

		for (int i = 0; i < height; i++) // ���� plane �ı߳�
		{
			single_block_creator(new_data);
			new_data.position.x += 1;

			for (int j = 0; j < width; j++)
			{
				single_block_creator(new_data);
				new_data.position.y -= 1;
			}

			new_data.position.y = 0;
		}
	}

#pragma endregion

#pragma region ͬ��������Ϸ���������

	void GameRenderApp::sync_trans_form_phy_obj()
	{
		for (int i = 0; i < game_object_map_.size(); i++)
		{
			// std::cout << game_object_map_.size() << std::endl;

			// �������ģ����ʽ��ʼ���������� map �� ID ����Ⱦ map �� ID �Ե��ϣ���ʾ����������������㣬��Ҫͬ������
			if (thread_state_manager_->get_physical_simulation_app_ptr()->is_phy_sim_started)
			{
				if (thread_state_manager_->get_physical_simulation_app_ptr()->get_phy_obj_map().contains(game_object_map_[i].get_id()))
				{
					// std::cout << thread_state_manager_->get_physical_simulation_app_ptr()->get_phy_obj_map()[game_object_map_[i].get_id()].rigid_body->getWorldTransform().getOrigin().getX();

					glm::vec3 position =
					{
						thread_state_manager_->get_physical_simulation_app_ptr()->get_phy_obj_map()[game_object_map_[i].get_id()].rigid_body->getWorldTransform().getOrigin().getX(),
						thread_state_manager_->get_physical_simulation_app_ptr()->get_phy_obj_map()[game_object_map_[i].get_id()].rigid_body->getWorldTransform().getOrigin().getY(),
						thread_state_manager_->get_physical_simulation_app_ptr()->get_phy_obj_map()[game_object_map_[i].get_id()].rigid_body->getWorldTransform().getOrigin().getZ(),
					};

					glm::vec3 rotation =
					{
						thread_state_manager_->get_physical_simulation_app_ptr()->get_phy_obj_map()[game_object_map_[i].get_id()].rigid_body->getWorldTransform().getRotation().getX(),
						thread_state_manager_->get_physical_simulation_app_ptr()->get_phy_obj_map()[game_object_map_[i].get_id()].rigid_body->getWorldTransform().getRotation().getY(),
						thread_state_manager_->get_physical_simulation_app_ptr()->get_phy_obj_map()[game_object_map_[i].get_id()].rigid_body->getWorldTransform().getRotation().getZ(),
					};

					// scale ֵ����
					game_object_map_[i].transform_.translation = position;
					game_object_map_[i].transform_.rotation = rotation;
				}
			}
		}
	}

#pragma endregion

#pragma region �����ú���ʵ��

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

	void GameRenderApp::test_load_falling_cube(BlockGenerateData cube_data)
	{
		std::shared_ptr<GameModel> stone_model = GameModel::create_model_from_file(game_device_, model_file_path_);
		BaseGameObject stone_obj = BaseGameObject::create_game_object(false);

		stone_obj.model_ = stone_model;
		stone_obj.transform_.translation = cube_data.position;
		stone_obj.transform_.rotation = cube_data.rotation;
		stone_obj.transform_.scale = cube_data.scale;

		// ����ĳߴ�պ�����Ⱦ��һ��
		btVector3 obj_size{ cube_data.scale.x / 2, cube_data.scale.y / 2, cube_data.scale.z / 2 };
		btVector3 obj_origin{ cube_data.position.x, cube_data.position.y, cube_data.position.z };
		float obj_mass = cube_data.mass;

		// std::cout << obj_size.x() << ", " << obj_size.y() << ", " << obj_size.z() << std::endl;
		// std::cout << stone_obj.transform_.scale.x << ", " << stone_obj.transform_.scale.y << ", " << stone_obj.transform_.scale.z << std::endl;

		// ���ɷ�����������
		PhysicsObjectCreateData obj_data = // ��֪����ʲô�ʹ�����
		{
			obj_size,
			obj_origin,
			{},
			obj_mass,
			{},
			true
		};

		// ��Ⱦ���ݷ��� game object map
		game_object_map_.emplace(stone_obj.get_id(), std::move(stone_obj));
		thread_state_manager_->get_physical_simulation_app_ptr()->create_single_physics_block(stone_obj.get_id(), obj_data);
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
		// Ӳ����ƹ�
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
