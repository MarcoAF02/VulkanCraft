
#include "game_main.h"

// HACK: ȫ��Ϸ����ڵ�
int main(void)
{
	try
	{
		std::unique_ptr<GameMain> game_main = std::make_unique<GameMain>();
		game_main -> game_render_app_.update_render_window_content();
	}
	catch (std::string& e)
	{
		std::cout << "GameMain ���ʼ��ʧ�ܣ�" << e << std::endl;
	}

	return 0;
}
