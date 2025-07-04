
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

		update_render_window_content();
	}

	GameRenderApp::~GameRenderApp() { }

	void GameRenderApp::create_global_pool()
	{
		global_pool_ =
			VulkanBaseDescriptorPool::Builder(game_device_)
			.set_max_sets(GameSwapChain::static_max_frames_in_flight_)
			.add_pool_size(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, GameSwapChain::static_max_frames_in_flight_)
			.add_pool_size(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, GameSwapChain::static_max_frames_in_flight_)
			.build();
	}

	void GameRenderApp::initialize_render_system()
	{
		ubo_buffer_vector_.resize((GameSwapChain::static_max_frames_in_flight_));

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

		// TODO: 在每个 GameModel 中声明 GameTexture 类，模型加载完后直接读取 GameTexture，在这里设置每个 GameObject 贴图的采样参数。
		load_object_texture();

		global_set_layout_ =
			VulkanBaseDescriptorSetLayout::Builder(game_device_)
			.add_binding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS)
			.add_binding(1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT)
			.build();

		global_descriptor_set_vector_.resize((GameSwapChain::static_max_frames_in_flight_));

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

		// HACK: 初始化玩家，初始化玩家光标（GLFWwindow 指针）
		game_entity_manager_->get_character_controller()->init_character_controller();
		game_entity_manager_->get_character_controller()->init_mouse_rotate(game_window_.get_glfw_window());

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
			game_entity_manager_->get_character_controller()->rotate(frame_time, game_window_.get_glfw_window());
			game_entity_manager_->get_character_controller()->switch_pause_menu(game_window_.get_glfw_window());

			sync_trans_form_phy_obj(); // 同步物理世界的运算至渲染

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

				// 物体渲染顺序排序于此处，不透明物体的渲染排在半透明物体的后面。
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
		int plane_length = 20, plane_width = 20;
		int wall_height = 6, wall_width = 6;

		create_plane(plane_length, plane_width);
		create_wall(wall_height, wall_width);

		test_load_big_point_light();
		test_load_rotate_light();
	}

	void GameRenderApp::load_object_texture()
	{
		game_base_texture_ = std::make_unique<GameTexture>(game_device_, "textures/cobblestone.png");

		image_info_.sampler = game_base_texture_->get_sampler();
		image_info_.imageView = game_base_texture_->get_image_view();
		image_info_.imageLayout = game_base_texture_->get_image_layout();
	}

#pragma region 游戏地形生成器

	void GameRenderApp::single_block_creator(BlockGenerateData block_data)
	{
		std::shared_ptr<GameModel> stone_model = GameModel::create_model_from_file(game_device_, model_file_path_);
		BaseGameObject stone_obj = BaseGameObject::create_game_object(false);

		stone_obj.model_ = stone_model;
		stone_obj.transform_.translation = block_data.position;
		stone_obj.transform_.rotation = block_data.rotation;
		stone_obj.transform_.scale = block_data.scale;

		// 物理的尺寸刚好是渲染的一半
		btVector3 obj_size{ block_data.scale.x / 2, block_data.scale.y / 2, block_data.scale.z / 2 };
		btVector3 obj_origin{ block_data.position.x, block_data.position.y, block_data.position.z };
		float obj_mass = block_data.mass;

		// 生成方块物理数据
		PhysicsObjectCreateData obj_data = // 不知道传什么就打花括号
		{
			obj_size,
			obj_origin,
			{},
			obj_mass,
			{}
		};

		// 渲染数据放入 game object map
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

		for (int j = 0; j < width; j++) // 单独处理第一列
		{
			single_block_creator(new_data);
			new_data.position.z += 1;
		}

		new_data.position.z = 0;

		for (int i = 0; i < length; i++) // 对于 plane 的边长
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

		for (int j = 0; j < width; j++) // 单独处理第一列
		{
			single_block_creator(new_data);
			new_data.position.y -= 1;
		}

		new_data.position.y = 0;

		for (int i = 0; i < height; i++) // 对于 plane 的边长
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

#pragma region 同步物理游戏对象的数据

	void GameRenderApp::sync_trans_form_phy_obj()
	{
		for (int i = 0; i < game_object_map_.size(); i++)
		{
			// std::cout << game_object_map_.size() << std::endl;

			// 如果物理模拟正式开始，并且物理 map 的 ID 和渲染 map 的 ID 对得上，表示该物体纳入物理计算，需要同步数据
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

					// scale 值不变
					game_object_map_[i].transform_.translation = position;
					game_object_map_[i].transform_.rotation = rotation;
				}
			}
		}
	}

#pragma endregion

#pragma region 测试用函数实现

	void GameRenderApp::test_load_viking_room_texture()
	{
		game_base_texture_ = std::make_unique<GameTexture>(game_device_, "textures/viking_room.png");

		image_info_.sampler = game_base_texture_->get_sampler();
		image_info_.imageView = game_base_texture_->get_image_view();
		image_info_.imageLayout = game_base_texture_->get_image_layout();
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

		// 物理的尺寸刚好是渲染的一半
		btVector3 obj_size{ cube_data.scale.x / 2, cube_data.scale.y / 2, cube_data.scale.z / 2 };
		btVector3 obj_origin{ cube_data.position.x, cube_data.position.y, cube_data.position.z };
		float obj_mass = cube_data.mass;

		// std::cout << obj_size.x() << ", " << obj_size.y() << ", " << obj_size.z() << std::endl;
		// std::cout << stone_obj.transform_.scale.x << ", " << stone_obj.transform_.scale.y << ", " << stone_obj.transform_.scale.z << std::endl;

		// 生成方块物理数据
		PhysicsObjectCreateData obj_data = // 不知道传什么就打花括号
		{
			obj_size,
			obj_origin,
			{},
			obj_mass,
			{},
			true
		};

		// 渲染数据放入 game object map
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

			point_light.transform_.translation = glm::vec3(rotate_light * glm::vec4(-1.f, -3.f, -1.f, 1.f));
			game_object_map_.emplace(point_light.get_id(), std::move(point_light));
		}
	}

#pragma endregion

}
