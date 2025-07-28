
#pragma once

#include "../gameplay_logic/game_window.h"

// std lib headers
#include <string>
#include <vector>

// vulkan
#include <vulkan/vulkan.h>

namespace vulkancraft
{
	struct SwapChainSupportDetails
	{
		VkSurfaceCapabilitiesKHR capabilities;
		std::vector<VkSurfaceFormatKHR> formats_vector;
		std::vector<VkPresentModeKHR> present_mode_vector;
	};

	struct QueueFamilyIndices
	{
		uint32_t graphics_family = 0;
		uint32_t present_family = 0;
		bool graphics_family_has_value = false;
		bool present_family_has_value = false;
		bool isComplete() const { return graphics_family_has_value && present_family_has_value; }
	};

	class GameDevice
	{
	public:
#ifdef NDEBUG
		const bool enable_validation_layers_ = false;
#else
		const bool enable_validation_layers_ = true;
#endif

		GameDevice(GameWindow& window);
		~GameDevice();

		// Not copyable or movable
		GameDevice(const GameDevice&) = delete;
		GameDevice& operator=(const GameDevice&) = delete;
		GameDevice(GameDevice&&) = delete;
		GameDevice& operator=(GameDevice&&) = delete;

		VkCommandPool get_command_pool() const { return command_pool_; }

		// 得到 Vulkan 单例
		VkInstance get_vulkan_instance() const { return instance_; }

		// 得到 Vulkan 逻辑设备
		VkDevice get_vulkan_device() const { return vulkan_device_; }

		// 得到 Vulkan 物理设备
		VkPhysicalDevice get_physical_device() const { return physical_device_; }

		// 得到 Graphics Queue
		VkQueue get_graphics_queue() const { return graphics_queue_; }

		VkSurfaceKHR surface() const { return surface_; }
		VkQueue graphics_queue() const { return graphics_queue_; }
		VkQueue present_queue() const { return present_queue_; }

		SwapChainSupportDetails get_swap_chain_support() { return query_swap_chain_support(physical_device_); }
		uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
		QueueFamilyIndices find_physical_queue_families() { return find_queue_families(physical_device_); }
		VkFormat find_supported_format(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features);

		// Buffer Helper Functions
		void create_buffer
		(
			VkDeviceSize size,
			VkBufferUsageFlags usage,
			VkMemoryPropertyFlags properties,
			VkBuffer& buffer,
			VkDeviceMemory& buffer_memory
		);

		VkCommandBuffer begin_single_time_commands();
		void end_single_time_commands(VkCommandBuffer command_buffer);
		void copy_buffer(VkBuffer src_buffer, VkBuffer dst_buffer, VkDeviceSize size);

		void copy_buffer_to_image
		(
			VkBuffer buffer,
			VkImage image,
			uint32_t width,
			uint32_t height,
			uint32_t layer_count
		);

		void create_image_with_info
		(
			const VkImageCreateInfo& imageInfo,
			VkMemoryPropertyFlags properties,
			VkImage& image,
			VkDeviceMemory& image_memory
		);

		VkPhysicalDeviceProperties properties_;

	private:
		void create_vulkan_instance();
		void setup_debug_messenger();
		void create_surface();
		void pick_physical_device();
		void create_logical_device();
		void create_command_pool();

		// helper functions
		bool is_device_suitable(VkPhysicalDevice physical_device);
		std::vector<const char*> get_required_extension_vector();
		bool check_validation_layer_support();
		QueueFamilyIndices find_queue_families(VkPhysicalDevice physical_device);
		void populate_debug_messenger_create_info(VkDebugUtilsMessengerCreateInfoEXT& create_info);
		void has_required_instance_extensions();
		bool check_device_extension_support(VkPhysicalDevice physical_device);
		SwapChainSupportDetails query_swap_chain_support(VkPhysicalDevice physical_device);

		VkInstance instance_;
		VkDebugUtilsMessengerEXT debug_messenger_;
		VkPhysicalDevice physical_device_ = VK_NULL_HANDLE;
		GameWindow& game_window_;
		VkCommandPool command_pool_;

		VkDevice vulkan_device_;
		VkSurfaceKHR surface_;
		VkQueue graphics_queue_;
		VkQueue present_queue_;

		const std::vector<const char*> validation_layer_vector_ = { "VK_LAYER_KHRONOS_validation" };
		const std::vector<const char*> device_extension_vector_ = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };
	};

}  // namespace vulkancraft
