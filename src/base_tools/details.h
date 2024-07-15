
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

// HACK 这个文件将大部分要用的自定义数据结构存起来

// 单个方块的生成数据
struct BlockGenerateData
{
	glm::vec3 position = { 0.0f, 0.0f, 0.0f };
	glm::vec3 rotation = { 0.0f, 0.0f, 0.0f }; // 这个是弧度制
	glm::vec3 scale = { 1.0f, 1.0f, 1.0f };

	// 无论方块是不是静态的，都要传递给 Bullet 物理引擎，静止不动的方块也有碰撞
	float mass = 0.0f; // 物体的质量（大于 0 表示能受到重力影响）
};

// 从物理线程同步回来的数据
struct PhysicsObjectTransData
{
	glm::vec3 position;
	glm::vec3 rotation;
	glm::vec3 scale;
};
