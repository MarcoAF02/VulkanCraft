
#include "game_window.h"

// std
#include <stdexcept>

namespace vulkancraft
{
	GameWindow::GameWindow(int width, int height, std::string window_name) : width_{ width }, height_{ height }, window_name_{ window_name }
	{
		init_window();
	}

	GameWindow::~GameWindow()
	{
		glfwDestroyWindow(glfw_window_);
		glfwTerminate();
	}

	void GameWindow::init_window()
	{
		glfwInit();
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

		glfw_window_ = glfwCreateWindow(width_, height_, window_name_.c_str(), nullptr, nullptr);
		glfwSetWindowUserPointer(glfw_window_, this);
		glfwSetFramebufferSizeCallback(glfw_window_, frame_buffer_resize_callback);
	}

	void GameWindow::create_vulkan_window(VkInstance instance, VkSurfaceKHR* surface)
	{
		if (glfwCreateWindowSurface(instance, glfw_window_, nullptr, surface) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to craete window surface");
		}
	}

	void GameWindow::frame_buffer_resize_callback(GLFWwindow* window, int width, int height)
	{
		GameWindow* game_window = reinterpret_cast<GameWindow*>(glfwGetWindowUserPointer(window));
		game_window -> frame_buffer_resized_ = true;
		game_window -> width_ = width;
		game_window -> height_ = height;
	}

}  // namespace vulkancraft
