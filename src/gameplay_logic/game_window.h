
#pragma once

#define GLFW_INCLUDE_VULKAN // 创建 Vulkan 上下文
#include <GLFW/glfw3.h>

// std
#include <iostream>
#include <string>

namespace vulkancraft
{
	// TODO: 未来的更新：把 GLFWwindow* 和所附加的原子指针包装为一个类。然后用智能指针托住它（相当于自己实现了智能指针）
	extern std::atomic<GLFWwindow*> global_glfw_window_ptr; // 声明一个全局原子指针

	class GameWindow
	{
	public:
		GameWindow(int width, int height, std::string window_name);
		~GameWindow();

		GameWindow(const GameWindow&) = delete;
		GameWindow& operator=(const GameWindow&) = delete;

		bool should_close() { return glfwWindowShouldClose(glfw_window_); }

		VkExtent2D get_extent();
		bool was_window_resized() { return frame_buffer_resized_; }
		void reset_window_resized_flag() { frame_buffer_resized_ = false; }
		GLFWwindow* get_glfw_window() const { return glfw_window_; }

		void create_vulkan_window(VkInstance instance, VkSurfaceKHR* surface);

	private:

		int width_ = 0;
		int height_ = 0;
		bool frame_buffer_resized_ = false;

		std::string window_name_ = nullptr;
		GLFWwindow* glfw_window_ = nullptr;

		static void frame_buffer_resize_callback(GLFWwindow* window, int width, int height);
		void init_window();
	};

}  // namespace vulkancraft
