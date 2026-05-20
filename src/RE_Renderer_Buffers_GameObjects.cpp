#include "RE_Renderer_Buffers_Internal.hpp"

namespace RE {

	VkBuffer vk_ahGameObjectsBuffers[RE_VK_FRAMES_IN_FLIGHT];

	bool create_game_objects_buffers() {
		PRINT_DEBUG("Querying queues for GPU-local game object buffer");
		const VkDeviceSize vk_bufferSize = get_max_game_object_count() * sizeof(GameObjectShaderData);
		constexpr uint32_t au32ObjectBufferQueues[] = {
			RENDER_TASK_SUBINDEX_BUFFER_TRANSFER,
			RENDER_TASK_SUBINDEX_PROCESSING
		};
		const VulkanQueueCollection queuesForObjectBuffer = aRenderTasks[0].queues_of_functions(au32ObjectBufferQueues, sizeof(au32ObjectBufferQueues) / sizeof(au32ObjectBufferQueues[0]));
		uint8_t u8FrameInFlightCreateIndex = 0;
		for (; u8FrameInFlightCreateIndex < RE_VK_FRAMES_IN_FLIGHT; u8FrameInFlightCreateIndex++) {
			PRINT_DEBUG("Creating game object buffer in Vulkan");
			if (create_vulkan_buffer(0,
					vk_bufferSize,
					VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
					queuesForObjectBuffer.u8QueueCount,
					queuesForObjectBuffer.queueFamilyIndices.get(),
					&vk_ahGameObjectsBuffers[u8FrameInFlightCreateIndex]))
				continue;
			else
				RE_FATAL_ERROR("Failed to create game object buffer in Vulkan at index ", u8FrameInFlightCreateIndex);
			break;
		}
		if (u8FrameInFlightCreateIndex == RE_VK_FRAMES_IN_FLIGHT)
			return true;
		PRINT_DEBUG("Destroying game object buffers due to failure creating all");
		for (uint8_t u8FrameInFlightDestroyIndex = 0; u8FrameInFlightDestroyIndex < u8FrameInFlightCreateIndex; u8FrameInFlightDestroyIndex++)
			vkDestroyBuffer(vk_hDevice, vk_ahGameObjectsBuffers[u8FrameInFlightDestroyIndex], nullptr);
		return false;
	}

	void destroy_game_objects_buffers() {
		PRINT_DEBUG("Destroying game object buffers in Vulkan");
		for (uint8_t u8FrameInFlightDestroyIndex = 0; u8FrameInFlightDestroyIndex < RE_VK_FRAMES_IN_FLIGHT; u8FrameInFlightDestroyIndex++)
			vkDestroyBuffer(vk_hDevice, vk_ahGameObjectsBuffers[u8FrameInFlightDestroyIndex], nullptr);
	}

}
