
#pragma once

#include "../gameplay_logic/base_game_object.h"
#include "../gameplay_logic/system/terrain_generation.h"

// Bullet Physics
#include "btBulletDynamicsCommon.h"

// libs
#include <glm/gtc/matrix_transform.hpp>

// std
#include <memory>
#include <mutex>
#include <unordered_map>

class vulkancraft::BaseGameObject;
struct vulkancraft::BlockGenerateData;

// ȫ�����ͱ���
using id_t = unsigned _int64;
using RenderAppObjMap = std::unordered_map<id_t, vulkancraft::BaseGameObject>;
using ObjectPublicDataMap = std::unordered_map<id_t, vulkancraft::GameObjectPublicData>;

namespace vulkancraft
{
	class GameObjectPublicData
	{
	public:
		GameObjectPublicData(id_t id_pub, btScalar mass, TransformComponent transform_component, btVector3 local_inertia_pub);

	private:
		id_t id_pub_ = 0; // �������ݵ� ID
		btScalar mass_ = 0; // ��������������������ã�
		TransformComponent transform_pub_{};
		btVector3 local_inertia_pub_ = { 0.0f, 0.0f, 0.0f }; // �ֲ�������������
	};

	class GameObjectManager // ���̵߳�����
	{
	public:

		// TODO: �ȴ�����Ⱦ��Ϸ���壬�ٴ���������Ϸ����
		// TODO: ���ﴢ�� ��Ⱦ������ APP ��Ҫ�ı���������
		
		// RenderAppObjMap ������ Render App �������ֻ�Ǽ�¼����
		// PhysicsAppObjMap ������ PhysicalSimulationApp �������ֻ�Ǽ�¼����
		GameObjectManager();
		~GameObjectManager();

		// ��������У���������У����������
		GameObjectManager(const GameObjectManager&) = delete; // ��ֹ��������
		GameObjectManager& operator = (const GameObjectManager&) = delete; // ��ֹ��ֵ����

		// ������Ϸ��������
		void insert_sharing_game_object_data(id_t id_pub, GameObjectPublicData data);

		// ����ָ�� ID ��ù�������
		GameObjectPublicData get_public_data_by_id(id_t id_pub);

		// �õ�����
		static std::shared_ptr<GameObjectManager> get_instance();

	private:

		static std::once_flag init_instance_flag_;
		static std::shared_ptr<GameObjectManager> instance_;

		mutable std::mutex mutex_; // ���ڱ����̰߳�ȫ�Ļ�����

		ObjectPublicDataMap game_obj_public_data_map_{};

		// ��ʼ������
		static void init_singleton();
	};
}
