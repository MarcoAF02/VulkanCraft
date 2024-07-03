
#pragma once

#include "../render_system_include.h"

// std
#include <iostream>
#include <string>
#include <memory>

namespace vulkancraft
{
	// ����������������ݣ���̬��
	struct BlockGenerateData
	{
		glm::vec3 position = { 0.0f, 0.0f, 0.0f };
		glm::vec3 rotation = { 0.0f, 0.0f, 0.0f }; // ����ǻ�����
		glm::vec3 scale = { 1.0f, 1.0f, 1.0f };
	};

	// ����������
	class TerrainGeneration
	{
	public:
		TerrainGeneration(GameDevice& game_device, BaseGameObject::Map& game_object_map);
		~TerrainGeneration();

		// ��ȫ�Բ���
		TerrainGeneration (const TerrainGeneration&) = delete;
		TerrainGeneration& operator = (const TerrainGeneration&) = delete;

		// ��̬����������
		void single_block_creator(BlockGenerateData block_data);
		void create_plane(int length, int width); // ֱ�Ӵ���һ��ƽ��

	private:

		// ģ���ļ�·��
		std::string model_file_path_ = "models/block.obj";

		GameDevice& game_device_;
		BaseGameObject::Map& game_object_map_;
		std::shared_ptr<GameObjectManager> game_object_manager_;

	};
}
