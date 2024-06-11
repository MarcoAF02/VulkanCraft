
#include "game_device.h"

// std headers
#include <cstring>
#include <iostream>
#include <set>
#include <unordered_set>

namespace vulkancraft
{
	// local callback functions
	static VKAPI_ATTR VkBool32 VKAPI_CALL debug_call_back
	(
		VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
		VkDebugUtilsMessageTypeFlagsEXT messageType,
		const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
		void* pUserData
	)
	{
		std::cerr << "验证层发来的错误信息: " << pCallbackData -> pMessage << std::endl;
		return VK_FALSE;
	}

	VkResult create_debug_utils_messenger_Ext
	(
		VkInstance instance,
		const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
		const VkAllocationCallbacks* pAllocator,
		VkDebugUtilsMessengerEXT* pDebugMessenger
	)
	{
		auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr
		(
			instance,
			"vkCreateDebugUtilsMessengerEXT"
		);

		if (func != nullptr)
		{
			return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
		}
		else
		{
			return VK_ERROR_EXTENSION_NOT_PRESENT;
		}
	}

	void destroy_debug_utils_messenger_Ext
	(
		VkInstance instance,
		VkDebugUtilsMessengerEXT debugMessenger,
		const VkAllocationCallbacks* pAllocator
	)
	{
		auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr
		(
			instance,
			"vkDestroyDebugUtilsMessengerEXT"
		);

		if (func != nullptr)
		{
			func(instance, debugMessenger, pAllocator);
		}
	}

	// class member functions
	GameDevice::GameDevice(GameWindow& window) : game_window_{ window }
	{
		try
		{
			create_vulkan_instance();
			setup_debug_messenger();
			create_surface();
			pick_physical_device();
			create_logical_device();
			create_command_pool();

			std::cout << "========================================" << std::endl;
			std::cout << "GameDevice 类初始化成功" << std::endl;
			std::cout << "========================================" << std::endl;
		}
		catch (std::string &e)
		{
			throw std::runtime_error(e + "GameDevice 类初始化失败");
		}
	}

	GameDevice::~GameDevice()
	{
		vkDestroyCommandPool(vulkan_device_, command_pool_, nullptr);
		vkDestroyDevice(vulkan_device_, nullptr);

		if (enable_validation_layers_)
		{
			destroy_debug_utils_messenger_Ext(instance_, debug_messenger_, nullptr);
		}

		vkDestroySurfaceKHR(instance_, surface_, nullptr);
		vkDestroyInstance(instance_, nullptr);
	}

	void GameDevice::create_vulkan_instance()
	{
		if (enable_validation_layers_ && !check_validation_layer_support())
		{
			throw std::runtime_error("已请求验证层，但验证层不可用");
		}

		VkApplicationInfo appInfo = {};
		appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		appInfo.pApplicationName = "VulkanCraft";
		appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
		appInfo.pEngineName = "No Engine";
		appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
		appInfo.apiVersion = VK_API_VERSION_1_0;

		VkInstanceCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		createInfo.pApplicationInfo = &appInfo;

		auto extensions = get_required_extension_vector();
		createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
		createInfo.ppEnabledExtensionNames = extensions.data();

		VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo;

		if (enable_validation_layers_)
		{
			createInfo.enabledLayerCount = static_cast<uint32_t>(validation_layer_vector_.size());
			createInfo.ppEnabledLayerNames = validation_layer_vector_.data();

			populate_debug_messenger_create_info(debugCreateInfo);
			createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugCreateInfo;
		}
		else
		{
			createInfo.enabledLayerCount = 0;
			createInfo.pNext = nullptr;
		}

		if (vkCreateInstance(&createInfo, nullptr, &instance_) != VK_SUCCESS)
		{
			throw std::runtime_error("Vulkan Instance 创建失败");
		}

		has_required_instance_extensions();
	}

	void GameDevice::pick_physical_device()
	{
		uint32_t deviceCount = 0;
		vkEnumeratePhysicalDevices(instance_, &deviceCount, nullptr);

		if (deviceCount == 0)
		{
			throw std::runtime_error("没有找到支持 Vulkan 的显卡");
		}

		std::cout << "找到的显卡数量: " << deviceCount << std::endl;
		std::vector<VkPhysicalDevice> devices(deviceCount);
		vkEnumeratePhysicalDevices(instance_, &deviceCount, devices.data());

		for (const auto& device : devices)
		{
			if (is_device_suitable(device))
			{
				physical_device_ = device;
				break;
			}
		}

		if (physical_device_ == VK_NULL_HANDLE)
		{
			throw std::runtime_error("没有找到合适的显卡");
		}

		vkGetPhysicalDeviceProperties(physical_device_, &properties_);
		std::cout << "显卡的名称为: " << properties_.deviceName << std::endl;
	}

