
#pragma once

#include "../render_system_include.h"

// Bullet Physics
#include "btBulletDynamicsCommon.h"

// std
#include <iostream>
#include <vector>
#include <string>
#include <memory>

namespace vulkancraft
{
	// 单个方块的生成数据
	struct BlockGenerateData
	{
		glm::vec3 position = { 0.0f, 0.0f, 0.0f };
		glm::vec3 rotation = { 0.0f, 0.0f, 0.0f }; // 这个是弧度制
		glm::vec3 scale = { 1.0f, 1.0f, 1.0f };

		// 无论方块是不是静态的，都要传递给 Bullet 物理引擎，静止不动的方块也有碰撞
		float mass = 0.0f; // 物体的质量（大于 0 表示能受到重力影响）
	};

	//// 世界生成器
	//class TerrainGeneration
	//{
	//public:
	//	TerrainGeneration();
	//	~TerrainGeneration();

	//	// 安全性操作
	//	TerrainGeneration(const TerrainGeneration&) = delete;
	//	TerrainGeneration& operator = (const TerrainGeneration&) = delete;

	//	// 静态方块生成器
	//	void single_block_creator(BlockGenerateData block_data);
	//	void create_plane(int length, int width); // 直接创建一个平面
	//	void create_wall(int height, int width); // 创建一面墙

	//private:

	//	// 模型文件路径
	//	std::string model_file_path_ = "models/block.obj";
	//};
}
