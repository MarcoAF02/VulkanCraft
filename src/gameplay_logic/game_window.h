
#pragma once

#define GLFW_INCLUDE_VULKAN // ���� Vulkan ������
#include <GLFW/glfw3.h>

// std
#include <iostream>
#include <string>

namespace vulkancraft
{
	// TODO: δ���ĸ��£��� GLFWwindow* �������ӵ�ԭ��ָ���װΪһ���ࡣȻ��������ָ����ס�����൱���Լ�ʵ��������ָ�룩
	extern std::atomic<GLFWwindow*> global_glfw_window_ptr; // ����һ��ȫ��ԭ��ָ��

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
