
#pragma once

#include "../gameplay_logic/base_game_object.h"
#include "../game_physics_system/physics_aabb.h"

// libs
#include <glm/gtc/matrix_transform.hpp>

// std
#include <memory>
#include <mutex>
#include <unordered_map>

class vulkancraft::BaseGameObject;

namespace vulkancraft
{
	using id_t = unsigned _int64;

	// 游戏对象的公共数据记录的不多，节省内存
	class GameObjectPublicData
	{
	public:
		TransformComponent transform_pub_ = {};
		AABBCollider aabb_collider_;

		GameObjectPublicData(id_t obj_id, bool is_static, TransformComponent transform_component, AABBCollider aabb_collider);
		id_t get_pub_id() {return id_pub_;}

	private:
		id_t id_pub_ = 0;
		bool is_static_ = false;

		// 禁用默认构造函数
		GameObjectPublicData() = default;
	};

	using PublicObjMap = std::unordered_map<id_t, GameObjectPublicData>;

	class GameObjectManager // 多线程单例类
	{
	public:

		GameObjectManager();
		~GameObjectManager();

		// 这个必须有，这个必须有，这个必须有
		GameObjectManager(const GameObjectManager&) = delete; // 禁止拷贝构造
		GameObjectManager& operator = (const GameObjectManager&) = delete; // 禁止赋值操作

		// 得到单例
		static std::shared_ptr<GameObjectManager> get_instance();

		// 根据 ID 得到共享 game_object 数据
		std::shared_ptr<GameObjectPublicData> get_sharing_obj_by_id(const id_t obj_id);

		// 插入共享 game_object 数据（插入行为需要在内存中拷贝一份，而不是给指针）
		void insert_sharing_game_object_data(const id_t obj_id, const GameObjectPublicData game_object_data);

		// 添加需要进行物理计算的 game_object
		void add_physical_obj_by_id(const id_t obj_id);

		// 得到需要应用物理计算的 vector（得到指针数组的引用）
		std::vector<std::shared_ptr<GameObjectPublicData>>& get_physical_obj_vector();

	private:

		static std::once_flag init_instance_flag_;
		static std::shared_ptr<GameObjectManager> instance_;

		PublicObjMap sharing_object_map_; // 共享数据的游戏物体
		std::vector<std::shared_ptr<GameObjectPublicData>> physical_game_obj_vector_; // 需要应用物理计算的游戏物体

		mutable std::mutex mutex_; // 用于保护线程安全的互斥锁

		// 初始化单例
		static void init_singleton();
	};
}
