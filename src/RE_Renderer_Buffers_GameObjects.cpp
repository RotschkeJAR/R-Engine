#include "RE_Renderer_Buffers_Internal.hpp"

namespace RE {

#define USAGE_FLAGS VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT

	VkBuffer vk_ahGameObjectsBuffers[RE_VK_FRAMES_IN_FLIGHT];

	bool create_game_objects_buffers() {
		PRINT_DEBUG("Querying queues for GPU-local game object buffer");
		const VkDeviceSize vk_bufferSize = get_max_game_object_count() * sizeof(GameObjectShaderData);
		constexpr unsigned auObjectBufferQueues[] = {
			RENDER_TASK_SUBINDEX_BUFFER_TRANSFER,
			RENDER_TASK_SUBINDEX_PROCESSING,
			RENDER_TASK_SUBINDEX_RENDERING
		};
		const VulkanQueueCollection queuesForObjectBuffer = aRenderTasks[0].queues_of_functions(auObjectBufferQueues, sizeof(auObjectBufferQueues) / sizeof(auObjectBufferQueues[0]), false);
		unsigned uFrameInFlightCreateIndex = 0;
		for (; uFrameInFlightCreateIndex < RE_VK_FRAMES_IN_FLIGHT; uFrameInFlightCreateIndex++) {
			PRINT_DEBUG("Creating game object buffer in Vulkan");
			if (create_vulkan_buffer(0,
					vk_bufferSize,
				#ifndef NDEBUG
					USAGE_FLAGS | VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
				#else
					USAGE_FLAGS,
				#endif
					queuesForObjectBuffer.uQueueCount,
					queuesForObjectBuffer.std_queueFamilyIndices.get(),
					&vk_ahGameObjectsBuffers[uFrameInFlightCreateIndex]))
				continue;
			else
				RE_FATAL_ERROR("Failed to create game object buffer in Vulkan at index ", uFrameInFlightCreateIndex);
			break;
		}
		if (uFrameInFlightCreateIndex == RE_VK_FRAMES_IN_FLIGHT)
			return true;
		PRINT_DEBUG("Destroying game object buffers due to failure creating all");
		for (uint8_t u8FrameInFlightDestroyIndex = 0; u8FrameInFlightDestroyIndex < uFrameInFlightCreateIndex; u8FrameInFlightDestroyIndex++)
			vkDestroyBuffer(vk_hDevice, vk_ahGameObjectsBuffers[u8FrameInFlightDestroyIndex], nullptr);
		return false;
	}

	void destroy_game_objects_buffers() {
		PRINT_DEBUG("Destroying game object buffers in Vulkan");
		for (uint8_t u8FrameInFlightDestroyIndex = 0; u8FrameInFlightDestroyIndex < RE_VK_FRAMES_IN_FLIGHT; u8FrameInFlightDestroyIndex++)
			vkDestroyBuffer(vk_hDevice, vk_ahGameObjectsBuffers[u8FrameInFlightDestroyIndex], nullptr);
	}

}
