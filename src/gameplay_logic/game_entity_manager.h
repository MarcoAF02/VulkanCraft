
#pragma once

#include "../gameplay_logic/character_controller.h" // ��ҽ�ɫ������

// std
#include <vector>
#include <memory>

namespace vulkancraft
{
	// ���ﴢ�����ʵ������ü�������ָ������ã����������ʵ��Ĺ���
	// ����Ϸ��ʼ����ʱ���������Ϊ�������ݸ���ͬ�߳�
	class GameEntityManager
	{
	public:

		std::shared_ptr<CharacterController> character_controller_;
	};

}
