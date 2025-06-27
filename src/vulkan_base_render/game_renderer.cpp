
#include "game_renderer.h"

// std
#include <array>
#include <cassert>
#include <stdexcept>

namespace vulkancraft
{
	GameRenderer::GameRenderer(GameWindow& window, GameDevice& device) : game_window_{ window }, game_device_{ device }
	{
		recreate_swap_chain();
		create_command_buffers();
	}

	GameRenderer::~GameRenderer() { free_command_buffers(); }

	void GameRenderer::recreate_swap_chain()
	{
		VkExtent2D extent = game_window_.get_extent();

		// 处理窗口大小为 0 的情况
		while (extent.width == 0 || extent.height == 0)
		{
			extent = game_window_.get_extent();
			glfwWaitEvents();
		}

		vkDeviceWaitIdle(game_device_.get_vulkan_device());

		if (game_swap_chain_ == nullptr)
		{
			game_swap_chain_ = std::make_unique<GameSwapChain>(game_device_, extent);
		}
		else
		{
			std::shared_ptr<GameSwapChain> oldSwapChain = std::move(game_swap_chain_);
			game_swap_chain_ = std::make_unique<GameSwapChain>(game_device_, extent, oldSwapChain);

			if (!oldSwapChain->compare_swap_formats(*game_swap_chain_.get()))
			{
				throw std::runtime_error("Swap chain image(or depth) format has changed!");
			}
		}
	}

	void GameRenderer::create_command_buffers()
	{
		command_buffer_vector_.resize(GameSwapChain::static_max_frames_in_flight_);

		VkCommandBufferAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandPool = game_device_.get_command_pool();
		allocInfo.commandBufferCount = static_cast<uint32_t>(command_buffer_vector_.size());

		if (vkAllocateCommandBuffers(game_device_.get_vulkan_device(), &allocInfo, command_buffer_vector_.data()) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to allocate command buffers!");
		}
	}

	void GameRenderer::free_command_buffers()
	{
		vkFreeCommandBuffers
		(
			game_device_.get_vulkan_device(),
			game_device_.get_command_pool(),
			static_cast<uint32_t>(command_buffer_vector_.size()),
			command_buffer_vector_.data()
		);

		command_buffer_vector_.clear();
	}

	VkCommandBuffer GameRenderer::begin_frame()
	{
		assert(!is_frame_started_ && "Can't call beginFrame while already in progress");
		auto result = game_swap_chain_->acquire_next_image(&current_image_index_);

		if (result == VK_ERROR_OUT_OF_DATE_KHR)
		{
			recreate_swap_chain();
			return nullptr;
		}

		if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
		{
			throw std::runtime_error("failed to acquire swap chain image!");
		}

		is_frame_started_ = true;
		auto commandBuffer = get_current_command_buffer();
		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

		if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to begin recording command buffer!");
		}

		return commandBuffer;
	}

	void GameRenderer::end_frame()
	{
		assert(is_frame_started_ && "Can't call endFrame while frame is not in progress");
		auto commandBuffer = get_current_command_buffer();

		if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to record command buffer!");
		}

		auto result = game_swap_chain_->submit_command_buffers(&commandBuffer, &current_image_index_);

		if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || game_window_.was_window_resized())
		{
			game_window_.reset_window_resized_flag();
			recreate_swap_chain();
		}
		else if (result != VK_SUCCESS)
		{
			throw std::runtime_error("failed to present swap chain image!");
		}

		is_frame_started_ = false;
		current_frame_index_ = (current_frame_index_ + 1) % GameSwapChain::static_max_frames_in_flight_;
	}

	void GameRenderer::begin_swap_chain_render_pass(VkCommandBuffer commandBuffer)
	{
		assert(is_frame_started_ && "Can't call beginSwapChainRenderPass if frame is not in progress");
		assert(commandBuffer == get_current_command_buffer() && "Can't begin render pass on command buffer from a different frame");

		VkRenderPassBeginInfo renderPassInfo{};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = game_swap_chain_->get_render_pass();
		renderPassInfo.framebuffer = game_swap_chain_->get_frame_buffer(current_image_index_);

		renderPassInfo.renderArea.offset = { 0, 0 };
		renderPassInfo.renderArea.extent = game_swap_chain_->get_swap_chain_extent();

		std::array<VkClearValue, 2> clearValues{};
		clearValues[0].color = { 0.01f, 0.01f, 0.01f, 1.0f };
		clearValues[1].depthStencil = { 1.0f, 0 };
		renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
		renderPassInfo.pClearValues = clearValues.data();

		vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

		VkViewport viewport{};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = static_cast<float>(game_swap_chain_->get_swap_chain_extent().width);
		viewport.height = static_cast<float>(game_swap_chain_->get_swap_chain_extent().height);
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;

		VkRect2D scissor{ {0, 0}, game_swap_chain_->get_swap_chain_extent() };
		vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
		vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
	}

	void GameRenderer::end_swap_chain_render_pass(VkCommandBuffer commandBuffer)
	{
		assert(is_frame_started_ && "Can't call endSwapChainRenderPass if frame is not in progress");
		assert(commandBuffer == get_current_command_buffer() && "Can't end render pass on command buffer from a different frame");
		vkCmdEndRenderPass(commandBuffer);
	}

}  // namespace vulkancraft