	void GameDevice::create_logical_device()
	{
		QueueFamilyIndices indices = find_queue_families(physical_device_);

		std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
		std::set<uint32_t> uniqueQueueFamilies = { indices.graphics_family, indices.present_family };

		float queuePriority = 1.0f;

		for (uint32_t queueFamily : uniqueQueueFamilies)
		{
			VkDeviceQueueCreateInfo queueCreateInfo = {};
			queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			queueCreateInfo.queueFamilyIndex = queueFamily;
			queueCreateInfo.queueCount = 1;
			queueCreateInfo.pQueuePriorities = &queuePriority;
			queueCreateInfos.push_back(queueCreateInfo);
		}

		VkPhysicalDeviceFeatures deviceFeatures = {};
		deviceFeatures.samplerAnisotropy = VK_TRUE;

		VkDeviceCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

		createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
		createInfo.pQueueCreateInfos = queueCreateInfos.data();

		createInfo.pEnabledFeatures = &deviceFeatures;
		createInfo.enabledExtensionCount = static_cast<uint32_t>(device_extension_vector_.size());
		createInfo.ppEnabledExtensionNames = device_extension_vector_.data();

		// might not really be necessary anymore because device specific validation layers
		// have been deprecated
		if (enable_validation_layers_)
		{
			createInfo.enabledLayerCount = static_cast<uint32_t>(validation_layer_vector_.size());
			createInfo.ppEnabledLayerNames = validation_layer_vector_.data();
		}
		else
		{
			createInfo.enabledLayerCount = 0;
		}

		if (vkCreateDevice(physical_device_, &createInfo, nullptr, &vulkan_device_) != VK_SUCCESS)
		{
			throw std::runtime_error("逻辑设备创建失败");
		}

		vkGetDeviceQueue(vulkan_device_, indices.graphics_family, 0, &graphics_queue_);
		vkGetDeviceQueue(vulkan_device_, indices.present_family, 0, &present_queue_);
	}

	void GameDevice::create_command_pool()
	{
		QueueFamilyIndices queueFamilyIndices = find_physical_queue_families();

		VkCommandPoolCreateInfo poolInfo = {};
		poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		poolInfo.queueFamilyIndex = queueFamilyIndices.graphics_family;
		poolInfo.flags =
			VK_COMMAND_POOL_CREATE_TRANSIENT_BIT | VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

		if (vkCreateCommandPool(vulkan_device_, &poolInfo, nullptr, &command_pool_) != VK_SUCCESS)
		{
			throw std::runtime_error("Command Pool 创建失败");
		}
	}

	void GameDevice::create_surface() { game_window_.create_vulkan_window(instance_, &surface_); }

	bool GameDevice::is_device_suitable(VkPhysicalDevice device)
	{
		QueueFamilyIndices indices = find_queue_families(device);

		bool extensionsSupported = check_device_extension_support(device);
		bool swapChainAdequate = false;

		if (extensionsSupported)
		{
			SwapChainSupportDetails swapChainSupport = query_swap_chain_support(device);
			swapChainAdequate = !swapChainSupport.formats_vector.empty() && !swapChainSupport.present_mode_vector.empty();
		}

		VkPhysicalDeviceFeatures supportedFeatures;
		vkGetPhysicalDeviceFeatures(device, &supportedFeatures);

		return indices.isComplete() && extensionsSupported && swapChainAdequate && supportedFeatures.samplerAnisotropy;
	}

	void GameDevice::populate_debug_messenger_create_info(VkDebugUtilsMessengerCreateInfoEXT& createInfo)
	{
		createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
		createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
		createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
		createInfo.pfnUserCallback = debug_call_back;
		createInfo.pUserData = nullptr;  // Optional
	}

	void GameDevice::setup_debug_messenger()
	{
		if (!enable_validation_layers_) return;
		VkDebugUtilsMessengerCreateInfoEXT createInfo;
		populate_debug_messenger_create_info(createInfo);

		if (create_debug_utils_messenger_Ext(instance_, &createInfo, nullptr, &debug_messenger_) != VK_SUCCESS)
		{
			throw std::runtime_error("Debug Messenger 创建失败");
		}
	}

