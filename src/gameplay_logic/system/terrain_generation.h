
#pragma once

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include "../physical_simulation_app.h"
#include "../base_game_object.h"

// Bullet Physics
#include "btBulletDynamicsCommon.h"

// std
#include <iostream>
#include <vector>
#include <string>
#include <memory>

class PhysicalSimulationApp;
class GameEntityManager;
class BlockGenerateData;
class BaseGameObject;
class GameDevice;

namespace vulkancraft
{
	using id_t = unsigned _int64;

	// �����������������
	struct BlockGenerateData
	{
		glm::vec3 position = { 0.0f, 0.0f, 0.0f };
		glm::vec3 rotation = { 0.0f, 0.0f, 0.0f }; // ����ǻ�����
		glm::vec3 scale = { 1.0f, 1.0f, 1.0f };

		// ���۷����ǲ��Ǿ�̬�ģ���Ҫ���ݸ� Bullet �������棬��ֹ�����ķ���Ҳ����ײ
		float mass = 0.0f; // ��������������� 0 ��ʾ���ܵ�����Ӱ�죩
	};

	// ����������
	class TerrainGeneration
	{
	public:
		TerrainGeneration(GameDevice& game_device, PhysicalSimulationApp& physical_simulation_app, BaseGameObject::RenderAppObjMap& game_object_map);
		~TerrainGeneration();

		// ��ȫ�Բ���
		TerrainGeneration(const TerrainGeneration&) = delete;
		TerrainGeneration& operator = (const TerrainGeneration&) = delete;

		// ��̬����������
		void single_block_creator(BlockGenerateData block_data);
		void create_plane(int length, int width); // ֱ�Ӵ���һ��ƽ��
		void create_wall(int height, int width); // ����һ��ǽ

	private:
		GameDevice& game_device_;
		BaseGameObject::RenderAppObjMap& game_object_map_; // ��Ⱦ map ������
		PhysicalSimulationApp& physical_simulation_app_; // ���� app ������

		// ģ���ļ�·��
		std::string model_file_path_ = "models/block.obj";
	};
}
