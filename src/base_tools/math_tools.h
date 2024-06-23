
#pragma once

#include <glm/glm.hpp>

#define GLM_ENABLE_EXPERIMENTAL // �������ͷ�ļ���ʵ���Ե� GLM ��չ
#include <glm/gtx/compatibility.hpp> // �������Բ�ֵ�������ֵ�ȹ���

namespace vulkancraft
{
	// �÷�ͬ Unity ����� MoveTowards()
	template<typename T>
	glm::vec3 move_towards(const glm::vec3& current, const glm::vec3& target, T max_distance);
}