	bool GameDevice::check_validation_layer_support()
	{
		uint32_t layerCount;
		vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

		std::vector<VkLayerProperties> availableLayers(layerCount);
		vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

		for (const char* layerName : validation_layer_vector_)
		{
			bool layerFound = false;

			for (const auto& layerProperties : availableLayers)
			{
				if (strcmp(layerName, layerProperties.layerName) == 0)
				{
					layerFound = true;
					break;
				}
			}

			if (!layerFound)
			{
				return false;
			}
		}

		return true;
	}

	std::vector<const char*> GameDevice::get_required_extension_vector()
	{
		uint32_t glfw_extension_count = 0;
		const char** glfw_extension_array;
		glfw_extension_array = glfwGetRequiredInstanceExtensions(&glfw_extension_count);

		std::vector<const char*> extensions(glfw_extension_array, glfw_extension_array + glfw_extension_count);

		if (enable_validation_layers_)
		{
			extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
		}

		return extensions;
	}

	void GameDevice::has_required_instance_extensions()
	{
		uint32_t extensionCount = 0;
		vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
		std::vector<VkExtensionProperties> extensions(extensionCount);
		vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());

		std::cout << "可用的扩展为:" << std::endl;
		std::unordered_set<std::string> available;

		for (const auto& extension : extensions)
		{
			std::cout << "\t" << extension.extensionName << std::endl;
			available.insert(extension.extensionName);
		}

		std::cout << "必需的扩展为:" << std::endl;
		auto requiredExtensions = get_required_extension_vector();

