
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

		glm::mat3 normalMatrix();
	};

	struct PointLightComponent
	{
		float light_intensity = 1.0f;
	};

	class BaseGameObject
	{
	public:

		using id_t = unsigned int;
		using Map = std::unordered_map<id_t, BaseGameObject>;

		static BaseGameObject create_game_object()
		{
			static id_t current_id = 0;
			return BaseGameObject{ current_id++ };
		}

		static BaseGameObject make_point_light(float intensity = 10.f, float radius = 0.1f, glm::vec3 color = glm::vec3(1.f));

		BaseGameObject(const BaseGameObject&) = delete;
		BaseGameObject& operator = (const BaseGameObject&) = delete;
		BaseGameObject(BaseGameObject&&) = default;
		BaseGameObject& operator = (BaseGameObject&&) = default;

		id_t get_id() { return id_; }

		glm::vec3 color_{};
		TransformComponent transform_{};

		// Optional pointer components
		std::shared_ptr<GameModel> model_{};
		std::unique_ptr<PointLightComponent> point_light_ = nullptr;

	private:

		BaseGameObject(id_t obj_id) : id_{ obj_id } {}
		id_t id_;
	};

}  // namespace vulkancraft
