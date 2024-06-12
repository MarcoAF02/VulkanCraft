
#pragma once

#include <glm/glm.hpp>

namespace vulkancraft
{
	// ��ײ�е��ƶ����
	struct ColliderTransformComponent
	{
		float collider_default_size = 1.0f; // ��ײ��Ĭ�ϴ�С

		glm::vec3 translation{};
		glm::vec3 scale{ 1.f, 1.f, 1.f };
		glm::vec3 rotation{};

		// Matrix corrsponds to Translate * Ry * Rx * Rz * Scale
		// Rotations correspond to Tait-bryan angles of Y(1), X(2), Z(3)
		// https://en.wikipedia.org/wiki/Euler_angles#Rotation_matrix
		glm::mat4 mat4();

		glm::mat3 normal_matrix();
	};

	class AABBCollider
	{
	public:

	};
}
