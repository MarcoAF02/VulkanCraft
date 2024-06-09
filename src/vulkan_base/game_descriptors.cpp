
#include "game_descriptors.h"

// std
#include <cassert>
#include <stdexcept>

namespace vulkancraft
{
	// *************** Descriptor Set Layout Builder *********************

	VulkanBaseDescriptorSetLayout::Builder& VulkanBaseDescriptorSetLayout::Builder::add_binding
	(
		uint32_t binding,
		VkDescriptorType descriptorType,
		VkShaderStageFlags stageFlags,
		uint32_t count
	)
	{
		assert(bindings_map_.count(binding) == 0 && "Binding already in use");
		VkDescriptorSetLayoutBinding layoutBinding{};
		layoutBinding.binding = binding;
		layoutBinding.descriptorType = descriptorType;
		layoutBinding.descriptorCount = count;
		layoutBinding.stageFlags = stageFlags;
		bindings_map_[binding] = layoutBinding;
		return *this;
	}

	std::unique_ptr<VulkanBaseDescriptorSetLayout> VulkanBaseDescriptorSetLayout::Builder::build() const
	{
		return std::make_unique<VulkanBaseDescriptorSetLayout>(game_device_, bindings_map_);
	}

	// *************** Descriptor Set Layout *********************

	VulkanBaseDescriptorSetLayout::VulkanBaseDescriptorSetLayout
	(
		GameDevice& gameDevice, std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings
	) : game_device_{ gameDevice }, bindings_{ bindings }
	{

		std::vector<VkDescriptorSetLayoutBinding> setLayoutBindings{};

		// HACK: 这里修改过，原版不是引用
		for (auto& kv : bindings)
		{
			setLayoutBindings.push_back(kv.second);
		}

		VkDescriptorSetLayoutCreateInfo descriptorSetLayoutInfo{};
		descriptorSetLayoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		descriptorSetLayoutInfo.bindingCount = static_cast<uint32_t>(setLayoutBindings.size());
		descriptorSetLayoutInfo.pBindings = setLayoutBindings.data();

		if (vkCreateDescriptorSetLayout
		(
			gameDevice.get_vulkan_device(),
			&descriptorSetLayoutInfo,
			nullptr,
			&descriptor_set_layout_
		) != VK_SUCCESS
			)
		{
			throw std::runtime_error("failed to create descriptor set layout!");
		}
	}

	VulkanBaseDescriptorSetLayout::~VulkanBaseDescriptorSetLayout()
	{
		vkDestroyDescriptorSetLayout(game_device_.get_vulkan_device(), descriptor_set_layout_, nullptr);
	}

	// *************** Descriptor Pool Builder *********************

	VulkanBaseDescriptorPool::Builder& VulkanBaseDescriptorPool::Builder::add_pool_size
	(
		VkDescriptorType descriptorType, uint32_t count) {
		pool_size_vector_.push_back({ descriptorType, count });
		return *this;
	}

	VulkanBaseDescriptorPool::Builder& VulkanBaseDescriptorPool::Builder::set_pool_flags(VkDescriptorPoolCreateFlags flags)
	{
		pool_flags_ = flags;
		return *this;
	}

	VulkanBaseDescriptorPool::Builder& VulkanBaseDescriptorPool::Builder::set_max_sets(uint32_t count)
	{
		max_sets_ = count;
		return *this;
	}

	std::unique_ptr<VulkanBaseDescriptorPool> VulkanBaseDescriptorPool::Builder::build() const
	{
		return std::make_unique<VulkanBaseDescriptorPool>(game_device_, max_sets_, pool_flags_, pool_size_vector_);
	}

	// *************** Descriptor Pool *********************

	VulkanBaseDescriptorPool::VulkanBaseDescriptorPool
	(
		GameDevice& gameDevice,
		uint32_t maxSets,
		VkDescriptorPoolCreateFlags poolFlags,
		const std::vector<VkDescriptorPoolSize>& poolSizes
	) : game_device_{ gameDevice }
	{
		VkDescriptorPoolCreateInfo descriptorPoolInfo{};
		descriptorPoolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		descriptorPoolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
		descriptorPoolInfo.pPoolSizes = poolSizes.data();
		descriptorPoolInfo.maxSets = maxSets;
		descriptorPoolInfo.flags = poolFlags;

		if (vkCreateDescriptorPool(gameDevice.get_vulkan_device(), &descriptorPoolInfo, nullptr, &descriptor_pool_) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create descriptor pool!");
		}
	}

