
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

	// 游戏对象的公共数据记录的不多，节省内存
	struct GameObjectPublicData
	{
		id_t id_pub_;
		bool is_static_;
		TransformComponent transform_pub_;
	};

	// 这个 map 和渲染系统里的 map 一致
	using PublicObjMap = std::unordered_map<id_t, GameObjectPublicData>;
}
