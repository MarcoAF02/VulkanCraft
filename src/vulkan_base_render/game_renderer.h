
#pragma once

#include "../vulkan_base/game_device.h"
#include "../vulkan_base/game_swap_chain.h"
#include "../gameplay_logic/game_window.h"

// std
#include <cassert>
#include <memory>
#include <vector>

namespace vulkancraft
{
	class GameRenderer
	{
	public:
		GameRenderer(GameWindow& window, GameDevice& game_device);
		~GameRenderer();

		GameRenderer(const GameRenderer&) = delete;
		GameRenderer& operator = (const GameRenderer&) = delete;

		VkRenderPass get_swap_chain_render_pass() const { return game_swap_chain_ -> get_render_pass(); }
		float get_aspect_ratio() const { return game_swap_chain_ -> extent_aspect_ratio(); }
		bool is_frame_in_progress() const { return is_frame_started_; }

		VkCommandBuffer get_current_command_buffer() const
		{
			assert(is_frame_started_ && "Cannot get command buffer when frame not in progress");
			return command_buffer_vector_[current_frame_index_];
		}

		int get_frame_index() const
		{
			assert(is_frame_started_ && "Cannot get frame index when frame not in progress");
			return current_frame_index_;
		}

		VkCommandBuffer begin_frame();
		void end_frame();
		void begin_swap_chain_render_pass(VkCommandBuffer command_buffer);
		void end_swap_chain_render_pass(VkCommandBuffer command_buffer);

	private:
		void create_command_buffers();
		void free_command_buffers();
		void recreate_swap_chain();

		GameWindow& game_window_;
		GameDevice& game_device_;
		std::unique_ptr<GameSwapChain> game_swap_chain_;
		std::vector<VkCommandBuffer> command_buffer_vector_;

		uint32_t current_image_index_ = 0;
		int current_frame_index_{ 0 };
		bool is_frame_started_{ false };
	};

}  // namespace vulkancraft
