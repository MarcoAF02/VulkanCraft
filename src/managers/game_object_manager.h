
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

// 全局类型别名
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
		id_t id_pub_ = 0; // 共享数据的 ID
		btScalar mass_ = 0; // 物体的质量（物理引擎用）
		TransformComponent transform_pub_{};
		btVector3 local_inertia_pub_ = { 0.0f, 0.0f, 0.0f }; // 局部惯性张量矩阵
	};

	class GameObjectManager // 多线程单例类
	{
	public:

		// TODO: 先创建渲染游戏物体，再创建物理游戏物体
		// TODO: 这里储存 渲染，物理 APP 需要的变量的引用
		
		// RenderAppObjMap 储存在 Render App 类里，这里只是记录引用
		// PhysicsAppObjMap 储存在 PhysicalSimulationApp 类里，这里只是记录引用
		GameObjectManager();
		~GameObjectManager();

		// 这个必须有，这个必须有，这个必须有
		GameObjectManager(const GameObjectManager&) = delete; // 禁止拷贝构造
		GameObjectManager& operator = (const GameObjectManager&) = delete; // 禁止赋值操作

		// 插入游戏共享数据
		void insert_sharing_game_object_data(id_t id_pub, GameObjectPublicData data);

		// 根据指定 ID 获得共享数据
		GameObjectPublicData get_public_data_by_id(id_t id_pub);

		// 得到单例
		static std::shared_ptr<GameObjectManager> get_instance();

	private:

		static std::once_flag init_instance_flag_;
		static std::shared_ptr<GameObjectManager> instance_;

		mutable std::mutex mutex_; // 用于保护线程安全的互斥锁

		ObjectPublicDataMap game_obj_public_data_map_{};

		// 初始化单例
		static void init_singleton();
	};
}
