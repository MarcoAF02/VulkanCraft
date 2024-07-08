
#include "game_object_manager.h"

struct vulkancraft::BlockGenerateData;

namespace vulkancraft
{

#pragma region 单例类相关

	// 初始化静态成员
	std::once_flag GameObjectManager::init_instance_flag_;
	std::shared_ptr<GameObjectManager> GameObjectManager::instance_ = nullptr;

	GameObjectManager::GameObjectManager() { } // 啥也没有
	GameObjectManager::~GameObjectManager() { } // 啥也没有

	void GameObjectManager::init_singleton()
	{
		instance_ = std::make_shared<GameObjectManager>();
	}

	std::shared_ptr<GameObjectManager> GameObjectManager::get_instance()
	{
		// 确保它只被执行了一次
		std::call_once(init_instance_flag_, &GameObjectManager::init_singleton);
		return instance_;
	}

	void GameObjectManager::insert_sharing_game_object_data(id_t id_pub, GameObjectPublicData data)
	{
		std::lock_guard<std::mutex> lock(mutex_); // 保护数据安全

		if (game_obj_public_data_map_.contains(id_pub)) // 重复添加
		{
			std::cerr << "ID 为 " << id_pub << " 的游戏物体被重复添加至 Game Object Manager。本次添加将忽略" << std::endl;
			return;
		}
		else // 正确添加
		{
			game_obj_public_data_map_.emplace(id_pub, data);
		}	
	}

	GameObjectPublicData GameObjectManager::get_public_data_by_id(id_t id_pub)
	{
		std::lock_guard<std::mutex> lock(mutex_); // 保护数据安全

		if (game_obj_public_data_map_.contains(id_pub))
		{
			return game_obj_public_data_map_[id_pub];
		}
		else
		{
			std::cerr << "没有 ID 为 " << id_pub << " 的游戏公共数据" << std::endl;
			throw std::runtime_error("异常已抛出\n");
		}
	}

#pragma endregion

#pragma region GameObjectPublicData 相关

	GameObjectPublicData::GameObjectPublicData(id_t id_pub, btScalar mass, TransformComponent transform_component, btVector3 local_inertia_pub) : id_pub_{id_pub}, mass_{mass}, transform_pub_{transform_component}, local_inertia_pub_{local_inertia_pub} { }

#pragma endregion

}
