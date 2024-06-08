
#include "game_swap_chain.h"

// std
#include <array>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <limits>
#include <set>
#include <stdexcept>

namespace vulkancraft
{
	GameSwapChain::GameSwapChain(GameDevice& deviceRef, VkExtent2D extent) : game_device_{ deviceRef }, window_extent_{ extent }
	{
		init();
	}

	GameSwapChain::GameSwapChain(GameDevice& deviceRef, VkExtent2D extent, std::shared_ptr<GameSwapChain> previous) : game_device_{ deviceRef }, window_extent_{ extent }, old_swap_chain_{ previous }
	{
		init();
		old_swap_chain_ = nullptr;
	}

	void GameSwapChain::init()
	{
		create_swap_chain();
		create_image_views();
		create_render_pass();
		create_depth_resources();
		create_frame_buffers();
		create_sync_objects();
	}

	GameSwapChain::~GameSwapChain()
	{
		for (auto imageView : swap_chain_image_view_vector_)
		{
			vkDestroyImageView(game_device_.get_vulkan_device(), imageView, nullptr);
		}

		swap_chain_image_view_vector_.clear();

		if (swap_chain_ != nullptr)
		{
			vkDestroySwapchainKHR(game_device_.get_vulkan_device(), swap_chain_, nullptr);
			swap_chain_ = nullptr;
		}

		for (int i = 0; i < depth_image_vector_.size(); i++)
		{
			vkDestroyImageView(game_device_.get_vulkan_device(), depth_image_view_vector_[i], nullptr);
			vkDestroyImage(game_device_.get_vulkan_device(), depth_image_vector_[i], nullptr);
			vkFreeMemory(game_device_.get_vulkan_device(), depth_image_memory_vector_[i], nullptr);
		}

		for (auto framebuffer : swap_chain_frame_buffer_vector_)
		{
			vkDestroyFramebuffer(game_device_.get_vulkan_device(), framebuffer, nullptr);
		}

		vkDestroyRenderPass(game_device_.get_vulkan_device(), render_pass_, nullptr);

		// cleanup synchronization objects
		for (size_t i = 0; i < kMaxFramesInFlight; i++)
		{
			vkDestroySemaphore(game_device_.get_vulkan_device(), render_finished_semaphore_vector_[i], nullptr);
			vkDestroySemaphore(game_device_.get_vulkan_device(), image_available_semaphore_vector_[i], nullptr);
			vkDestroyFence(game_device_.get_vulkan_device(), in_flight_fence_vector_[i], nullptr);
		}
	}

	VkResult GameSwapChain::acquire_next_image(uint32_t* imageIndex)
	{
		vkWaitForFences
		(
			game_device_.get_vulkan_device(),
			1,
			&in_flight_fence_vector_[current_frame_],
			VK_TRUE,
			std::numeric_limits<uint64_t>::max()
		);

		VkResult result = vkAcquireNextImageKHR
		(
			game_device_.get_vulkan_device(),
			swap_chain_,
			std::numeric_limits<uint64_t>::max(),
			image_available_semaphore_vector_[current_frame_],  // must be a not signaled semaphore
			VK_NULL_HANDLE,
			imageIndex
		);

		return result;
	}

