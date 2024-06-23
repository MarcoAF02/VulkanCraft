
#pragma once

#include "../managers/game_object_manager.h"
#include "../managers/thread_state_manager.h"

#include <iostream>
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

		void update_physical_simulation(); // 循环更新物理模拟
		void calculate_aabb_collider(); // 检测动态物体是否相互碰撞

	private:

		using Clock = std::chrono::high_resolution_clock; // Clock 类型名
		using Duration = std::chrono::duration<double>;

		std::shared_ptr<GameObjectManager> game_object_manager_; // 游戏公共对象管理单例
		std::shared_ptr<ThreadStateManager> thread_state_manager_; // 线程管理单例

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
	};
}
