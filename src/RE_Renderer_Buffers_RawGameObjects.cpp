#include "RE_Renderer_Buffers_Internal.hpp"

namespace RE {
	
	static VkBuffer vk_ahStagingRawGameObjectBuffers[RE_VK_FRAMES_IN_FLIGHT];
	RawGameObjectShaderData *apaGameObjectBufferInstanceData[RE_VK_FRAMES_IN_FLIGHT];

	VkBuffer vk_ahRawGameObjectBuffers[RE_VK_FRAMES_IN_FLIGHT];

	bool create_raw_game_object_buffers() {
		PRINT_DEBUG("Querying queues for GPU-local raw game object buffer");
		constexpr VkDeviceSize vk_objectBufferByteSize = 1000 * sizeof(RawGameObjectShaderData);
		constexpr uint32_t au32ObjectBufferQueues[] = {
			RENDER_TASK_SUBINDEX_BUFFER_TRANSFER,
			RENDER_TASK_SUBINDEX_PROCESSING
		};
		const VulkanQueueCollection queuesForObjectBuffer = renderTasks[0].queues_of_functions(au32ObjectBufferQueues, sizeof(au32ObjectBufferQueues) / sizeof(au32ObjectBufferQueues[0]));
		const bool bStagingNecessary = is_staging_before_gpu_use_necessary();
		uint8_t u8FrameInFlightCreateIndex = 0;
		for (; u8FrameInFlightCreateIndex < RE_VK_FRAMES_IN_FLIGHT; u8FrameInFlightCreateIndex++) {
			if (bStagingNecessary) {
				PRINT_DEBUG("Creating staging game object buffer in Vulkan at index ", u8FrameInFlightCreateIndex);
				if (!create_vulkan_buffer(0,
						vk_objectBufferByteSize,
						VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
						1,
						nullptr,
						&vk_ahStagingRawGameObjectBuffers[u8FrameInFlightCreateIndex])) {
					RE_FATAL_ERROR("Failed to create staging game object buffer in Vulkan at index ", u8FrameInFlightCreateIndex);
					break;
				}
			}
			PRINT_DEBUG("Creating raw game object buffer on GPU");
			if (create_vulkan_buffer(0,
					vk_objectBufferByteSize,
					VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | (bStagingNecessary ? VK_BUFFER_USAGE_TRANSFER_DST_BIT : 0),
					queuesForObjectBuffer.u8QueueCount,
					queuesForObjectBuffer.queueFamilyIndices.get(),
					&vk_ahRawGameObjectBuffers[u8FrameInFlightCreateIndex]))
				continue;
			else
				RE_FATAL_ERROR("Failed to create raw game object buffer in Vulkan at index ", u8FrameInFlightCreateIndex);
			if (bStagingNecessary) {
				PRINT_DEBUG("Destroying staging game object buffer in Vulkan at index ", u8FrameInFlightCreateIndex, " due to failure creating GPU-local buffer");
				vkDestroyBuffer(vk_hDevice, vk_ahStagingRawGameObjectBuffers[u8FrameInFlightCreateIndex], nullptr);
			}
			break;
		}
		if (u8FrameInFlightCreateIndex == RE_VK_FRAMES_IN_FLIGHT)
			return true;
		for (uint8_t u8FrameInFlightDestroyIndex = 0; u8FrameInFlightDestroyIndex < u8FrameInFlightCreateIndex; u8FrameInFlightDestroyIndex++) {
			PRINT_DEBUG("Destroying game object buffers at frame-in-flight index ", u8FrameInFlightDestroyIndex, " due to failure creating all");
			if (bStagingNecessary)
				vkDestroyBuffer(vk_hDevice, vk_ahStagingRawGameObjectBuffers[u8FrameInFlightDestroyIndex], nullptr);
			vkDestroyBuffer(vk_hDevice, vk_ahRawGameObjectBuffers[u8FrameInFlightDestroyIndex], nullptr);
		}
		return false;
	}

	void destroy_raw_game_object_buffers() {
		const bool bStagingBufferPresent = is_staging_before_gpu_use_necessary();
		for (uint8_t u8FrameInFlightDestroyIndex = 0; u8FrameInFlightDestroyIndex < RE_VK_FRAMES_IN_FLIGHT; u8FrameInFlightDestroyIndex++) {
			PRINT_DEBUG("Destroying game object buffers in Vulkan at frame-in-flight index ", u8FrameInFlightDestroyIndex);
			if (bStagingBufferPresent)
				vkDestroyBuffer(vk_hDevice, vk_ahStagingRawGameObjectBuffers[u8FrameInFlightDestroyIndex], nullptr);
			vkDestroyBuffer(vk_hDevice, vk_ahRawGameObjectBuffers[u8FrameInFlightDestroyIndex], nullptr);
		}
	}

}
