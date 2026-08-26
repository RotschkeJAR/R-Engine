#include "RE_Renderer_Buffers_Internal.hpp"

namespace RE {

#ifndef NDEBUG

	VkBuffer vk_hDebugBuffer = VK_NULL_HANDLE;
	VulkanMemory *pDebugBufferMemory = nullptr;
	void *pDebugBufferContent = nullptr;

	bool create_debug_buffer() {
		PRINT_DEBUG("Creating Vulkan buffer for debugging");
		std::vector<uint32_t> std_queues;
		std_queues.reserve(u8LogicalQueueCount);
		for (auto xQueueIndex = 0; xQueueIndex < u8LogicalQueueCount; xQueueIndex++) {
			if ((vk_paeQueueTypes[xQueueIndex] & VK_QUEUE_TRANSFER_BIT))
				std_queues.push_back(xQueueIndex);
		}
		if (create_vulkan_buffer(
				0,
				10000000, /* 10 MB */
				VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
				static_cast<uint32_t>(std_queues.size()),
				std_queues.data(),
				&vk_hDebugBuffer)) {
			return true;
		} else
			RE_ERROR("Failed to create Vulkan buffer for debugging");
		vk_hDebugBuffer = VK_NULL_HANDLE;
		pDebugBufferMemory = nullptr;
		pDebugBufferContent = nullptr;
		return false;
	}

	void destroy_debug_buffer() {
		PRINT_DEBUG("Destroying Vulkan buffer used for debugging");
		vkDestroyBuffer(vk_hDevice, vk_hDebugBuffer, nullptr);
		vk_hDebugBuffer = VK_NULL_HANDLE;
		pDebugBufferMemory = nullptr;
		pDebugBufferContent = nullptr;
	}

#endif

}
