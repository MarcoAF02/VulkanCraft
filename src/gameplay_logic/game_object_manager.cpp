
#include "game_object_manager.h"

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
		std::lock_guard<std::mutex> lock(mutex_); // 确保线程安全
		auto it = sharing_object_map_.find(obj_id);

		if (it != sharing_object_map_.end()) // 对于这个游戏物体存在的情况
		{
			return std::make_shared<GameObjectPublicData>(it -> second);
		}
		else
		{
			throw new std::runtime_error("无法找到对应的游戏物体，请检查代码逻辑");
		}
	}

	void GameObjectManager::add_physical_obj_by_id(id_t obj_id)
	{
		// 使用的时候，应该先将 game object 公共数据库，再将它从这里取出来用，顺序不能错。
		std::shared_ptr<GameObjectPublicData> new_physical_data = get_sharing_obj_by_id(obj_id);
		physical_game_obj_vector_.push_back(new_physical_data);
	}

	std::vector<std::shared_ptr<GameObjectPublicData>>& GameObjectManager::get_physical_obj_vector()
	{
		return physical_game_obj_vector_;
	}

#pragma endregion

#pragma region GameObjectPublicData 相关

	GameObjectPublicData::GameObjectPublicData(id_t obj_id, bool is_static, TransformComponent transform_component) : id_pub_ {obj_id}, is_static_ {is_static}, transform_pub_ {transform_component} {}

#pragma endregion

}
