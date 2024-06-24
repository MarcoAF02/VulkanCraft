
#pragma once

#include "../gameplay_logic/character_controller.h" // ��ҽ�ɫ������

// std
#include <vector>
#include <memory>
#include <condition_variable> // �������������ڲ�ͬ�̼߳��ͨ��

// �����ࣺ��Ϸʵ�������
namespace vulkancraft
{
	class GameEntityManager
	{
	public:
		GameEntityManager();
		GameEntityManager(GLFWwindow* glfw_window);
		~GameEntityManager();

		GameEntityManager(const GameEntityManager&) = delete;
		GameEntityManager& operator = (const GameEntityManager&) = delete;

		void create_player();
		static GLFWwindow* get_glfw_window();

		// �õ����������ǲ��ö�ָ�븳ֵ
		static std::shared_ptr<GameEntityManager> get_instance_no_glfw_ptr();

		// �õ�������������Ҫ��ָ�븳ֵ
		static std::shared_ptr<GameEntityManager> get_instance(GLFWwindow* glfw_window);
		static void wait_until_window_initialized(); // �ȴ� glfw_window ָ�벻Ϊ��

	private:
		static std::once_flag init_instance_flag_;
		static std::shared_ptr<GameEntityManager> instance_;

		mutable std::mutex mutex_; // ���ڱ����̰߳�ȫ�Ļ�����

		static std::condition_variable cv_; // ��������
		static std::mutex cv_mutex_; // �������������׵Ļ�����
		static bool glfw_window_initialized_; // ��� GLFWwindow �Ƿ��ѳ�ʼ��

		std::shared_ptr<CharacterController> character_controller_; // ��ҽ�ɫ������
		static GLFWwindow* glfw_window_;

		// ��ʼ������
		static void init_singleton();
	};

}
