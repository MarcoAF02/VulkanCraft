
#include "game_main.h"

// HACK: ȫ��Ϸ����ڵ�
int main(void)
{
	try
	{
		std::unique_ptr<GameMain> game_main = std::make_unique<GameMain>();
	}
	catch (std::string& e)
	{
		std::cout << "GameMain ���ʼ��ʧ�ܣ�" << e << std::endl;
	}

	return 0;
}
