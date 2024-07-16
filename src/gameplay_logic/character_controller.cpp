
#include "character_controller.h"

namespace vulkancraft
{
	CharacterController::CharacterController() { }
	CharacterController::~CharacterController() { }

	GameBaseCamera& CharacterController::get_player_camera() { return player_camera_; }

	void CharacterController::init_character_controller(glm::vec3 spawn_point)
	{
		spawn_point_ = spawn_point;

		character_game_obj_ = BaseGameObject::create_game_object(false); // ������ɫ��Ϸ����
		camera_game_obj_ = BaseGameObject::create_game_object(false); // �����������Ϸ����

		// ��ʼ�����վλ
		character_game_obj_.transform_.translation = spawn_point_;
		character_game_obj_.transform_.rotation = { 0, 0, 0 };
		character_game_obj_.transform_.scale = { 1, 1, 1 }; // ��ҵ����Ų��øı�

		// ��ʼ���������վλ
		camera_game_obj_.transform_.translation = spawn_point_ + glm::vec3{ 0, -camera_height_, 0 };
		camera_game_obj_.transform_.rotation = character_game_obj_.transform_.rotation; // ��ת����ҽ�ɫ����һ��
		camera_game_obj_.transform_.scale = { 1, 1, 1 }; // ��������Ų��øı�

		print_player_details();
	}

	void CharacterController::init_character_physics(BaseGameObject::RenderAppObjMap& game_object_map, std::unordered_map<BaseGameObject::id_t, PhysicsObjectSaveData>& physics_obj_map)
	{
		float half_height = character_height_ / 2;
		float half_width = character_width_ / 2;

		BlockGenerateData collider_data =
		{
			character_game_obj_.transform_.translation, // λ�����ڳ�����
			character_game_obj_.transform_.rotation,
			character_game_obj_.transform_.scale,

			1.0f // ����
		};


	}

	void CharacterController::print_player_details() const
	{
		// TODO: ��ӡ Collider �����������ת

		// ���״̬
		std::cout << "��ҳ�����Ϊ��"
			<< spawn_point_.x << ", "
			<< spawn_point_.y << ", "
			<< spawn_point_.z << std::endl;

		std::cout << "��ҵ�ǰ�ڵĵ�λΪ��"
			<< character_game_obj_.transform_.translation.x << ", "
			<< character_game_obj_.transform_.translation.y << ", "
			<< character_game_obj_.transform_.translation.z << std::endl;

		std::cout << "��ҵ�ǰ����ת�Ƕ�Ϊ��"
			<< character_game_obj_.transform_.rotation.x << ", "
			<< character_game_obj_.transform_.rotation.y << ", "
			<< character_game_obj_.transform_.rotation.z << std::endl;

		std::cout << "��������������Ϊ��"
			<< camera_game_obj_.transform_.translation.x << ", "
			<< camera_game_obj_.transform_.translation.y << ", "
			<< camera_game_obj_.transform_.translation.z << std::endl;

		std::cout << "������������ת�Ƕ�Ϊ��"
			<< camera_game_obj_.transform_.rotation.x << ", "
			<< camera_game_obj_.transform_.rotation.y << ", "
			<< camera_game_obj_.transform_.rotation.z << std::endl;
	}

	void CharacterController::move(float fixed_delta_time, GLFWwindow* glfw_window)
	{
		// TODO: ����Ӧ�ÿ�����������ƶ��ٶȣ��ӽ�ת���ٶ�

		// ����ֻ�����ƶ��������������ת
		keyboard_move_controller_.player_move(glfw_window, fixed_delta_time, character_game_obj_);
		camera_game_obj_.transform_.translation = character_game_obj_.transform_.translation + glm::vec3{ 0, -camera_height_, 0 };

		camera_game_obj_.transform_.scale = { 1, 1, 1 }; // ��������Ų��øı�
	}

	void CharacterController::init_mouse_rotate(GLFWwindow* glfw_window)
	{
		mouse_rotate_controller_.initialize(glfw_window);
	}

	void CharacterController::rotate(float fixed_delta_time, GLFWwindow* glfw_window)
	{
		mouse_rotate_controller_.rotate_control(glfw_window, fixed_delta_time, character_game_obj_, kRotateAll);

		camera_game_obj_.transform_.translation = character_game_obj_.transform_.translation + glm::vec3{ 0, -camera_height_, 0 };
		camera_game_obj_.transform_.rotation = character_game_obj_.transform_.rotation;
		camera_game_obj_.transform_.scale = { 1, 1, 1 }; // ��������Ų��øı�
	}

	void CharacterController::set_player_camera(PlayerCameraView camera_view)
	{
		// ����������۲����
		player_camera_.set_view_yxz(camera_game_obj_.transform_.translation, camera_game_obj_.transform_.rotation);

		// �����������ͼ
		player_camera_.set_perspective_projection
		(
			camera_view.fovy,
			camera_view.aspect,
			camera_view.near,
			camera_view.far
		);
	}

}
