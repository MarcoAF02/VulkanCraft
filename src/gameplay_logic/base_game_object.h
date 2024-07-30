
#pragma once

#include "../vulkan_base_render/game_model.h"

// libs
#include <glm/gtc/matrix_transform.hpp>

// std
#include <memory>
#include <unordered_map>

namespace vulkancraft
{
	struct TransformComponent
	{
		glm::vec3 translation{};
		glm::vec3 scale{ 1.f, 1.f, 1.f };
		glm::vec3 rotation{};

		// Matrix corrsponds to Translate * Ry * Rx * Rz * Scale
		// Rotations correspond to Tait-bryan angles of Y(1), X(2), Z(3)
		// https://en.wikipedia.org/wiki/Euler_angles#Rotation_matrix
		glm::mat4 mat4();

		glm::mat3 normal_matrix();
	};

	struct PointLightComponent
	{
		float light_intensity = 1.0f;
	};

	class BaseGameObject
	{
	public:
		using id_t = unsigned _int64;
		using RenderAppObjMap = std::unordered_map<id_t, BaseGameObject>;

		// HACK: 玩家角色直接硬编码为 ID 2560
		// TODO: ID 改为用 String 对象计数
		static BaseGameObject create_game_object(bool is_static)
		{
			static unsigned __int64 current_id = 0;

			// 检查并跳过数字 2560
			if (current_id == 2560) current_id = 2561; // 直接设置为2561
			else ++current_id; // 通常情况下递增

			return BaseGameObject{ current_id, is_static };
		}

		static BaseGameObject make_point_light(float intensity = 10.0f, float radius = 0.1f, glm::vec3 color = glm::vec3(1.f));

		BaseGameObject() = default; // 默认构造函数
		BaseGameObject(const BaseGameObject&) = delete;
		BaseGameObject& operator = (const BaseGameObject&) = delete;
		BaseGameObject(BaseGameObject&&) = default;
		BaseGameObject& operator = (BaseGameObject&&) = default;

		id_t& get_id() { return id_; }
		bool& get_static_state() { return is_static_; }

		// 直接设置新 ID
		void directly_set_id(id_t new_id) { id_ = new_id; }

		glm::vec3 color_{};
		TransformComponent transform_{};

		// Optional pointer components
		std::shared_ptr<GameModel> model_{};
		std::unique_ptr<PointLightComponent> point_light_ = nullptr;

	private:

		bool is_static_ = false; // 该游戏对象是否是静态的
		id_t id_ = 0;
		BaseGameObject(id_t obj_id, bool is_static) : id_{ obj_id } , is_static_{ is_static } {}
	};

}  // namespace vulkancraft
