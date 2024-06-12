
#include "game_render_app.h"

namespace vulkancraft
{
	GameRender::GameRender()
	{
		create_global_pool();
		initialize_render_system();
	}

	GameRender::~GameRender() { }

	void GameRender::create_global_pool()
	{
		global_pool_ =
			VulkanBaseDescriptorPool::Builder(game_device_)
			.set_max_sets(GameSwapChain::kMaxFramesInFlight)
			.add_pool_size(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, GameSwapChain::kMaxFramesInFlight)
			.add_pool_size(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, GameSwapChain::kMaxFramesInFlight)
			.build();
	}

	void GameRender::initialize_render_system()
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

		viewer_object_ = BaseGameObject::create_game_object();
		viewer_object_.transform_.translation.z = -2.5f;
	}

	void GameRender::update_render_window_content()
	{
		load_game_object();
		std::chrono::steady_clock::time_point current_time = std::chrono::high_resolution_clock::now();

		while (!game_window_.should_close())
		{
			glfwPollEvents();

			std::chrono::steady_clock::time_point new_time = std::chrono::high_resolution_clock::now();
			float frame_time = std::chrono::duration<float, std::chrono::seconds::period>(new_time - current_time).count();
			current_time = new_time;

			camera_controller_.move_in_plane_xz(game_window_.get_glfw_window(), frame_time, viewer_object_);
			viewer_camera_.set_view_yxz(viewer_object_.transform_.translation, viewer_object_.transform_.rotation);

			float aspect = game_renderer_.get_aspect_ratio();
			viewer_camera_.set_perspective_projection(glm::radians(50.f), aspect, 0.1f, 100.f);

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

				point_light_system_ -> update(frame_info, ubo);
				ubo_buffer_vector_[frame_index]->write_to_buffer(&ubo);
				ubo_buffer_vector_[frame_index]->flush();

				// 走完 render pass 的生命周期
				game_renderer_.begin_swap_chain_render_pass(command_buffer);

				// order here matters
				simple_render_system_ -> render_game_objects(frame_info);
				point_light_system_ -> render(frame_info);

				game_renderer_.end_swap_chain_render_pass(command_buffer);
				game_renderer_.end_frame();
			}
		}

		vkDeviceWaitIdle(game_device_.get_vulkan_device());
	}

	void GameRender::load_game_object()
	{
		std::shared_ptr<GameModel> stone_model = GameModel::create_model_from_file(game_device_, "models/block.obj");
		auto stone_obj = BaseGameObject::create_game_object();

		stone_obj.model_ = stone_model;
		stone_obj.transform_.translation = {0.0f, 0.6f, 0.0f};
		stone_obj.transform_.rotation = {0.0f, 0.0f, 0.0f};
		stone_obj.transform_.scale = {1.0f, 1.0f, 1.0f};

		game_object_map_.emplace(stone_obj.get_id(), std::move(stone_obj));

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

	void GameRender::load_object_texture()
	{
		game_base_texture_ = std::make_unique<GameTexture>(game_device_, "textures/cobblestone.png");

		image_info_.sampler = game_base_texture_->get_sampler();
		image_info_.image_view = game_base_texture_->get_image_view();
		image_info_.image_layout = game_base_texture_->get_image_layout();
	}

#pragma region 测试用函数实现

	void GameRender::test_load_viking_room_texture()
	{
		game_base_texture_ = std::make_unique<GameTexture>(game_device_, "textures/viking_room.png");

		image_info_.sampler = game_base_texture_->get_sampler();
		image_info_.image_view = game_base_texture_->get_image_view();
		image_info_.image_layout = game_base_texture_->get_image_layout();
	}

	void GameRender::test_load_viking_room()
	{
		std::shared_ptr<GameModel> lve_model = GameModel::create_model_from_file(game_device_, "models/viking_room.obj");
		auto viking_room = BaseGameObject::create_game_object();
		viking_room.model_ = lve_model;
		viking_room.transform_.translation = { 0.0f, 0.6f, 0.0f };
		viking_room.transform_.rotation = { 3.1415926f / 2.0f, 3.1415926f, 0.0f };
		viking_room.transform_.scale = { 1.0f, 1.0f, 1.0f };
		game_object_map_.emplace(viking_room.get_id(), std::move(viking_room));
	}

#pragma endregion


}
