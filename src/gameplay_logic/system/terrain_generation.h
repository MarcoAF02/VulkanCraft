
#pragma once

#include "../render_system_include.h"

// std
#include <iostream>
#include <vector>
#include <string>
#include <memory>

namespace vulkancraft
{
	// �����������������
	struct BlockGenerateData
	{
		glm::vec3 position = { 0.0f, 0.0f, 0.0f };
		glm::vec3 rotation = { 0.0f, 0.0f, 0.0f }; // ����ǻ�����
		glm::vec3 scale = { 1.0f, 1.0f, 1.0f };

		// ���۷����ǲ��Ǿ�̬�ģ���Ҫ���ݸ� Bullet �������棬��ֹ�����ķ���Ҳ����ײ
		bool is_static = true; // �����Ƿ��Ǿ�̬��
	};

	// ����������
	class TerrainGeneration
	{
	public:
		TerrainGeneration(GameDevice& game_device, RenderAppObjMap& game_object_map);
		~TerrainGeneration();

		// ��ȫ�Բ���
		TerrainGeneration (const TerrainGeneration&) = delete;
		TerrainGeneration& operator = (const TerrainGeneration&) = delete;

		// ��̬����������
		void single_block_creator(BlockGenerateData block_data);
		void create_plane(int length, int width); // ֱ�Ӵ���һ��ƽ��
		void create_wall(int height, int width); // ����һ��ǽ

	private:

		// ģ���ļ�·��
		std::string model_file_path_ = "models/block.obj";

		GameDevice& game_device_;
		RenderAppObjMap& game_object_map_;
		std::shared_ptr<GameObjectManager> game_object_manager_;

#pragma region DEBUG �ú���

		// ����������̬����
		void test_dynamic_block_create(BlockGenerateData block_data);

#pragma endregion

	};
}
