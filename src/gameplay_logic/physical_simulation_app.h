
#pragma once

#include "../managers/thread_state_manager.h"
#include "../managers/game_entity_manager.h"

// Bullet Physics
#include "btBulletDynamicsCommon.h"

// std
#include <iostream>
#include <memory>
#include <chrono>
#include <cmath>
#include <unordered_map>
#include <thread>
#include <mutex>
#include <condition_variable>

class ThreadStateManager;

namespace vulkancraft
{
	// ������Ϸ����Ĵ�������
	struct PhysicsObjectCreateData
	{
		btVector3 obj_size{}; // Box Collider �ĳߴ�
		btVector3 obj_origin{}; // Box Collider �����
		btTransform transform{}; // �����Ҫ�� setOrigin() ��������ʼ��λ��
		btScalar mass{}; // ����
		btVector3 local_inertia{};
	};

	// ����������������õ�����
	struct PhysicsObjectSaveData
	{
		btCollisionShape* collision_shape;
		btRigidBody* rigid_body;
	};

	class PhysicalSimulationApp // ����ģ�� App ��
	{
	public:
		std::thread physics_thread_; // �����߳�
		std::atomic<bool> should_stop_{ false }; // ֹͣ���

		// ���ͱ���
		using id_t = unsigned _int64;

		PhysicalSimulationApp();
		~PhysicalSimulationApp();

		PhysicalSimulationApp(const PhysicalSimulationApp&) = delete;
		PhysicalSimulationApp& operator = (const PhysicalSimulationApp&) = delete;

		void init_physics_system(); // ��ʼ������ϵͳ
		void clear_physics_system(); // ��������ϵͳ�����ڴ�ȣ�
		void update_bullet_physics_world(); // ѭ�������� Bullet 3 ��������������
		void update_physical_simulation(); // ѭ����������ģ��

		void start_physical_thread(); // ��ʼ����ѭ���߳�

		GLFWwindow* get_glfw_window_ptr() const {return glfw_window_;}

#pragma region ��Ϸ������󴴽��ú���

		// TODO: �������
		void create_single_physics_block(PhysicsObjectCreateData data);

#pragma endregion

	private:
		std::mutex collision_shape_array_mutex_; // �����̰߳�ȫ�õĻ�����

		GLFWwindow* glfw_window_ = nullptr; // ��Ⱦ�߳��ڵĴ���ָ��

		using Clock = std::chrono::high_resolution_clock; // Clock ������
		using Duration = std::chrono::duration<double>;

		std::shared_ptr<GameEntityManager> game_entity_manager_ = nullptr; // ʵ�����������

		// Bullet Physics ����������س�ʼ��
		// HACK: ע����������ʹ�õĶ���ԭʼָ�룬�ǵ��ͷ��ڴ�
		btDefaultCollisionConfiguration* collision_default_config_ = nullptr;
		btCollisionDispatcher* dispatcher_ = nullptr;
		btBroadphaseInterface* overlapping_pair_cache_ = nullptr;
		btSequentialImpulseConstraintSolver* solver_ = nullptr;
		btDiscreteDynamicsWorld* dynamics_world_ = nullptr;

		// �����������
		// TODO: ����ֻ����ʹ�� unordered_map
		btAlignedObjectArray<btCollisionShape*> collision_shape_array_;
		std::unordered_map<id_t, PhysicsObjectSaveData> physics_obj_map_;

		const btVector3 kGravityVector = { 0.0f, 10.0f, 0.0f }; // ����

		// ׷��ģʽ��ѭ��ʱ��
		const double kTargetFps = 60.0; // ������µ�Ŀ��֡��
		const double kTimePerUpdate = 1.0 / kTargetFps; // ÿ֡���µ�ʱ����

		double accumulator_ = 0.0; // ʱ����ۻ�
		std::chrono::steady_clock::time_point previous_time_ = std::chrono::steady_clock::now(); // ��һ֡��ʱ���

		// ׷��ģʽ��ѭ��Ƶ��
		// 800000000.0
		const double kTimeStep = 0.0; // �������ѭ���ļ��
		std::chrono::steady_clock::time_point previous_step_ = std::chrono::steady_clock::now(); // ��һ�θ��µ�ʱ��
		double accumulator_step_ = 0.0; // ʱ����ۻ�

#pragma region DEBUG �ú���

		void test_create_dynamic_rigidbody(); // ���Դ���һ����̬��������

#pragma endregion

	};
}
