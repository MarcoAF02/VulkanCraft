
#pragma once

#include "game_device.h"

// vulkan headers
#include <vulkan/vulkan.h>

// std lib headers
#include <memory>
#include <string>
#include <vector>

namespace vulkancraft
{
	class GameSwapChain
	{
	public:

		static int static_max_frames_in_flight_;

		GameSwapChain(GameDevice& game_device_ref, VkExtent2D window_extent);
		GameSwapChain(GameDevice& game_device_ref, VkExtent2D window_extent, std::shared_ptr<GameSwapChain> previous);

		~GameSwapChain();

		GameSwapChain(const GameSwapChain&) = delete;
		GameSwapChain& operator=(const GameSwapChain&) = delete;

		VkFramebuffer get_frame_buffer(int index) { return swap_chain_frame_buffer_vector_[index]; }
		VkRenderPass get_render_pass() { return render_pass_; }
		VkImageView get_image_view(int index) { return swap_chain_image_view_vector_[index]; }
		size_t get_image_count() { return swap_chain_image_vector_.size(); }
		VkFormat get_swap_chain_image_format() { return swap_chain_image_format_; }
		VkExtent2D get_swap_chain_extent() { return swap_chain_extent_; }
		uint32_t get_width() { return swap_chain_extent_.width; }
		uint32_t get_height() { return swap_chain_extent_.height; }

		float extent_aspect_ratio()
		{
			return static_cast<float>(swap_chain_extent_.width) / static_cast<float>(swap_chain_extent_.height);
		}

		VkFormat find_depth_format();
		VkResult acquire_next_image(uint32_t* image_index);
		VkResult submit_command_buffers(const VkCommandBuffer* buffers, uint32_t* image_index);

		bool compare_swap_formats(const GameSwapChain& swap_chain) const
		{
			return swap_chain.swap_chain_depth_format_ == swap_chain_depth_format_ && swap_chain.swap_chain_image_format_ == swap_chain_image_format_;
		}

	private:

		void init();
		void create_swap_chain();
		void create_image_views();
		void create_depth_resources();
		void create_render_pass();
		void create_frame_buffers();
		void create_sync_objects();

		// Helper functions
		VkSurfaceFormatKHR choose_swap_surface_format(const std::vector<VkSurfaceFormatKHR>& available_formats);
		VkPresentModeKHR choose_swap_present_mode(const std::vector<VkPresentModeKHR>& available_present_modes);
		VkExtent2D choose_swap_extent(const VkSurfaceCapabilitiesKHR& capabilities);

		VkFormat swap_chain_image_format_;
		VkFormat swap_chain_depth_format_;
		VkExtent2D swap_chain_extent_;

		std::vector<VkFramebuffer> swap_chain_frame_buffer_vector_;
		VkRenderPass render_pass_;

		std::vector<VkImage> depth_image_vector_;
		std::vector<VkDeviceMemory> depth_image_memory_vector_;
		std::vector<VkImageView> depth_image_view_vector_;
		std::vector<VkImage> swap_chain_image_vector_;
		std::vector<VkImageView> swap_chain_image_view_vector_;

		GameDevice& game_device_;
		VkExtent2D window_extent_;

		VkSwapchainKHR swap_chain_;
		std::shared_ptr<GameSwapChain> old_swap_chain_;

		std::vector<VkSemaphore> image_available_semaphore_vector_;
		std::vector<VkSemaphore> render_finished_semaphore_vector_;
		std::vector<VkFence> in_flight_fence_vector_;
		std::vector<VkFence> images_in_flight_vector_;
		size_t current_frame_ = 0;
	};

}  // namespace vulkancraft
