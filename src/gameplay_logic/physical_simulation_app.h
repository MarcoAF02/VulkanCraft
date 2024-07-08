
#pragma once

#include "../managers/game_object_manager.h"
#include "../managers/thread_state_manager.h"
#include "../managers/game_entity_manager.h"

// Bullet Physics
#include "btBulletDynamicsCommon.h"

// std
#include <iostream>
#include <memory>
#include <chrono>
#include <cmath>

namespace vulkancraft
{
	class PhysicalSimulationApp // 物理模拟 App 类
	{
	public:
		PhysicalSimulationApp();
		~PhysicalSimulationApp();

		PhysicalSimulationApp(const PhysicalSimulationApp&) = delete;
		PhysicalSimulationApp& operator = (const PhysicalSimulationApp&) = delete;

		void init_physics_system(); // 初始化物理系统
		void clear_physics_system(); // 清理物理系统（如内存等）
		void update_bullet_physics_world(); // 循环更新由 Bullet 3 创建的物理世界
		void update_physical_simulation(); // 循环更新物理模拟

	private:
		GLFWwindow* glfw_window_ = nullptr; // 渲染线程内的窗口指针

		using Clock = std::chrono::high_resolution_clock; // Clock 类型名
		using Duration = std::chrono::duration<double>;

		std::shared_ptr<GameObjectManager> game_object_manager_ = nullptr; // 游戏公共对象管理单例
		std::shared_ptr<ThreadStateManager> thread_state_manager_ = nullptr; // 线程管理单例
		std::shared_ptr<GameEntityManager> game_entity_manager_ = nullptr; // 实体管理器单例

		// Bullet Physics 物理引擎相关初始化
		// HACK: 注意物理引擎使用的都是原始指针，记得释放内存
		btDefaultCollisionConfiguration* collision_default_config_ = nullptr;
		btCollisionDispatcher* dispatcher_ = nullptr;
		btBroadphaseInterface* overlapping_pair_cache_ = nullptr;
		btSequentialImpulseConstraintSolver* solver_ = nullptr;
		btDiscreteDynamicsWorld* dynamics_world_ = nullptr;

		btAlignedObjectArray<btCollisionShape*> collision_shape_array_;

		const btVector3 kGravityVector = { 0.0f, 10.0f, 0.0f }; // 重力

		// 追赶模式的循环时间
		const double kTargetFps = 60.0; // 物理更新的目标帧率
		const double kTimePerUpdate = 1.0 / kTargetFps; // 每帧更新的时间间隔

		double accumulator_ = 0.0; // 时间差累积
		std::chrono::steady_clock::time_point previous_time_ = std::chrono::steady_clock::now(); // 上一帧的时间点

		// 追赶模式的循环频率
		// 800000000.0
		const double kTimeStep = 0.0; // 物理更新循环的间隔
		std::chrono::steady_clock::time_point previous_step_ = std::chrono::steady_clock::now(); // 上一次更新的时间
		double accumulator_step_ = 0.0; // 时间差累积

#pragma region DEBUG 用函数

		void test_create_dynamic_rigidbody(); // 尝试创建一个动态物理物体

#pragma endregion

	};
}
