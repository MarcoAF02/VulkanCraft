
#pragma once

#include "../character_controller.h"

// std
#include <iostream>
#include <memory>

// Character Controller 类里不需要包含状态机类，状态机类会读到 Character Controller 类并调用其中的函数

namespace vulkancraft
{
	// FSM 状态机接口
	class FSMState
	{
	public:
		virtual ~FSMState() {};
		virtual void enter(std::shared_ptr<CharacterController> character_controller) = 0;
		virtual void execute(std::shared_ptr<CharacterController> character_controller, float delta_time) = 0;
		virtual void exit(std::shared_ptr<CharacterController> character_controller) = 0;

		// 切换状态的条件
		virtual std::shared_ptr<FSMState> switch_state_condition(std::shared_ptr<CharacterController> character_controller) = 0;
	};

	// 有限状态机执行类
	class FSMExecute
	{
	public:
		FSMExecute(std::shared_ptr<FSMState> initial_state);
		void update(std::shared_ptr<CharacterController> character_controller, float delta_time);

	private:
		std::shared_ptr<FSMState> current_state_ = nullptr;
	};

	// 不同具体的状态类
	class PlayerControlState : public FSMState // 玩家游戏控制状态
	{
		void enter(std::shared_ptr<CharacterController> character_controller) override;
		void execute(std::shared_ptr<CharacterController> character_controller, float delta_time) override;
		void exit(std::shared_ptr<CharacterController> character_controller) override;

		std::shared_ptr<FSMState> switch_state_condition(std::shared_ptr<CharacterController> character_controller) override;
	};

	class PauseMenuState : public FSMState// 暂停菜单开启状态
	{
		void enter(std::shared_ptr<CharacterController> character_controller) override;
		void execute(std::shared_ptr<CharacterController> character_controller, float delta_time) override;
		void exit(std::shared_ptr<CharacterController> character_controller) override;

		std::shared_ptr<FSMState> switch_state_condition(std::shared_ptr<CharacterController> character_controller) override;
	};
}
