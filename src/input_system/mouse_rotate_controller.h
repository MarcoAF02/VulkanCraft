
#pragma once

// stl
#include <chrono>

// glm 数学库
#include <glm/glm.hpp>

#include "../gameplay_logic/game_window.h"
#include "../gameplay_logic/base_game_object.h"
#include "../base_tools/enumeration_tools.h"

// GameWindow 中有 GLFWwindow

namespace vulkancraft
{
	class MouseRotateController
	{
	public:

		MouseRotateController();

		// 初始化鼠标控制器
		void initialize(GLFWwindow* window);

		// 静态回调函数，处理鼠标位置变化
		static void cursor_position_callback_static(GLFWwindow* window, double xpos, double ypos);

		// 鼠标位置回调函数
		void cursor_position_callback(GLFWwindow* window, double xpos, double ypos);

		// 旋转被控制的物体
		void rotate_control(GLFWwindow* window, float delta_time, BaseGameObject& game_object, RotateState kRotateState);

		// 鼠标静止时阻止视角转动
		void fix_mouse_offset();

		// DEBUG 打印鼠标移动信息
		void print_mouse_position();

	private:

		// 上一帧的鼠标动量
		double last_pos_x_ = 0.0;
		double last_pos_y_ = 0.0;

		float look_speed_ = 1.2f;

		glm::vec2 mouse_offset_ = {0, 0};
		glm::vec2 last_mouse_offset_ = {0, 0};

		GLFWwindow* glfw_window_ = nullptr;
	};

}
