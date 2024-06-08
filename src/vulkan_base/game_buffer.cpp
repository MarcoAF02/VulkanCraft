
/*
 * Encapsulates a vulkan buffer
 *
 * Initially based off VulkanBuffer by Sascha Willems -
 * https://github.com/SaschaWillems/Vulkan/blob/master/base/VulkanBuffer.h
 */

#include "game_buffer.h"

 // std
#include <cassert>
#include <cstring>

namespace vulkancraft
{
	/**
	 * Returns the minimum instance size required to be compatible with devices minOffsetAlignment
	 *
	 * @param instanceSize The size of an instance
	 * @param minOffsetAlignment The minimum required alignment, in bytes, for the offset member (eg
	 * minUniformBufferOffsetAlignment)
	 *
	 * @return VkResult of the buffer mapping call
	 */

	VkDeviceSize GameBuffer::get_alignment(VkDeviceSize instanceSize, VkDeviceSize minOffsetAlignment)
	{
		if (minOffsetAlignment > 0)
		{
			return (instanceSize + minOffsetAlignment - 1) & ~(minOffsetAlignment - 1);
		}

		return instanceSize;
	}

	GameBuffer::GameBuffer
	(
		GameDevice& device,
		VkDeviceSize instanceSize,
		uint32_t instanceCount,
		VkBufferUsageFlags usageFlags,
		VkMemoryPropertyFlags memoryPropertyFlags,
		VkDeviceSize minOffsetAlignment
	) : game_device_{ device },
		instance_size_{ instanceSize },
		instance_count_{ instanceCount },
		usage_flags_{ usageFlags },
		memory_property_flags_{ memoryPropertyFlags } {
		alignment_size_ = get_alignment(instanceSize, minOffsetAlignment);
		buffer_size_ = alignment_size_ * instanceCount;
		device.create_buffer(buffer_size_, usageFlags, memoryPropertyFlags, buffer_, memory_);
	}

	GameBuffer::~GameBuffer()
	{
		unmap();
		vkDestroyBuffer(game_device_.get_vulkan_device(), buffer_, nullptr);
		vkFreeMemory(game_device_.get_vulkan_device(), memory_, nullptr);
	}

	/**
	 * Map a memory range of this buffer. If successful, mapped points to the specified buffer range.
	 *
	 * @param size (Optional) Size of the memory range to map. Pass VK_WHOLE_SIZE to map the complete
	 * buffer range.
	 * @param offset (Optional) Byte offset from beginning
	 *
	 * @return VkResult of the buffer mapping call
	 */
	VkResult GameBuffer::map(VkDeviceSize size, VkDeviceSize offset)
	{
		assert(buffer_ && memory_ && "Called map on buffer before create");
		return vkMapMemory(game_device_.get_vulkan_device(), memory_, offset, size, 0, &mapped_);
	}

	/**
	 * Unmap a mapped memory range
	 *
	 * @note Does not return a result as vkUnmapMemory can't fail
	 */
	void GameBuffer::unmap()
	{
		if (mapped_)
		{
			vkUnmapMemory(game_device_.get_vulkan_device(), memory_);
			mapped_ = nullptr;
		}
	}

	/**
	 * Copies the specified data to the mapped buffer. Default value writes whole buffer range
	 *
	 * @param data Pointer to the data to copy
	 * @param size (Optional) Size of the data to copy. Pass VK_WHOLE_SIZE to flush the complete buffer
	 * range.
	 * @param offset (Optional) Byte offset from beginning of mapped region
	 *
	 */
	void GameBuffer::write_to_buffer(void* data, VkDeviceSize size, VkDeviceSize offset)
	{
		assert(mapped_ && "Cannot copy to unmapped buffer");

		if (size == VK_WHOLE_SIZE)
		{
			memcpy(mapped_, data, buffer_size_);
		}
		else
		{
			char* memOffset = (char*)mapped_;
			memOffset += offset;
			memcpy(memOffset, data, size);
		}
	}

	/**
	 * Flush a memory range of the buffer to make it visible to the device
	 *
	 * @note Only required for non-coherent memory
	 *
	 * @param size (Optional) Size of the memory range to flush. Pass VK_WHOLE_SIZE to flush the
	 * complete buffer range.
	 * @param offset (Optional) Byte offset from beginning
	 *
	 * @return VkResult of the flush call
	 */
	VkResult GameBuffer::flush(VkDeviceSize size, VkDeviceSize offset)
	{
		VkMappedMemoryRange mappedRange = {};
		mappedRange.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
		mappedRange.memory = memory_;
		mappedRange.offset = offset;
		mappedRange.size = size;
		return vkFlushMappedMemoryRanges(game_device_.get_vulkan_device(), 1, &mappedRange);
	}

	/**
	 * Invalidate a memory range of the buffer to make it visible to the host
	 *
	 * @note Only required for non-coherent memory
	 *
	 * @param size (Optional) Size of the memory range to invalidate. Pass VK_WHOLE_SIZE to invalidate
	 * the complete buffer range.
	 * @param offset (Optional) Byte offset from beginning
	 *
	 * @return VkResult of the invalidate call
	 */
	VkResult GameBuffer::invalidate(VkDeviceSize size, VkDeviceSize offset)
	{
		VkMappedMemoryRange mappedRange = {};
		mappedRange.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
		mappedRange.memory = memory_;
		mappedRange.offset = offset;
		mappedRange.size = size;
		return vkInvalidateMappedMemoryRanges(game_device_.get_vulkan_device(), 1, &mappedRange);
	}

	/**
	 * Create a buffer info descriptor
	 *
	 * @param size (Optional) Size of the memory range of the descriptor
	 * @param offset (Optional) Byte offset from beginning
	 *
	 * @return VkDescriptorBufferInfo of specified offset and range
	 */
	VkDescriptorBufferInfo GameBuffer::descriptor_info(VkDeviceSize size, VkDeviceSize offset)
	{
		return VkDescriptorBufferInfo
		{
			buffer_,
			offset,
			size,
		};
	}

	/**
	 * Copies "instanceSize" bytes of data to the mapped buffer at an offset of index * alignmentSize
	 *
	 * @param data Pointer to the data to copy
	 * @param index Used in offset calculation
	 *
	 */
	void GameBuffer::write_to_index(void* data, int index)
	{
		write_to_buffer(data, instance_size_, index * alignment_size_);
	}

	/**
	 *  Flush the memory range at index * alignmentSize of the buffer to make it visible to the device
	 *
	 * @param index Used in offset calculation
	 *
	 */
	VkResult GameBuffer::flush_index(int index) { return flush(alignment_size_, index * alignment_size_); }

	/**
	 * Create a buffer info descriptor
	 *
	 * @param index Specifies the region given by index * alignmentSize
	 *
	 * @return VkDescriptorBufferInfo for instance at index
	 */
	VkDescriptorBufferInfo GameBuffer::descriptor_info_for_index(int index)
	{
		return descriptor_info(alignment_size_, index * alignment_size_);
	}

	/**
	 * Invalidate a memory range of the buffer to make it visible to the host
	 *
	 * @note Only required for non-coherent memory
	 *
	 * @param index Specifies the region to invalidate: index * alignmentSize
	 *
	 * @return VkResult of the invalidate call
	 */
	VkResult GameBuffer::invalidate_index(int index)
	{
		return invalidate(alignment_size_, index * alignment_size_);
	}

}  // namespace vulkancraft