	VulkanBaseDescriptorPool::~VulkanBaseDescriptorPool()
	{
		vkDestroyDescriptorPool(game_device_.get_vulkan_device(), descriptor_pool_, nullptr);
	}

	bool VulkanBaseDescriptorPool::allocate_descriptor(const VkDescriptorSetLayout descriptorSetLayout, VkDescriptorSet& descriptor) const
	{
		VkDescriptorSetAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		allocInfo.descriptorPool = descriptor_pool_;
		allocInfo.pSetLayouts = &descriptorSetLayout;
		allocInfo.descriptorSetCount = 1;

		// Might want to create a "DescriptorPoolManager" class that handles this case, and builds
		// a new pool whenever an old pool fills up. But this is beyond our current scope
		if (vkAllocateDescriptorSets(game_device_.get_vulkan_device(), &allocInfo, &descriptor) != VK_SUCCESS)
		{
			return false;
		}

		return true;
	}

	void VulkanBaseDescriptorPool::free_descriptors(std::vector<VkDescriptorSet>& descriptors) const
	{
		vkFreeDescriptorSets
		(
			game_device_.get_vulkan_device(),
			descriptor_pool_,
			static_cast<uint32_t>(descriptors.size()),
			descriptors.data()
		);
	}

	void VulkanBaseDescriptorPool::reset_pool()
	{
		vkResetDescriptorPool(game_device_.get_vulkan_device(), descriptor_pool_, 0);
	}

	// *************** Descriptor Writer *********************

	VulkanBaseDescriptorWriter::VulkanBaseDescriptorWriter(VulkanBaseDescriptorSetLayout& setLayout, VulkanBaseDescriptorPool& pool) : set_layout_{ setLayout }, pool_{ pool } {}

	VulkanBaseDescriptorWriter& VulkanBaseDescriptorWriter::write_buffer(uint32_t binding, VkDescriptorBufferInfo* bufferInfo)
	{
		assert(set_layout_.bindings_.count(binding) == 1 && "Layout does not contain specified binding");
		auto& bindingDescription = set_layout_.bindings_[binding];
		assert(bindingDescription.descriptorCount == 1 && "Binding single descriptor info, but binding expects multiple");

		VkWriteDescriptorSet write{};
		write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		write.descriptorType = bindingDescription.descriptorType;
		write.dstBinding = binding;
		write.pBufferInfo = bufferInfo;
		write.descriptorCount = 1;

		write_vector_.push_back(write);
		return *this;
	}

	VulkanBaseDescriptorWriter& VulkanBaseDescriptorWriter::write_image(uint32_t binding, VkDescriptorImageInfo* imageInfo)
	{
		assert(set_layout_.bindings_.count(binding) == 1 && "Layout does not contain specified binding");
		auto& bindingDescription = set_layout_.bindings_[binding];
		assert(bindingDescription.descriptorCount == 1 && "Binding single descriptor info, but binding expects multiple");

		VkWriteDescriptorSet write{};
		write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		write.descriptorType = bindingDescription.descriptorType;
		write.dstBinding = binding;
		write.pImageInfo = imageInfo;
		write.descriptorCount = 1;

		write_vector_.push_back(write);
		return *this;
	}

	bool VulkanBaseDescriptorWriter::build(VkDescriptorSet& set)
	{
		bool success = pool_.allocate_descriptor(set_layout_.get_descriptor_set_layout(), set);

		if (!success)
		{
			return false;
		}

		overwrite(set);
		return true;
	}

	void VulkanBaseDescriptorWriter::overwrite(VkDescriptorSet& set)
	{
		for (auto& write : write_vector_)
		{
			write.dstSet = set;
		}

		vkUpdateDescriptorSets(pool_.game_device_.get_vulkan_device(), static_cast<uint32_t>(write_vector_.size()), write_vector_.data(), 0, nullptr);
	}

}  // namespace vulkancraft
