
#pragma once

// stl
#include <iostream>
#include <memory>
#include <stdexcept>
#include <atomic>
#include <mutex>

namespace vulkancraft
{
	// �߳�״̬������
	class ThreadStateManager
	{
	public:

		ThreadStateManager();
		~ThreadStateManager();

		ThreadStateManager(const ThreadStateManager&) = delete;
		ThreadStateManager& operator = (const ThreadStateManager&) = delete;

		static std::shared_ptr<ThreadStateManager> get_instance(); // �õ�����

		void set_render_thread_state_to_phy(bool new_state);
		bool& get_render_thread_state_to_phy();

	private:

		static std::once_flag init_instance_flag_;
		static std::shared_ptr<ThreadStateManager> instance_;

		bool render_thread_finished_to_phy_ = false; // ��¼��Ⱦ�߳��Ƿ����
		mutable std::mutex mutex_; // ���ڱ����̰߳�ȫ�Ļ�����

		// ��ʼ������
		static void init_singleton();
	};
}
