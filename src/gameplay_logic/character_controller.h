
#pragma once

#include "../game_physics_system/physics_aabb.h"
#include "../input_system/keyboard_input_controller.h"

// stl
#include <iostream>

// glm ��ѧ��
#include <glm/glm.hpp>

class vulkancraft::AABBCollider; // AABB ��ײ��
class vulkancraft::KeyboardMovementController; // ���̿�����

namespace vulkancraft
{
	class CharacterController
	{
	public:

	// TODO: ���캯������ֹ���Ƶİ�ȫ�Բ���
	// TODO: �����̿�����������������BaseGameObject��AABBCollider��RigidbodySystem Ϊһ��

	// Unity ���߼���Move ������ dir ��������������ռ䵥λ (vector3)���������� inuput vector (vector3) ������ռ䵥λ��ת����ȥ������

	private:

	// HACK: ����ĸ߶�Ϊ 1.8 ���Ϊ 0.6

	};
}
