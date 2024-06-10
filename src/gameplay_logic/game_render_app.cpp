
#include "game_render_app.h"

namespace vulkancraft
{
	GameRender::GameRender()
	{
		create_global_pool(); // 创建全局描述符池
		load_game_object(); // 加载游戏物体
	}

	GameRender::~GameRender() { }

	// 创建全局描述符池
	void GameRender::create_global_pool()
	{
		global_pool_ = VulkanBaseDescriptorPool::Builder(game_device_)
			.set_max_sets(GameSwapChain::kMaxFramesInFlight)
			.add_pool_size(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, GameSwapChain::kMaxFramesInFlight)
			.add_pool_size(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, GameSwapChain::kMaxFramesInFlight)
			.build();
	}

	// 加载游戏对象
	void GameRender::load_game_object()
	{
		// HACK: 这里是房间的模型
		std::shared_ptr<GameModel> lve_model = GameModel::create_model_from_file(game_device_, "models/viking_room.obj");
		vulkancraft::BaseGameObject viking_room = BaseGameObject::create_game_object();

		viking_room.model_ = lve_model;
		viking_room.transform_.translation = { 0.0f, 0.6f, 0.0f };
		viking_room.transform_.rotation = { 3.1415926f / 2.0f, 3.1415926f, 0.0f };
		viking_room.transform_.scale = { 1.0f, 1.0f, 1.0f };

		game_object_map_.emplace(viking_room.get_id(), std::move(viking_room));

		// HACK 下面是硬编码的灯光
		std::vector<glm::vec3> light_color_vector
		{
			{1.f, .1f, .1f},
			{.1f, .1f, 1.f},
			{.1f, 1.f, .1f},
			{1.f, 1.f, .1f},
			{.1f, 1.f, 1.f},
			{1.f, 1.f, 1.f}
		};

		// 让灯物体转起来
		for (int i = 0; i < light_color_vector.size(); i++)
		{
			// TODO: 灯没亮！
			vulkancraft::BaseGameObject point_light = BaseGameObject::make_point_light(20.0f);
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

	// 更新渲染主循环
	void GameRender::update_render_window_content()
	{
		std::vector<std::unique_ptr<GameBuffer>> ubo_buffer_vector_(GameSwapChain::kMaxFramesInFlight);

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

			ubo_buffer_vector_[i] -> map();
		}

		game_base_texture_ = std::make_unique<GameTexture>(game_device_, "textures/viking_room.png");

		VkDescriptorImageInfo image_info = {};
		image_info.sampler = game_base_texture_ -> get_sampler();
		image_info.image_view = game_base_texture_ -> get_image_view();
		image_info.image_layout = game_base_texture_ -> get_image_layout();

		std::unique_ptr< VulkanBaseDescriptorSetLayout> global_set_layout =
			VulkanBaseDescriptorSetLayout::Builder(game_device_)
			.add_binding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS)
			.add_binding(1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT)
			.build();

		std::vector<VkDescriptorSet> global_descriptor_set_vector(GameSwapChain::kMaxFramesInFlight);

		for (int i = 0; i < global_descriptor_set_vector.size(); i++)
		{
			auto buffer_info = ubo_buffer_vector_[i] -> descriptor_info();

			VulkanBaseDescriptorWriter(*global_set_layout, *global_pool_)
				.write_buffer(0, &buffer_info)
				.write_image(1, &image_info)
				.build(global_descriptor_set_vector[i]);
		}

		SimpleRenderSystem simple_render_system
		{
			game_device_,
			game_renderer_.get_swap_chain_render_pass(),
			global_set_layout -> get_descriptor_set_layout()
		};

		PointLightSystem point_light_system
		{
			game_device_,
			game_renderer_.get_swap_chain_render_pass(),
			global_set_layout -> get_descriptor_set_layout()
		};

		GameBaseCamera camera{};

		auto viewer_object = BaseGameObject::create_game_object();
		viewer_object.transform_.translation.z = -2.5f;
		KeyboardMovementController camera_controller{};

		auto current_time = std::chrono::high_resolution_clock::now();

		while (true)
		{
			auto viewer_object = BaseGameObject::create_game_object();
			viewer_object.transform_.translation.z = -2.5f;
			KeyboardMovementController camera_controller{};

			// HACK: 这里处理 SDL 窗口的各种事件
			if (SDL_PollEvent(&game_window_.sdl_event_))
			{
				switch (game_window_.sdl_event_.type)
				{
				case SDL_QUIT: return;

				case SDL_WINDOWEVENT:

					// 窗口大小改变事件
					if (game_window_.sdl_event_.window.event == SDL_WINDOWEVENT_SIZE_CHANGED)
					{
						std::cout << "窗口大小改变" << std::endl;
						game_window_.recreate_sdl_surface();
					}

					break;
				}
			}

			// 计算流逝的时间
			std::chrono::high_resolution_clock::time_point new_time = std::chrono::high_resolution_clock::now();
			float frame_time = std::chrono::duration<float, std::chrono::seconds::period>(new_time - current_time).count();
			current_time = new_time;

			// HACK: 这么好的时间啊
			// std::cout << current_time.time_since_epoch() << std::endl;

			float aspect = game_renderer_.get_aspect_ratio();
			camera.set_perspective_projection(glm::radians(50.f), aspect, 0.1f, 100.f);

			// TODO: 键盘读取类存在 BUG
			// camera_controller.move_in_plane_xz(game_window_.get_window(), frame_time, viewer_object);
			camera.set_view_yxz(viewer_object.transform_.translation, viewer_object.transform_.rotation);

			if (auto command_buffer = game_renderer_.begin_frame())
			{
				int frame_index = game_renderer_.get_frame_index();

				FrameInfo frame_info
				{
					frame_index,
					frame_time,
					command_buffer,
					camera,
					global_descriptor_set_vector[frame_index],
					game_object_map_
				};

				// update
				GlobalUbo ubo{};
				ubo.projection = camera.get_projection();
				ubo.view = camera.get_view();
				ubo.inverse_view = camera.get_inverse_view();

				point_light_system.update(frame_info, ubo);
				ubo_buffer_vector_[frame_index] -> write_to_buffer(&ubo);
				ubo_buffer_vector_[frame_index] -> flush();

				// render
				game_renderer_.begin_swap_chain_render_pass(command_buffer);

				// order here matters
				simple_render_system.render_game_objects(frame_info);
				point_light_system.render(frame_info);

				game_renderer_.end_swap_chain_render_pass(command_buffer);
				game_renderer_.end_frame();
			}

			vkDeviceWaitIdle(game_device_.get_vulkan_device());
		}
	}

}
