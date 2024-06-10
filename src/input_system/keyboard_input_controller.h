
#pragma once

#include "../gameplay_logic/game_window.h"
#include "../gameplay_logic/base_game_object.h"

// 键盘输入管理类集合

namespace vulkancraft
{
	class KeyboardMovementController
	{
	public:
		struct KeyMap
		{
			// 移动
			int move_left = SDLK_a; // 左右前后上下（按顺序写的）
			int move_right = SDLK_d;
			int move_forward = SDLK_w;
			int move_backward = SDLK_s;

			int move_up = SDLK_e;
			int move_down = SDLK_q;

			// 旋转
			int look_left = SDLK_LEFT; // 左右上下（按顺序写的）
			int look_right = SDLK_RIGHT;
			int look_up = SDLK_UP;
			int look_down = SDLK_DOWN;
		};

		KeyMap keys {};

		float move_speed_ = 1.6f;
		float rotate_speed_ = 1.2f;

		const Uint8* kKeyboardState_ = nullptr; // SDL 按键输入读取
		void move_in_plane_xz(SDL_Window* sdl_window, float delta_time, BaseGameObject& game_object);
	};
}
