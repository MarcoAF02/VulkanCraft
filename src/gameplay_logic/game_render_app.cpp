
#include "game_render_app.h"

namespace vulkancraft
{
	GameRender::GameRender()
	{
		global_pool_ =
			VulkanBaseDescriptorPool::Builder(game_device_)
			.set_max_sets(GameSwapChain::kMaxFramesInFlight)
			.add_pool_size(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, GameSwapChain::kMaxFramesInFlight)
			.add_pool_size(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, GameSwapChain::kMaxFramesInFlight)
			.build();

		load_game_object();
	}

	GameRender::~GameRender() { }

	void GameRender::update_render_window_content()
	{
		std::vector<std::unique_ptr<GameBuffer>> ubo_buffer_vector(GameSwapChain::kMaxFramesInFlight);

		for (int i = 0; i < ubo_buffer_vector.size(); i++)
		{
			ubo_buffer_vector[i] = std::make_unique<GameBuffer>
				(
					game_device_,
					sizeof(GlobalUbo),
					1,
					VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
					VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
				);

			ubo_buffer_vector[i]->map();
		}

		game_base_texture_ = std::make_unique<GameTexture>(game_device_, "textures/viking_room.png");

		VkDescriptorImageInfo image_info = {};
		image_info.sampler = game_base_texture_->get_sampler();
		image_info.image_view = game_base_texture_->get_image_view();
		image_info.image_layout = game_base_texture_->get_image_layout();

		std::unique_ptr<VulkanBaseDescriptorSetLayout> global_set_layout =
			VulkanBaseDescriptorSetLayout::Builder(game_device_)
			.add_binding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS)
			.add_binding(1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT)
			.build();

		std::vector<VkDescriptorSet> global_descriptor_set_vector(GameSwapChain::kMaxFramesInFlight);

		for (int i = 0; i < global_descriptor_set_vector.size(); i++)
		{
			VkDescriptorBufferInfo buffer_info = ubo_buffer_vector[i]->descriptor_info();

			VulkanBaseDescriptorWriter(*global_set_layout, *global_pool_)
				.write_buffer(0, &buffer_info)
				.write_image(1, &image_info)
				.build(global_descriptor_set_vector[i]);
		}

		SimpleRenderSystem simple_render_system
		{
			game_device_,
			game_renderer_.get_swap_chain_render_pass(),
			global_set_layout->get_descriptor_set_layout()
		};

		PointLightSystem point_light_system
		{
			game_device_,
			game_renderer_.get_swap_chain_render_pass(),
			global_set_layout->get_descriptor_set_layout()
		};

		GameBaseCamera camera{};

		BaseGameObject viewer_object = BaseGameObject::create_game_object();
		viewer_object.transform_.translation.z = -2.5f;
		KeyboardMovementController camera_controller{};

		auto current_time = std::chrono::high_resolution_clock::now();

		while (!game_window_.should_close())
		{
			glfwPollEvents();

			auto new_time = std::chrono::high_resolution_clock::now();
			float frame_time = std::chrono::duration<float, std::chrono::seconds::period>(new_time - current_time).count();
			current_time = new_time;

			camera_controller.move_in_plane_xz(game_window_.get_glfw_window(), frame_time, viewer_object);
			camera.set_view_yxz(viewer_object.transform_.translation, viewer_object.transform_.rotation);

			float aspect = game_renderer_.get_aspect_ratio();
			camera.set_perspective_projection(glm::radians(50.f), aspect, 0.1f, 100.f);

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

				// std::cout << sizeof(ubo) << std::endl;

				point_light_system.update(frame_info, ubo);
				ubo_buffer_vector[frame_index] -> write_to_buffer(&ubo);
				ubo_buffer_vector[frame_index] -> flush();

				// render
				game_renderer_.begin_swap_chain_render_pass(command_buffer);

				// order here matters
				simple_render_system.render_game_objects(frame_info);
				point_light_system.render(frame_info);

				game_renderer_.end_swap_chain_render_pass(command_buffer);
				game_renderer_.end_frame();
			}
		}

		vkDeviceWaitIdle(game_device_.get_vulkan_device());
	}

	void GameRender::load_game_object()
	{
		std::shared_ptr<GameModel> lve_model = GameModel::create_model_from_file(game_device_, "models/viking_room.obj");
		auto viking_room = BaseGameObject::create_game_object();
		viking_room.model_ = lve_model;
		viking_room.transform_.translation = { 0.0f, 0.6f, 0.0f };
		viking_room.transform_.rotation = { 3.1415926f / 2.0f, 3.1415926f, 0.0f };
		viking_room.transform_.scale = { 1.0f, 1.0f, 1.0f };
		game_object_map_.emplace(viking_room.get_id(), std::move(viking_room));

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

}
