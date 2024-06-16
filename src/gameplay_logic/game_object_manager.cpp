
#include "game_object_manager.h"

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
		std::call_once(init_instance_flag_, GameObjectManager::init_singleton);
		return instance_;
	}

	void GameObjectManager::insert_sharing_game_object_data(const id_t obj_id, const GameObjectPublicData game_object_data)
	{
		std::lock_guard<std::mutex> lock(mutex_);
		sharing_object_map_.emplace(obj_id, game_object_data);
	}

	std::shared_ptr<GameObjectPublicData> GameObjectManager::get_sharing_obj_by_id(id_t obj_id)
	{
		std::lock_guard<std::mutex> lock(mutex_); // ȷ���̰߳�ȫ
		auto it = sharing_object_map_.find(obj_id);

		if (it != sharing_object_map_.end()) // ���������Ϸ������ڵ����
		{
			return std::make_shared<GameObjectPublicData>(it -> second);
		}
		else
		{
			throw new std::runtime_error("�޷��ҵ���Ӧ����Ϸ���壬��������߼�");
		}
	}

	void GameObjectManager::add_physical_obj_by_id(id_t obj_id)
	{
		// ʹ�õ�ʱ��Ӧ���Ƚ� game object �������ݿ⣬�ٽ���������ȡ�����ã�˳���ܴ�
		std::shared_ptr<GameObjectPublicData> new_physical_data = get_sharing_obj_by_id(obj_id);
		physical_game_obj_vector_.push_back(new_physical_data);
	}

	std::vector<std::shared_ptr<GameObjectPublicData>>& GameObjectManager::get_physical_obj_vector()
	{
		return physical_game_obj_vector_;
	}

#pragma endregion

#pragma region GameObjectPublicData ���

	GameObjectPublicData::GameObjectPublicData(id_t obj_id, bool is_static, TransformComponent transform_component) : id_pub_ {obj_id}, is_static_ {is_static}, transform_pub_ {transform_component} {}

#pragma endregion

}
