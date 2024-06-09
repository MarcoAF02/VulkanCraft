
#pragma once

#include "../vulkan_base/game_device.h"
#include <vulkan/vulkan_core.h>

// std
#include <iostream>
#include <string>
#include <fstream>

namespace vulkancraft
{
	class GameTexture
	{
	public:
		GameTexture(GameDevice& game_device, const std::string& file_path);
		~GameTexture();

		GameTexture(const GameTexture&) = delete;
		GameTexture& operator=(const GameTexture&) = delete;
		GameTexture(GameTexture&&) = delete;
		GameTexture& operator=(GameTexture&&) = delete;

		VkSampler get_sampler() { return sampler_; }
		VkImageView get_image_view() { return image_view_; }
		VkImageLayout get_image_layout() { return image_layout_; }

	private:
		void transition_image_layout(VkImageLayout old_layout, VkImageLayout new_layout);
		void generate_mipmaps();

		int width_ = 0;
		int height_ = 0;
		int mip_levels_ = 0;

		GameDevice& game_device_;
		VkImage image_;
		VkDeviceMemory image_memory_;
		VkImageView image_view_;
		VkSampler sampler_;
		VkFormat image_format_;
		VkImageLayout image_layout_;
	};

} // namespace vulkancraft
