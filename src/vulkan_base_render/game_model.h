
#pragma once

#include "../vulkan_base/game_buffer.h"
#include "../vulkan_base/game_device.h"

// libs
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

// std
#include <memory>
#include <vector>

namespace vulkancraft
{
	class GameModel
	{
	public:

		struct Vertex
		{
			glm::vec3 position{};
			glm::vec3 color{};
			glm::vec3 normal{};
			glm::vec2 uv{};

			static std::vector<VkVertexInputBindingDescription> get_binding_description_vector();
			static std::vector<VkVertexInputAttributeDescription> get_attribute_description_vector();

			bool operator == (const Vertex& other) const
			{
				return position == other.position && color == other.color && normal == other.normal && uv == other.uv;
			}
		};

		struct Builder
		{
			std::vector<Vertex> vertices{};
			std::vector<uint32_t> indices{};

			void load_model(const std::string& file_path);
		};

		GameModel(GameDevice& game_device, const GameModel::Builder& builder);
		~GameModel();

		GameModel(const GameModel&) = delete;
		GameModel& operator = (const GameModel&) = delete;

		static std::unique_ptr<GameModel> create_model_from_file(GameDevice& game_device, const std::string& file_path);

		void bind(VkCommandBuffer command_buffer);
		void draw(VkCommandBuffer command_buffer);

	private:
		void create_vertex_buffers(const std::vector<Vertex>& vertices);
		void create_index_buffers(const std::vector<uint32_t>& indices);

		GameDevice& game_device_;

		std::unique_ptr<GameBuffer> vertex_buffer_;
		uint32_t vertex_count_;

		bool has_index_buffer_ = false;
		std::unique_ptr<GameBuffer> index_buffer_;
		uint32_t index_count_;
	};

}  // namespace vulkancraft
