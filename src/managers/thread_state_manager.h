
#pragma once

// ��Ⱦ������ APP
#include "../gameplay_logic/game_render_app.h"
#include "../gameplay_logic/physical_simulation_app.h"

// stl
#include <iostream>
#include <memory>
#include <stdexcept>
#include <atomic>
#include <mutex>

class GameRenderApp;
class PhysicalSimulationApp;

// TODO: ��������ʱ�ȴ�����������࣬������ֱ𴴽� ���� APP����Ⱦ APP

namespace vulkancraft
{
	// �̹߳�����
	class ThreadStateManager
	{
	public:

		ThreadStateManager();
		~ThreadStateManager();

		ThreadStateManager(const ThreadStateManager&) = delete;
		ThreadStateManager& operator = (const ThreadStateManager&) = delete;

		static std::shared_ptr<ThreadStateManager> get_instance(); // �õ�����

		void create_render_thread(); // ������Ⱦ�߳�
		void create_physical_thread(); // ���������߳�
		void wait_for_threads(); // �ȴ��߳�ִ�����

		std::shared_ptr<PhysicalSimulationApp> get_physical_simulation_app_ptr() const {return physical_simulation_app_;}

		void set_render_thread_state_to_phy(bool new_state);
		bool& get_render_thread_state_to_phy();

	private:

		static std::once_flag init_instance_flag_;
		static std::shared_ptr<ThreadStateManager> instance_;

		std::thread render_thread_; // ��Ⱦ APP ���߳�
		std::thread physical_thread_; // ���� APP ���߳�

		// ָ�������̵߳�����ָ��
		std::shared_ptr<GameRenderApp> game_render_app_;
		std::shared_ptr<PhysicalSimulationApp> physical_simulation_app_;

		// TODO: �������Ҫȥ��
		bool render_thread_finished_to_phy_ = false; // ��¼��Ⱦ�߳��Ƿ����
		mutable std::mutex mutex_; // ���ڱ����̰߳�ȫ�Ļ�����

		// ��ʼ������
		static void init_singleton();

		void create_render_app(); // ������Ⱦ App
		void create_physical_app(); // �������� App
	};
}