	VkResult GameSwapChain::submit_command_buffers(const VkCommandBuffer* buffers, uint32_t* imageIndex)
	{
		if (images_in_flight_vector_[*imageIndex] != VK_NULL_HANDLE)
		{
			vkWaitForFences(game_device_.get_vulkan_device(), 1, &images_in_flight_vector_[*imageIndex], VK_TRUE, UINT64_MAX);
		}

		images_in_flight_vector_[*imageIndex] = in_flight_fence_vector_[current_frame_];

		VkSubmitInfo submitInfo = {};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

		VkSemaphore waitSemaphores[] = { image_available_semaphore_vector_[current_frame_] };
		VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
		submitInfo.waitSemaphoreCount = 1;
		submitInfo.pWaitSemaphores = waitSemaphores;
		submitInfo.pWaitDstStageMask = waitStages;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = buffers;

		VkSemaphore signalSemaphores[] = { render_finished_semaphore_vector_[current_frame_] };
		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = signalSemaphores;

		vkResetFences(game_device_.get_vulkan_device(), 1, &in_flight_fence_vector_[current_frame_]);

		if (vkQueueSubmit(game_device_.graphics_queue(), 1, &submitInfo, in_flight_fence_vector_[current_frame_]) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to submit draw command buffer!");
		}

		VkPresentInfoKHR presentInfo = {};
		presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
		presentInfo.waitSemaphoreCount = 1;
		presentInfo.pWaitSemaphores = signalSemaphores;

		VkSwapchainKHR swapChains[] = { swap_chain_ };
		presentInfo.swapchainCount = 1;
		presentInfo.pSwapchains = swapChains;
		presentInfo.pImageIndices = imageIndex;

		auto result = vkQueuePresentKHR(game_device_.present_queue(), &presentInfo);

		current_frame_ = (current_frame_ + 1) % kMaxFramesInFlight;

		return result;
	}

	void GameSwapChain::create_swap_chain()
	{
		SwapChainSupportDetails swapChainSupport = game_device_.get_swap_chain_support();

		VkSurfaceFormatKHR surfaceFormat = choose_swap_surface_format(swapChainSupport.formats_vector);
		VkPresentModeKHR presentMode = choose_swap_present_mode(swapChainSupport.present_mode_vector);
		VkExtent2D extent = choose_swap_extent(swapChainSupport.capabilities);

		uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;

		if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount)
		{
			imageCount = swapChainSupport.capabilities.maxImageCount;
		}

		VkSwapchainCreateInfoKHR createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		createInfo.surface = game_device_.surface();

		createInfo.minImageCount = imageCount;
		createInfo.imageFormat = surfaceFormat.format;
		createInfo.imageColorSpace = surfaceFormat.colorSpace;
		createInfo.imageExtent = extent;
		createInfo.imageArrayLayers = 1;
		createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

		QueueFamilyIndices indices = game_device_.find_physical_queue_families();
		uint32_t queueFamilyIndices[] = { indices.graphics_family, indices.present_family };

		if (indices.graphics_family != indices.present_family)
		{
			createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
			createInfo.queueFamilyIndexCount = 2;
			createInfo.pQueueFamilyIndices = queueFamilyIndices;
		}
		else
		{
			createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
			createInfo.queueFamilyIndexCount = 0;      // Optional
			createInfo.pQueueFamilyIndices = nullptr;  // Optional
		}

		createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
		createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;

		createInfo.presentMode = presentMode;
		createInfo.clipped = VK_TRUE;

		createInfo.oldSwapchain = old_swap_chain_ == nullptr ? VK_NULL_HANDLE : old_swap_chain_->swap_chain_;

		if (vkCreateSwapchainKHR(game_device_.get_vulkan_device(), &createInfo, nullptr, &swap_chain_) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create swap chain!");
		}

		// we only specified a minimum number of images in the swap chain, so the implementation is
		// allowed to create a swap chain with more. That's why we'll first query the final number of
		// images with vkGetSwapchainImagesKHR, then resize the container and finally call it again to
		// retrieve the handles.
		vkGetSwapchainImagesKHR(game_device_.get_vulkan_device(), swap_chain_, &imageCount, nullptr);
		swap_chain_image_vector_.resize(imageCount);
		vkGetSwapchainImagesKHR(game_device_.get_vulkan_device(), swap_chain_, &imageCount, swap_chain_image_vector_.data());

