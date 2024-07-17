
#pragma once

// 这里记录一些状态判断用的枚举
namespace vulkancraft
{
	enum RotateState
	{
		kRotateAll, // 一起旋转
		kRotateNone, // 不控制旋转
		kRotateX, // 仅绕 X 轴旋转，即 pitch（第一人称摄像机用这个）
		kRotateY, // 仅绕 Y 轴旋转，即 yaw
		kRotateZ // 仅绕 Z 轴旋转，即 roll
	};

	// 玩家的移动输入状态
	enum PlayerMoveInputState
	{
		kStop, // 停止
		kMoveInput // 正在移动输入
	};
}
