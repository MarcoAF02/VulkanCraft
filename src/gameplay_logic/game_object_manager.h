
#pragma once

#include "base_game_object.h"

// libs
#include <glm/gtc/matrix_transform.hpp>

// std
#include <memory>
#include <unordered_map>

class vulkancraft::BaseGameObject;

namespace vulkancraft
{
	using id_t = unsigned _int64;

	// ��Ϸ����Ĺ������ݼ�¼�Ĳ��࣬��ʡ�ڴ�
	struct GameObjectPublicData
	{
		id_t id_pub_;
		bool is_static_;
		TransformComponent transform_pub_;
	};

	// ��� map ����Ⱦϵͳ��� map һ��
	using PublicObjMap = std::unordered_map<id_t, GameObjectPublicData>;
}