		swap_chain_image_format_ = surfaceFormat.format;
		swap_chain_extent_ = extent;
	}

	void GameSwapChain::create_image_views()
	{
		swap_chain_image_view_vector_.resize(swap_chain_image_vector_.size());

		for (size_t i = 0; i < swap_chain_image_vector_.size(); i++)
		{
			VkImageViewCreateInfo viewInfo{};
			viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			viewInfo.image = swap_chain_image_vector_[i];
			viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
			viewInfo.format = swap_chain_image_format_;
			viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			viewInfo.subresourceRange.baseMipLevel = 0;
			viewInfo.subresourceRange.levelCount = 1;
			viewInfo.subresourceRange.baseArrayLayer = 0;
			viewInfo.subresourceRange.layerCount = 1;

			if (vkCreateImageView(game_device_.get_vulkan_device(), &viewInfo, nullptr, &swap_chain_image_view_vector_[i]) != VK_SUCCESS)
			{
				throw std::runtime_error("failed to create texture image view!");
			}
		}
	}

	void GameSwapChain::create_render_pass()
	{
		VkAttachmentDescription depthAttachment{};
		depthAttachment.format = find_depth_format();
		depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
		depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

		VkAttachmentReference depthAttachmentRef{};
		depthAttachmentRef.attachment = 1;
		depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

		VkAttachmentDescription colorAttachment = {};
		colorAttachment.format = get_swap_chain_image_format();
		colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
		colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

		VkAttachmentReference colorAttachmentRef = {};
		colorAttachmentRef.attachment = 0;
		colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		VkSubpassDescription subpass = {};
		subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		subpass.colorAttachmentCount = 1;
		subpass.pColorAttachments = &colorAttachmentRef;
		subpass.pDepthStencilAttachment = &depthAttachmentRef;

		VkSubpassDependency dependency = {};
		dependency.dstSubpass = 0;
		dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
		dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
		dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
		dependency.srcAccessMask = 0;
		dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;

		std::array<VkAttachmentDescription, 2> attachments = { colorAttachment, depthAttachment };
		VkRenderPassCreateInfo renderPassInfo = {};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
		renderPassInfo.pAttachments = attachments.data();
		renderPassInfo.subpassCount = 1;
		renderPassInfo.pSubpasses = &subpass;
		renderPassInfo.dependencyCount = 1;
		renderPassInfo.pDependencies = &dependency;

		if (vkCreateRenderPass(game_device_.get_vulkan_device(), &renderPassInfo, nullptr, &render_pass_) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create render pass!");
		}
	}

	void GameSwapChain::create_frame_buffers()
	{
		swap_chain_frame_buffer_vector_.resize(image_count());

		for (size_t i = 0; i < image_count(); i++)
		{
			std::array<VkImageView, 2> attachments = { swap_chain_image_view_vector_[i], depth_image_view_vector_[i] };

			VkExtent2D swapChainExtent = get_swap_chain_extent();
			VkFramebufferCreateInfo framebufferInfo = {};
			framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
			framebufferInfo.renderPass = render_pass_;
			framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
			framebufferInfo.pAttachments = attachments.data();
			framebufferInfo.width = swapChainExtent.width;
			framebufferInfo.height = swapChainExtent.height;
			framebufferInfo.layers = 1;

			if (vkCreateFramebuffer
			(
				game_device_.get_vulkan_device(),
				&framebufferInfo,
				nullptr,
				&swap_chain_frame_buffer_vector_[i]
			) != VK_SUCCESS)
			{
				throw std::runtime_error("failed to create framebuffer!");
			}
		}
	}

	void GameSwapChain::create_depth_resources()
	{
		VkFormat depthFormat = find_depth_format();
		swap_chain_depth_format_ = depthFormat;
		VkExtent2D swapChainExtent = get_swap_chain_extent();

		depth_image_vector_.resize(image_count());
		depth_image_memory_vector_.resize(image_count());
		depth_image_view_vector_.resize(image_count());

		for (int i = 0; i < depth_image_vector_.size(); i++)
		{
			VkImageCreateInfo imageInfo{};
			imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
			imageInfo.imageType = VK_IMAGE_TYPE_2D;
			imageInfo.extent.width = swapChainExtent.width;
			imageInfo.extent.height = swapChainExtent.height;
			imageInfo.extent.depth = 1;
			imageInfo.mipLevels = 1;
			imageInfo.arrayLayers = 1;
			imageInfo.format = depthFormat;
			imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
			imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			imageInfo.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
			imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
			imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
			imageInfo.flags = 0;

			game_device_.create_image_with_info
			(
				imageInfo,
				VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
				depth_image_vector_[i],
				depth_image_memory_vector_[i]
			);

			VkImageViewCreateInfo viewInfo{};
			viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			viewInfo.image = depth_image_vector_[i];
			viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
			viewInfo.format = depthFormat;
			viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
			viewInfo.subresourceRange.baseMipLevel = 0;
			viewInfo.subresourceRange.levelCount = 1;
			viewInfo.subresourceRange.baseArrayLayer = 0;
			viewInfo.subresourceRange.layerCount = 1;

			if (vkCreateImageView(game_device_.get_vulkan_device(), &viewInfo, nullptr, &depth_image_view_vector_[i]) != VK_SUCCESS)
			{
				throw std::runtime_error("failed to create texture image view!");
			}
		}
	}

	void GameSwapChain::create_sync_objects()
	{
		image_available_semaphore_vector_.resize(kMaxFramesInFlight);
		render_finished_semaphore_vector_.resize(kMaxFramesInFlight);
		in_flight_fence_vector_.resize(kMaxFramesInFlight);
		images_in_flight_vector_.resize(image_count(), VK_NULL_HANDLE);

		VkSemaphoreCreateInfo semaphoreInfo = {};
		semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

		VkFenceCreateInfo fenceInfo = {};
		fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

		for (size_t i = 0; i < kMaxFramesInFlight; i++)
		{
			if (vkCreateSemaphore(game_device_.get_vulkan_device(), &semaphoreInfo, nullptr, &image_available_semaphore_vector_[i]) != VK_SUCCESS ||
				vkCreateSemaphore(game_device_.get_vulkan_device(), &semaphoreInfo, nullptr, &render_finished_semaphore_vector_[i]) != VK_SUCCESS ||
				vkCreateFence(game_device_.get_vulkan_device(), &fenceInfo, nullptr, &in_flight_fence_vector_[i]) != VK_SUCCESS)
			{
				throw std::runtime_error("failed to create synchronization objects for a frame!");
			}
		}
	}

	VkSurfaceFormatKHR GameSwapChain::choose_swap_surface_format(const std::vector<VkSurfaceFormatKHR>& availableFormats)
	{
		for (const auto& availableFormat : availableFormats)
		{
			if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
			{
				return availableFormat;
			}
		}

		return availableFormats[0];
	}

	VkPresentModeKHR GameSwapChain::choose_swap_present_mode(const std::vector<VkPresentModeKHR>& availablePresentModes)
	{
		// HACK: 注释掉这下面这两段启用或关闭垂直同步
		// HACK: 显卡支持哪一种就会在前面两种显示模式之间选择一个
		for (const auto& availablePresentMode : availablePresentModes)
		{
			if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR)
			{
				std::cout << "显示模式：Mailbox" << std::endl;
				return availablePresentMode;
			}
		}

		for (const auto& availablePresentMode : availablePresentModes)
		{
			if (availablePresentMode == VK_PRESENT_MODE_IMMEDIATE_KHR)
			{
				std::cout << "显示模式：Immediate" << std::endl;
				return availablePresentMode;
			}
		}

		std::cout << "显示模式：V-Sync" << std::endl;
		return VK_PRESENT_MODE_FIFO_KHR;
	}

	VkExtent2D GameSwapChain::choose_swap_extent(const VkSurfaceCapabilitiesKHR& capabilities)
	{
		if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
		{
			return capabilities.currentExtent;
		}
		else
		{
			VkExtent2D actualExtent = window_extent_;
			actualExtent.width = std::max
			(
				capabilities.minImageExtent.width,
				std::min(capabilities.maxImageExtent.width, actualExtent.width)
			);

			actualExtent.height = std::max
			(
				capabilities.minImageExtent.height,
				std::min(capabilities.maxImageExtent.height, actualExtent.height)
			);

			return actualExtent;
		}
	}

	VkFormat GameSwapChain::find_depth_format()
	{
		return game_device_.find_supported_format
		(
			{ VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT },
			VK_IMAGE_TILING_OPTIMAL,
			VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT
		);
	}

}  // namespace vulkancraft
