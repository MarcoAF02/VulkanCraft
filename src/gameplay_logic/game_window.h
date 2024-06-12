
#pragma once

#define GLFW_INCLUDE_VULKAN // 创建 Vulkan 上下文
#include <GLFW/glfw3.h>

#include <iostream>
#include <string>

namespace vulkancraft
{
	class GameWindow
	{
	public:
		GameWindow(int width, int height, std::string window_name);
		~GameWindow();

		GameWindow(const GameWindow&) = delete;
		GameWindow& operator=(const GameWindow&) = delete;

		bool should_close() { return glfwWindowShouldClose(glfw_window_); }

		VkExtent2D get_extent() { return { static_cast<uint32_t>(width_), static_cast<uint32_t>(height_) }; }
		bool was_window_resized() { return frame_buffer_resized_; }
		void reset_window_resized_flag() { frame_buffer_resized_ = false; }
		GLFWwindow* get_glfw_window() const { return glfw_window_; }

		void create_vulkan_window(VkInstance instance, VkSurfaceKHR* surface);

	private:
		static void frame_buffer_resize_callback(GLFWwindow* window, int width, int height);
		void init_window();

		int width_;
		int height_;
		bool frame_buffer_resized_ = false;

		std::string window_name_;
		GLFWwindow* glfw_window_;
	};

}  // namespace vulkancraft
