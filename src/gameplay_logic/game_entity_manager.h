
#pragma once

#include "../gameplay_logic/character_controller.h" // ��ҽ�ɫ������

// std
#include <vector>
#include <memory>

// �����ࣺ��Ϸʵ�������
namespace vulkancraft
{
	class GameEntityManager
	{
	public:
		GameEntityManager();
		~GameEntityManager();

		GameEntityManager(const GameEntityManager&) = delete;
		GameEntityManager& operator = (const GameEntityManager&) = delete;

		void create_player();

		static std::shared_ptr<GameEntityManager> get_instance();
		std::shared_ptr<CharacterController> get_character_controller();

	private:
		static std::once_flag init_instance_flag_;
		static std::shared_ptr<GameEntityManager> instance_;

		mutable std::mutex mutex_; // ���ڱ����̰߳�ȫ�Ļ�����

		std::shared_ptr<CharacterController> character_controller_; // ��ҽ�ɫ������

		// ��ʼ������
		static void init_singleton();
	};

}
