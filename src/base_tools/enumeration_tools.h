
#pragma once

// �����¼һЩ״̬�ж��õ�ö��
namespace vulkancraft
{
	enum RotateState
	{
		kRotateAll, // һ����ת
		kRotateNone, // ��������ת
		kRotateX, // ���� X ����ת���� pitch����һ�˳�������������
		kRotateY, // ���� Y ����ת���� yaw
		kRotateZ // ���� Z ����ת���� roll
	};

	// ��ҵ��ƶ�����״̬
	enum PlayerMoveInputState
	{
		kStop, // ֹͣ
		kMoveInput // �����ƶ�����
	};
}
