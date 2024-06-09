
#include "game_model.h"
#include "game_utils.h"

// libs
#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>

// std
#include <cassert>
#include <cstring>
#include <unordered_map>

namespace std
{
	template <>
	struct hash<vulkancraft::GameModel::Vertex>
	{
		size_t operator()(vulkancraft::GameModel::Vertex const& vertex) const
		{
			size_t seed = 0;
			vulkancraft::hash_combine(seed, vertex.position, vertex.color, vertex.normal, vertex.uv);
			return seed;
		}
	};

}  // namespace std

namespace vulkancraft
{
	GameModel::GameModel(GameDevice& device, const GameModel::Builder& builder) : game_device_{ device }
	{
		create_vertex_buffers(builder.vertices);
		create_index_buffers(builder.indices);
	}

	GameModel::~GameModel() { }

	std::unique_ptr<GameModel> GameModel::create_model_from_file(GameDevice& game_device, const std::string& file_path)
	{
		Builder builder{};

		// HACK: 这里从磁盘加载模型，使用相对路径
		builder.load_model(file_path);

		return std::make_unique<GameModel>(game_device, builder);
	}

	void GameModel::create_vertex_buffers(const std::vector<Vertex>& vertices)
	{
		vertex_count_ = static_cast<uint32_t>(vertices.size());
		assert(vertex_count_ >= 3 && "模型顶点的数量至少要有 3 个");

		VkDeviceSize bufferSize = sizeof(vertices[0]) * vertex_count_;
		uint32_t vertexSize = sizeof(vertices[0]);

		GameBuffer stagingBuffer
		{
			game_device_,
			vertexSize,
			vertex_count_,
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
		};

		stagingBuffer.map();
		stagingBuffer.write_to_buffer((void*)vertices.data());

		vertex_buffer_ = std::make_unique<GameBuffer>
			(
				game_device_,
				vertexSize,
				vertex_count_,
				VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
				VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
			);

		game_device_.copy_buffer(stagingBuffer.get_buffer(), vertex_buffer_->get_buffer(), bufferSize);
	}

	void GameModel::create_index_buffers(const std::vector<uint32_t>& indices)
	{
		index_count_ = static_cast<uint32_t>(indices.size());
		has_index_buffer_ = index_count_ > 0;

		if (!has_index_buffer_) return;

		VkDeviceSize bufferSize = sizeof(indices[0]) * index_count_;
		uint32_t indexSize = sizeof(indices[0]);

		GameBuffer stagingBuffer
		{
			game_device_,
			indexSize,
			index_count_,
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
		};

		stagingBuffer.map();
		stagingBuffer.write_to_buffer((void*)indices.data());

		index_buffer_ = std::make_unique<GameBuffer>
			(
				game_device_,
				indexSize,
				index_count_,
				VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
				VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
			);

		game_device_.copy_buffer(stagingBuffer.get_buffer(), index_buffer_->get_buffer(), bufferSize);
	}

	void GameModel::draw(VkCommandBuffer commandBuffer)
	{
		if (has_index_buffer_)
		{
			vkCmdDrawIndexed(commandBuffer, index_count_, 1, 0, 0, 0);
		}
		else
		{
			vkCmdDraw(commandBuffer, vertex_count_, 1, 0, 0);
		}
	}

	void GameModel::bind(VkCommandBuffer commandBuffer)
	{
		VkBuffer buffers[] = { vertex_buffer_->get_buffer() };
		VkDeviceSize offsets[] = { 0 };

		vkCmdBindVertexBuffers(commandBuffer, 0, 1, buffers, offsets);

		if (has_index_buffer_)
		{
			vkCmdBindIndexBuffer(commandBuffer, index_buffer_->get_buffer(), 0, VK_INDEX_TYPE_UINT32);
		}
	}

	std::vector<VkVertexInputBindingDescription> GameModel::Vertex::get_binding_description_vector()
	{
		std::vector<VkVertexInputBindingDescription> bindingDescriptions(1);
		bindingDescriptions[0].binding = 0;
		bindingDescriptions[0].stride = sizeof(Vertex);
		bindingDescriptions[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
		return bindingDescriptions;
	}

	std::vector<VkVertexInputAttributeDescription> GameModel::Vertex::get_attribute_description_vector()
	{
		std::vector<VkVertexInputAttributeDescription> attributeDescriptions{};

		attributeDescriptions.push_back({ 0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, position) });
		attributeDescriptions.push_back({ 1, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, color) });
		attributeDescriptions.push_back({ 2, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, normal) });
		attributeDescriptions.push_back({ 3, 0, VK_FORMAT_R32G32_SFLOAT, offsetof(Vertex, uv) });

		return attributeDescriptions;
	}

	void GameModel::Builder::load_model(const std::string& filepath)
	{
		tinyobj::attrib_t attrib;
		std::vector<tinyobj::shape_t> shapes;
		std::vector<tinyobj::material_t> materials;
		std::string warn, err;

		if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, filepath.c_str()))
		{
			throw std::runtime_error(warn + err);
		}

		vertices.clear();
		indices.clear();

		std::unordered_map<Vertex, uint32_t> uniqueVertices{};

		for (const auto& shape : shapes)
		{
			for (const auto& index : shape.mesh.indices)
			{
				Vertex vertex{};

				if (index.vertex_index >= 0)
				{
					vertex.position =
					{
						attrib.vertices[3 * index.vertex_index + 0],
						attrib.vertices[3 * index.vertex_index + 1],
						attrib.vertices[3 * index.vertex_index + 2],
					};

					vertex.color =
					{
						attrib.colors[3 * index.vertex_index + 0],
						attrib.colors[3 * index.vertex_index + 1],
						attrib.colors[3 * index.vertex_index + 2],
					};
				}

				if (index.normal_index >= 0)
				{
					vertex.normal =
					{
						attrib.normals[3 * index.normal_index + 0],
						attrib.normals[3 * index.normal_index + 1],
						attrib.normals[3 * index.normal_index + 2],
					};
				}

				if (index.texcoord_index >= 0)
				{
					vertex.uv =
					{
						// attrib.texcoords[2 * index.texcoord_index + 0],
						// attrib.texcoords[2 * index.texcoord_index + 1],

						attrib.texcoords[2 * index.texcoord_index + 0],
						1.0f - attrib.texcoords[2 * index.texcoord_index + 1]
					};
				}

				if (uniqueVertices.count(vertex) == 0)
				{
					uniqueVertices[vertex] = static_cast<uint32_t>(vertices.size());
					vertices.push_back(vertex);
				}

				indices.push_back(uniqueVertices[vertex]);
			}
		}
	}

}  // namespace vulkancraft