		for (const auto& required : requiredExtensions)
		{
			std::cout << "\t" << required << std::endl;

			if (available.find(required) == available.end())
			{
				throw std::runtime_error("缺少必需的扩展");
			}
		}
	}

	bool GameDevice::check_device_extension_support(VkPhysicalDevice device)
	{
		uint32_t extensionCount;
		vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

		std::vector<VkExtensionProperties> availableExtensions(extensionCount);
		vkEnumerateDeviceExtensionProperties
		(
			device,
			nullptr,
			&extensionCount,
			availableExtensions.data()
		);

		std::set<std::string> requiredExtensions(device_extension_vector_.begin(), device_extension_vector_.end());

		for (const auto& extension : availableExtensions)
		{
			requiredExtensions.erase(extension.extensionName);
		}

		return requiredExtensions.empty();
	}

	QueueFamilyIndices GameDevice::find_queue_families(VkPhysicalDevice device)
	{
		QueueFamilyIndices indices;

		uint32_t queueFamilyCount = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

		std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
		vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

		int i = 0;

		for (const auto& queueFamily : queueFamilies)
		{
			if (queueFamily.queueCount > 0 && queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
			{
				indices.graphics_family = i;
				indices.graphics_family_has_value = true;
			}

			VkBool32 presentSupport = false;
			vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface_, &presentSupport);

			if (queueFamily.queueCount > 0 && presentSupport)
			{
				indices.present_family = i;
				indices.present_family_has_value = true;
			}

			if (indices.isComplete())
			{
				break;
			}

			i++;
		}

		return indices;
	}

	SwapChainSupportDetails GameDevice::query_swap_chain_support(VkPhysicalDevice device)
	{
		SwapChainSupportDetails details;
		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface_, &details.capabilities);

		uint32_t formatCount;
		vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface_, &formatCount, nullptr);

		if (formatCount != 0)
		{
			details.formats_vector.resize(formatCount);
			vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface_, &formatCount, details.formats_vector.data());
		}

		uint32_t presentModeCount;
		vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface_, &presentModeCount, nullptr);

		if (presentModeCount != 0)
		{
			details.present_mode_vector.resize(presentModeCount);
			vkGetPhysicalDeviceSurfacePresentModesKHR
			(
				device,
				surface_,
				&presentModeCount,
				details.present_mode_vector.data()
			);
		}

		return details;
	}

	VkFormat GameDevice::find_supported_format
	(
		const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features
	)
	{
		for (VkFormat format : candidates)
		{
			VkFormatProperties props;
			vkGetPhysicalDeviceFormatProperties(physical_device_, format, &props);

			if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features) 
			{
				return format;
			}
			else if (tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features)
			{
				return format;
			}
		}

		throw std::runtime_error("找不到支持的格式");
	}

	uint32_t GameDevice::findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties)
	{
		VkPhysicalDeviceMemoryProperties memProperties;
		vkGetPhysicalDeviceMemoryProperties(physical_device_, &memProperties);

		for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++)
		{
			if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties)
			{
				return i;
			}
		}

		throw std::runtime_error("找不到支持的内存类型");
	}

	void GameDevice::create_buffer
	(
		VkDeviceSize size,
		VkBufferUsageFlags usage,
		VkMemoryPropertyFlags properties,
		VkBuffer& buffer,
		VkDeviceMemory& bufferMemory
	)
	{
		VkBufferCreateInfo bufferInfo{};
		bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bufferInfo.size = size;
		bufferInfo.usage = usage;
		bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		if (vkCreateBuffer(vulkan_device_, &bufferInfo, nullptr, &buffer) != VK_SUCCESS)
		{
			throw std::runtime_error("Vertex Buffer 创建失败");
		}

		VkMemoryRequirements memRequirements;
		vkGetBufferMemoryRequirements(vulkan_device_, buffer, &memRequirements);

		VkMemoryAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocInfo.allocationSize = memRequirements.size;
		allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties);

		if (vkAllocateMemory(vulkan_device_, &allocInfo, nullptr, &bufferMemory) != VK_SUCCESS)
		{
			throw std::runtime_error("Vertex Buffer 内存分配失败");
		}

		vkBindBufferMemory(vulkan_device_, buffer, bufferMemory, 0);
	}

	VkCommandBuffer GameDevice::begin_single_time_commands()
	{
		VkCommandBufferAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandPool = command_pool_;
		allocInfo.commandBufferCount = 1;

		VkCommandBuffer commandBuffer;
		vkAllocateCommandBuffers(vulkan_device_, &allocInfo, &commandBuffer);

		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

		vkBeginCommandBuffer(commandBuffer, &beginInfo);
		return commandBuffer;
	}

	void GameDevice::end_single_time_commands(VkCommandBuffer commandBuffer)
	{
		vkEndCommandBuffer(commandBuffer);

		VkSubmitInfo submitInfo{};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &commandBuffer;

		vkQueueSubmit(graphics_queue_, 1, &submitInfo, VK_NULL_HANDLE);
		vkQueueWaitIdle(graphics_queue_);

		vkFreeCommandBuffers(vulkan_device_, command_pool_, 1, &commandBuffer);
	}

	void GameDevice::copy_buffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size)
	{
		VkCommandBuffer commandBuffer = begin_single_time_commands();

		VkBufferCopy copyRegion{};
		copyRegion.srcOffset = 0;  // Optional
		copyRegion.dstOffset = 0;  // Optional
		copyRegion.size = size;
		vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

		end_single_time_commands(commandBuffer);
	}

	void GameDevice::copy_buffer_to_image
	(
		VkBuffer buffer, VkImage image, uint32_t width, uint32_t height, uint32_t layerCount
	)
	{
		VkCommandBuffer commandBuffer = begin_single_time_commands();

		VkBufferImageCopy region{};
		region.bufferOffset = 0;
		region.bufferRowLength = 0;
		region.bufferImageHeight = 0;

		region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		region.imageSubresource.mipLevel = 0;
		region.imageSubresource.baseArrayLayer = 0;
		region.imageSubresource.layerCount = layerCount;

		region.imageOffset = { 0, 0, 0 };
		region.imageExtent = { width, height, 1 };

		vkCmdCopyBufferToImage
		(
			commandBuffer,
			buffer,
			image,
			VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
			1,
			&region
		);

		end_single_time_commands(commandBuffer);
	}

	void GameDevice::create_image_with_info
	(
		const VkImageCreateInfo& imageInfo,
		VkMemoryPropertyFlags properties,
		VkImage& image,
		VkDeviceMemory& imageMemory
	)
	{
		if (vkCreateImage(vulkan_device_, &imageInfo, nullptr, &image) != VK_SUCCESS)
		{
			throw std::runtime_error("VkImage 类创建失败");
		}

		VkMemoryRequirements memRequirements;
		vkGetImageMemoryRequirements(vulkan_device_, image, &memRequirements);

		VkMemoryAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocInfo.allocationSize = memRequirements.size;
		allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties);

		if (vkAllocateMemory(vulkan_device_, &allocInfo, nullptr, &imageMemory) != VK_SUCCESS)
		{
			throw std::runtime_error("VkImage 内存分配失败");
		}

		if (vkBindImageMemory(vulkan_device_, image, imageMemory, 0) != VK_SUCCESS)
		{
			throw std::runtime_error("VkImage 内存绑定失败");
		}
	}

}  // namespace vulkancraft
