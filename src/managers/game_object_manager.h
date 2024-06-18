
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

	// ��Ϸ����Ĺ������ݼ�¼�Ĳ��࣬��ʡ�ڴ�
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

		// ����Ĭ�Ϲ��캯��
		GameObjectPublicData() = default;
	};

	using PublicObjMap = std::unordered_map<id_t, GameObjectPublicData>;

	class GameObjectManager // ���̵߳�����
	{
	public:

		GameObjectManager();
		~GameObjectManager();

		// ��������У���������У����������
		GameObjectManager(const GameObjectManager&) = delete; // ��ֹ��������
		GameObjectManager& operator = (const GameObjectManager&) = delete; // ��ֹ��ֵ����

		// �õ�����
		static std::shared_ptr<GameObjectManager> get_instance();

		// ���� ID �õ����� game_object ����
		std::shared_ptr<GameObjectPublicData> get_sharing_obj_by_id(const id_t obj_id);

		// ���빲�� game_object ���ݣ�������Ϊ��Ҫ���ڴ��п���һ�ݣ������Ǹ�ָ�룩
		void insert_sharing_game_object_data(const id_t obj_id, const GameObjectPublicData game_object_data);

		// �����Ҫ������������ game_object
		void add_physical_obj_by_id(const id_t obj_id);

		// �õ���ҪӦ���������� vector���õ�ָ����������ã�
		std::vector<std::shared_ptr<GameObjectPublicData>>& get_physical_obj_vector();

	private:

		static std::once_flag init_instance_flag_;
		static std::shared_ptr<GameObjectManager> instance_;

		PublicObjMap sharing_object_map_; // �������ݵ���Ϸ����
		std::vector<std::shared_ptr<GameObjectPublicData>> physical_game_obj_vector_; // ��ҪӦ������������Ϸ����

		mutable std::mutex mutex_; // ���ڱ����̰߳�ȫ�Ļ�����

		// ��ʼ������
		static void init_singleton();
	};
}
