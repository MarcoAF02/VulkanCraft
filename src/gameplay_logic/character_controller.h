
#pragma once

#include "../game_physics_system/physics_aabb.h"
#include "../input_system/keyboard_input_controller.h"

// stl
#include <iostream>

// glm 数学库
#include <glm/glm.hpp>

class vulkancraft::AABBCollider; // AABB 碰撞盒
class vulkancraft::KeyboardMovementController; // 键盘控制器

namespace vulkancraft
{
	class CharacterController
	{
	public:

	// TODO: 构造函数，禁止复制的安全性操作
	// TODO: 集键盘控制器，鼠标控制器，BaseGameObject，AABBCollider，RigidbodySystem 为一体

	// Unity 的逻辑：Move 函数的 dir 必须是世界坐标空间单位 (vector3)，传进来的 inuput vector (vector3) 是物体空间单位，转换过去才能用

	private:

	// HACK: 人类的高度为 1.8 宽度为 0.6

	};
}
