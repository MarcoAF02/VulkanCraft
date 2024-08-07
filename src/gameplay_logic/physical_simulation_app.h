
#pragma once

#include "../managers/thread_state_manager.h"
#include "../managers/game_entity_manager.h"
#include "../base_tools/physics_object_save_data.h"

// Bullet Physics
#include "btBulletDynamicsCommon.h"

#include <glm/glm.hpp>

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
class PhysicsObjectGenerator;

namespace vulkancraft
{
	// 物理游戏对象的创建数据
	struct PhysicsObjectCreateData
	{
		btVector3 obj_size{}; // Box Collider 的尺寸
		btVector3 obj_origin{}; // Box Collider 的起点
		btTransform transform{}; // 这个需要用 setOrigin() 函数来初始化位置
		btScalar mass{}; // 质量
		btVector3 local_inertia{};
		bool disable_deactivate = false; // 创建出来的 Rigidbody 是否禁用停用
	};

	class PhysicalSimulationApp // 物理模拟 App 类
	{
	public:
		std::thread physics_thread_; // 物理线程
		std::atomic<bool> should_stop_{ false }; // 停止标记
		std::atomic<bool> is_phy_sim_started{ false }; // 记录物理模拟是不是真正开始了

		PhysicalSimulationApp();
		~PhysicalSimulationApp();

		PhysicalSimulationApp(const PhysicalSimulationApp&) = delete;
		PhysicalSimulationApp& operator = (const PhysicalSimulationApp&) = delete;

		void init_physics_system(); // 初始化物理系统
		void clear_physics_system(); // 清理物理系统（如内存等）
		void update_bullet_physics_world(); // 循环更新由 Bullet 3 创建的物理世界
		void update_physical_simulation(); // 循环更新物理模拟
		void register_player_physics(); // 注册玩家物理系统
		void start_physical_thread(); // 开始物理循环线程

		GLFWwindow* get_glfw_window_ptr() const { return glfw_window_; }
		std::unordered_map<BaseGameObject::id_t, PhysicsObjectSaveData>& get_phy_obj_map() { return physics_obj_map_; }

#pragma region 游戏物理对象创建用函数

		// 创建单个物理方块
		void create_single_physics_block(BaseGameObject::id_t obj_id, PhysicsObjectCreateData data);
		
#pragma endregion

	private:
		std::mutex collision_shape_array_mutex_; // 保护线程安全用的互斥锁

		GLFWwindow* glfw_window_ = nullptr; // 渲染线程内的窗口指针

		using Clock = std::chrono::high_resolution_clock; // Clock 类型名
		using Duration = std::chrono::duration<double>;

		std::shared_ptr<GameEntityManager> game_entity_manager_ = nullptr; // 实体管理器单例

		// Bullet Physics 物理引擎相关初始化
		// HACK: 注意物理引擎使用的都是原始指针，记得释放内存
		btDefaultCollisionConfiguration* collision_default_config_ = nullptr;
		btCollisionDispatcher* dispatcher_ = nullptr;
		btBroadphaseInterface* overlapping_pair_cache_ = nullptr;
		btSequentialImpulseConstraintSolver* solver_ = nullptr;
		btDiscreteDynamicsWorld* dynamics_world_ = nullptr;

		// 储存物理对象
		std::unordered_map<BaseGameObject::id_t, PhysicsObjectSaveData> physics_obj_map_;
		std::shared_ptr<PhysicsObjectGenerator> physics_object_generator_ = nullptr;

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

		std::atomic<float> accumulator_delta_time_{ 0 };

#pragma region DEBUG 用函数

		void test_create_dynamic_rigidbody(); // 尝试创建一个动态物理物体

#pragma endregion

	};
}
