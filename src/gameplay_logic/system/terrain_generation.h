
#pragma once

#include "../render_system_include.h"

// std
#include <iostream>
#include <string>
#include <memory>

namespace vulkancraft
{
	// 单个方块的生成数据（静态）
	struct BlockGenerateData
	{
		glm::vec3 position = { 0.0f, 0.0f, 0.0f };
		glm::vec3 rotation = { 0.0f, 0.0f, 0.0f }; // 这个是弧度制
		glm::vec3 scale = { 1.0f, 1.0f, 1.0f };
	};

	// 世界生成器
	class TerrainGeneration
	{
	public:
		TerrainGeneration(GameDevice& game_device, BaseGameObject::Map& game_object_map);
		~TerrainGeneration();

		// 安全性操作
		TerrainGeneration (const TerrainGeneration&) = delete;
		TerrainGeneration& operator = (const TerrainGeneration&) = delete;

		// 静态方块生成器
		void single_block_creator(BlockGenerateData block_data);
		void create_plane(int length, int width); // 直接创建一个平面

	private:

		// 模型文件路径
		std::string model_file_path_ = "models/block.obj";

		GameDevice& game_device_;
		BaseGameObject::Map& game_object_map_;
		std::shared_ptr<GameObjectManager> game_object_manager_;

	};
}
