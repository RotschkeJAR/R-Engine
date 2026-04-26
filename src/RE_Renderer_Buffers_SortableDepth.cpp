#include "RE_Renderer_Buffers_Internal.hpp"

namespace RE {

	VkBuffer vk_ahSortableDepthBuffers[RE_VK_FRAMES_IN_FLIGHT];

	bool create_sortable_depth_buffers(const VulkanQueueCollection &rQueues) {
		uint8_t u8FrameInFlightCreateIndex = 0;
		for (; u8FrameInFlightCreateIndex < RE_VK_FRAMES_IN_FLIGHT; u8FrameInFlightCreateIndex++) {
			PRINT_DEBUG("Creating sortable depth buffer at frame-in-flight index ", u8FrameInFlightCreateIndex);
			if (!create_vulkan_buffer(0,
					1000 * sizeof(DepthShaderData),
					VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
					rQueues.u8QueueCount,
					rQueues.queueFamilyIndices.get(),
					&vk_ahSortableDepthBuffers[u8FrameInFlightCreateIndex])) {
				RE_FATAL_ERROR("Failed to create sortable depth buffer in Vulkan at frame-in-flight index ", u8FrameInFlightCreateIndex);
				break;
			}
		}
		if (u8FrameInFlightCreateIndex == RE_VK_FRAMES_IN_FLIGHT)
			return true;
		for (uint8_t u8FrameInFlightDestroyIndex = 0; u8FrameInFlightDestroyIndex < u8FrameInFlightCreateIndex; u8FrameInFlightDestroyIndex++) {
			PRINT_DEBUG("Destroying sortable depth buffer at frame-in-flight index ", u8FrameInFlightDestroyIndex, " for failure creating all");
			vkDestroyBuffer(vk_hDevice, vk_ahSortableDepthBuffers[u8FrameInFlightDestroyIndex], nullptr);
		}
		return false;
	}

	void destroy_sortable_depth_buffers() {
		PRINT_DEBUG("Destroying sortable depth buffers in Vulkan");
		for (VkBuffer vk_hSortableDepthBuffer : vk_ahSortableDepthBuffers)
			vkDestroyBuffer(vk_hDevice, vk_hSortableDepthBuffer, nullptr);
	}

}
