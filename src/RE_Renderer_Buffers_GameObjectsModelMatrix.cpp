#include "RE_Renderer_Buffers_Internal.hpp"

namespace RE {

	VkBuffer vk_ahGameObjectsModelMatrixBuffers[RE_VK_FRAMES_IN_FLIGHT];

	bool create_game_objects_model_matrix_buffers(const VulkanQueueCollection &rQueues) {
		const VkDeviceSize vk_bufferSize = get_max_game_object_count() * sizeof(GameObjectModelMatrixShaderData);
		uint8_t u8FrameInFlightCreateIndex = 0;
		for (; u8FrameInFlightCreateIndex < RE_VK_FRAMES_IN_FLIGHT; u8FrameInFlightCreateIndex++) {
			PRINT_DEBUG("Creating game object model matrix buffer at frame-in-flight index ", u8FrameInFlightCreateIndex, " in Vulkan");
			if (!create_vulkan_buffer(0,
					vk_bufferSize,
					VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
					rQueues.u8QueueCount,
					rQueues.queueFamilyIndices.get(),
					&vk_ahGameObjectsModelMatrixBuffers[u8FrameInFlightCreateIndex])) {
				RE_FATAL_ERROR("Failed to create game object model matrix buffer at frame-in-flight index ", u8FrameInFlightCreateIndex, " in Vulkan");
				break;
			}
		}
		if (u8FrameInFlightCreateIndex == RE_VK_FRAMES_IN_FLIGHT)
			return true;
		PRINT_DEBUG("Destroying game object model matrix buffer in Vulkan for failing creating all");
		for (uint8_t u8FrameInFlightDestroyIndex = 0; u8FrameInFlightDestroyIndex < u8FrameInFlightCreateIndex; u8FrameInFlightDestroyIndex++)
			vkDestroyBuffer(vk_hDevice, vk_ahGameObjectsModelMatrixBuffers[u8FrameInFlightDestroyIndex], nullptr);
		return false;
	}

	void destroy_game_objects_model_matrix_buffers() {
		PRINT_DEBUG("Destroying all game object model matrix buffers in Vulkan");
		for (VkBuffer &vk_rhGameObjectBuffer : vk_ahGameObjectsModelMatrixBuffers)
			vkDestroyBuffer(vk_hDevice, vk_rhGameObjectBuffer, nullptr);
	}

}
