
#include "finite_state_machine.h"

namespace vulkancraft
{

#pragma region 有限状态机执行类

	FSMExecute::FSMExecute(std::shared_ptr<FSMState> initial_state) : current_state_{ initial_state } { }

	void FSMExecute::update(std::shared_ptr<CharacterController> character_controller, float delta_time)
	{

	}

#pragma endregion

#pragma region 玩家控制状态类

	void PlayerControlState::enter(std::shared_ptr<CharacterController> character_controller)
	{

	}

	void PlayerControlState::execute(std::shared_ptr<CharacterController> character_controller, float delta_time)
	{

	}

	void PlayerControlState::exit(std::shared_ptr<CharacterController> character_controller)
	{

	}

	std::shared_ptr<FSMState> PlayerControlState::switch_state_condition(std::shared_ptr<CharacterController> character_controller)
	{
		return nullptr;
	}

#pragma endregion

#pragma region 玩家菜单开关类

	void PauseMenuState::enter(std::shared_ptr<CharacterController> character_controller)
	{

	}

	void PauseMenuState::execute(std::shared_ptr<CharacterController> character_controller, float delta_time)
	{

	}

	void PauseMenuState::exit(std::shared_ptr<CharacterController> character_controller)
	{

	}

	std::shared_ptr<FSMState> PauseMenuState::switch_state_condition(std::shared_ptr<CharacterController> character_controller)
	{
		return nullptr;
	}

#pragma endregion

}
