
#pragma once

#include "game_device.h"

// std
#include <memory>
#include <unordered_map>
#include <vector>

namespace vulkancraft
{
	class VulkanBaseDescriptorSetLayout
	{
	public:
		class Builder
		{
		public:

			Builder(GameDevice& lveDevice) : game_device_{ lveDevice } {}

			Builder& add_binding
			(
				uint32_t binding,
				VkDescriptorType descriptor_type,
				VkShaderStageFlags stage_flags,
				uint32_t count = 1
			);

			std::unique_ptr<VulkanBaseDescriptorSetLayout> build() const;

		private:

			GameDevice& game_device_;
			std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings_map_{};
		};

		VulkanBaseDescriptorSetLayout(GameDevice& lveDevice, std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings);
		~VulkanBaseDescriptorSetLayout();

		VulkanBaseDescriptorSetLayout(const VulkanBaseDescriptorSetLayout&) = delete;
		VulkanBaseDescriptorSetLayout& operator=(const VulkanBaseDescriptorSetLayout&) = delete;

		VkDescriptorSetLayout get_descriptor_set_layout() const { return descriptor_set_layout_; }

	private:
		GameDevice& game_device_;
		VkDescriptorSetLayout descriptor_set_layout_;
		std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings_;

		friend class VulkanBaseDescriptorWriter;
	};

	class VulkanBaseDescriptorPool
	{
	public:
		class Builder
		{
		public:
			Builder(GameDevice& lveDevice) : game_device_{ lveDevice } {}

			Builder& add_pool_size(VkDescriptorType descriptor_type, uint32_t count);
			Builder& set_pool_flags(VkDescriptorPoolCreateFlags flags);
			Builder& set_max_sets(uint32_t count);
			std::unique_ptr<VulkanBaseDescriptorPool> build() const;

		private:
			GameDevice& game_device_;
			std::vector<VkDescriptorPoolSize> pool_size_vector_{};
			uint32_t max_sets_ = 1000;
			VkDescriptorPoolCreateFlags pool_flags_ = 0;
		};

		VulkanBaseDescriptorPool
		(
			GameDevice& game_device,
			uint32_t max_sets,
			VkDescriptorPoolCreateFlags pool_flags,
			const std::vector<VkDescriptorPoolSize>& pool_size_vector
		);

		~VulkanBaseDescriptorPool();
		VulkanBaseDescriptorPool(const VulkanBaseDescriptorPool&) = delete;
		VulkanBaseDescriptorPool& operator = (const VulkanBaseDescriptorPool&) = delete;

		bool allocate_descriptor(const VkDescriptorSetLayout descriptorSetLayout, VkDescriptorSet& descriptor) const;

		void free_descriptors(std::vector<VkDescriptorSet>& descriptors) const;
		void reset_pool();

	private:
		GameDevice& game_device_;
		VkDescriptorPool descriptor_pool_;

		friend class VulkanBaseDescriptorWriter;
	};

	class VulkanBaseDescriptorWriter
	{
	public:
		VulkanBaseDescriptorWriter(VulkanBaseDescriptorSetLayout& set_layout, VulkanBaseDescriptorPool& pool);

		VulkanBaseDescriptorWriter& write_buffer(uint32_t binding, VkDescriptorBufferInfo* buffer_info);
		VulkanBaseDescriptorWriter& write_image(uint32_t binding, VkDescriptorImageInfo* image_info);

		bool build(VkDescriptorSet& set);
		void overwrite(VkDescriptorSet& set);

	private:
		VulkanBaseDescriptorSetLayout& set_layout_;
		VulkanBaseDescriptorPool& pool_;
		std::vector<VkWriteDescriptorSet> write_vector_;
	};

}  // namespace vulkancraft
