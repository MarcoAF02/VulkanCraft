
#pragma once

#include "../character_controller.h"

// std
#include <iostream>
#include <memory>

// Character Controller ���ﲻ��Ҫ����״̬���࣬״̬�������� Character Controller �ಢ�������еĺ���

namespace vulkancraft
{
	// FSM ״̬���ӿ�
	class FSMState
	{
	public:
		virtual ~FSMState() {};
		virtual void enter(std::shared_ptr<CharacterController> character_controller) = 0;
		virtual void execute(std::shared_ptr<CharacterController> character_controller, float delta_time) = 0;
		virtual void exit(std::shared_ptr<CharacterController> character_controller) = 0;

		// �л�״̬������
		virtual std::shared_ptr<FSMState> switch_state_condition(std::shared_ptr<CharacterController> character_controller) = 0;
	};

	// ����״̬��ִ����
	class FSMExecute
	{
	public:
		FSMExecute(std::shared_ptr<FSMState> initial_state);
		void update(std::shared_ptr<CharacterController> character_controller, float delta_time);

	private:
		std::shared_ptr<FSMState> current_state_ = nullptr;
	};

	// ��ͬ�����״̬��
	class PlayerControlState : public FSMState // �����Ϸ����״̬
	{
		void enter(std::shared_ptr<CharacterController> character_controller) override;
		void execute(std::shared_ptr<CharacterController> character_controller, float delta_time) override;
		void exit(std::shared_ptr<CharacterController> character_controller) override;

		std::shared_ptr<FSMState> switch_state_condition(std::shared_ptr<CharacterController> character_controller) override;
	};

	class PauseMenuState : public FSMState// ��ͣ�˵�����״̬
	{
		void enter(std::shared_ptr<CharacterController> character_controller) override;
		void execute(std::shared_ptr<CharacterController> character_controller, float delta_time) override;
		void exit(std::shared_ptr<CharacterController> character_controller) override;

		std::shared_ptr<FSMState> switch_state_condition(std::shared_ptr<CharacterController> character_controller) override;
	};
}
