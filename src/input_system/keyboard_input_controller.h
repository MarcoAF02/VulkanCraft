
#pragma once

#include "../gameplay_logic/game_window.h"
#include "../gameplay_logic/base_game_object.h"

// ������������༯��

namespace vulkancraft
{
	class KeyboardMovementController
	{
	public:
		struct KeyMap
		{
			// �ƶ�
			int move_left = SDLK_a; // ����ǰ�����£���˳��д�ģ�
			int move_right = SDLK_d;
			int move_forward = SDLK_w;
			int move_backward = SDLK_s;

			int move_up = SDLK_e;
			int move_down = SDLK_q;

			// ��ת
			int look_left = SDLK_LEFT; // �������£���˳��д�ģ�
			int look_right = SDLK_RIGHT;
			int look_up = SDLK_UP;
			int look_down = SDLK_DOWN;
		};

		KeyMap keys {};

		float move_speed_ = 1.6f;
		float rotate_speed_ = 1.2f;

		const Uint8* kKeyboardState_ = nullptr; // SDL ���������ȡ
		void move_in_plane_xz(SDL_Window* sdl_window, float delta_time, BaseGameObject& game_object);
	};
}
