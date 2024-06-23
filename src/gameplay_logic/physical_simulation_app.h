
#pragma once

#include "../managers/game_object_manager.h"
#include "../managers/thread_state_manager.h"

#include <iostream>
#include <chrono>
#include <cmath>

namespace vulkancraft
{
	class PhysicalSimulationApp // ����ģ�� App ��
	{
	public:
		PhysicalSimulationApp();
		~PhysicalSimulationApp();

		PhysicalSimulationApp(const PhysicalSimulationApp&) = delete;
		PhysicalSimulationApp& operator = (const PhysicalSimulationApp&) = delete;

		void update_physical_simulation(); // ѭ����������ģ��
		void calculate_aabb_collider(); // ��⶯̬�����Ƿ��໥��ײ

	private:

		using Clock = std::chrono::high_resolution_clock; // Clock ������
		using Duration = std::chrono::duration<double>;

		std::shared_ptr<GameObjectManager> game_object_manager_; // ��Ϸ�������������
		std::shared_ptr<ThreadStateManager> thread_state_manager_; // �̹߳�����

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
	};
}
