
#include "game_object_manager.h"

struct vulkancraft::BlockGenerateData;

namespace vulkancraft
{

#pragma region ���������

	// ��ʼ����̬��Ա
	std::once_flag GameObjectManager::init_instance_flag_;
	std::shared_ptr<GameObjectManager> GameObjectManager::instance_ = nullptr;

	GameObjectManager::GameObjectManager() { } // ɶҲû��
	GameObjectManager::~GameObjectManager() { } // ɶҲû��

	void GameObjectManager::init_singleton()
	{
		instance_ = std::make_shared<GameObjectManager>();
	}

	std::shared_ptr<GameObjectManager> GameObjectManager::get_instance()
	{
		// ȷ����ֻ��ִ����һ��
		std::call_once(init_instance_flag_, &GameObjectManager::init_singleton);
		return instance_;
	}

	void GameObjectManager::insert_sharing_game_object_data(id_t id_pub, GameObjectPublicData data)
	{
		std::lock_guard<std::mutex> lock(mutex_); // �������ݰ�ȫ

		if (game_obj_public_data_map_.contains(id_pub)) // �ظ����
		{
			std::cerr << "ID Ϊ " << id_pub << " ����Ϸ���屻�ظ������ Game Object Manager��������ӽ�����" << std::endl;
			return;
		}
		else // ��ȷ���
		{
			game_obj_public_data_map_.emplace(id_pub, data);
		}	
	}

	GameObjectPublicData GameObjectManager::get_public_data_by_id(id_t id_pub)
	{
		std::lock_guard<std::mutex> lock(mutex_); // �������ݰ�ȫ

		if (game_obj_public_data_map_.contains(id_pub))
		{
			return game_obj_public_data_map_[id_pub];
		}
		else
		{
			std::cerr << "û�� ID Ϊ " << id_pub << " ����Ϸ��������" << std::endl;
			throw std::runtime_error("�쳣���׳�\n");
		}
	}

#pragma endregion

#pragma region GameObjectPublicData ���

	GameObjectPublicData::GameObjectPublicData(id_t id_pub, btScalar mass, TransformComponent transform_component, btVector3 local_inertia_pub) : id_pub_{id_pub}, mass_{mass}, transform_pub_{transform_component}, local_inertia_pub_{local_inertia_pub} { }

#pragma endregion

}
