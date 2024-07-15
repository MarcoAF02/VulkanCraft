
#pragma once

// libs
#ifndef GLM_FORCE_RADIANS
#define GLM_FORCE_RADIANS
#endif

#ifndef GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#endif

#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

// HACK ����ļ����󲿷�Ҫ�õ��Զ������ݽṹ������

// �����������������
struct BlockGenerateData
{
	glm::vec3 position = { 0.0f, 0.0f, 0.0f };
	glm::vec3 rotation = { 0.0f, 0.0f, 0.0f }; // ����ǻ�����
	glm::vec3 scale = { 1.0f, 1.0f, 1.0f };

	// ���۷����ǲ��Ǿ�̬�ģ���Ҫ���ݸ� Bullet �������棬��ֹ�����ķ���Ҳ����ײ
	float mass = 0.0f; // ��������������� 0 ��ʾ���ܵ�����Ӱ�죩
};

// �������߳�ͬ������������
struct PhysicsObjectTransData
{
	glm::vec3 position;
	glm::vec3 rotation;
	glm::vec3 scale;
};
