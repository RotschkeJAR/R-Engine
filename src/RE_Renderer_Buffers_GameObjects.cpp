#include "RE_Renderer_Buffers_Internal.hpp"

namespace RE {

	VkBuffer vk_ahGameObjectBuffers[RE_VK_FRAMES_IN_FLIGHT];

	bool create_game_object_buffers(const VulkanQueueCollection &rQueues) {
		uint8_t u8FrameInFlightCreateIndex = 0;
		while (u8FrameInFlightCreateIndex < RE_VK_FRAMES_IN_FLIGHT) {
			PRINT_DEBUG("Creating game object buffer at frame-in-flight index ", u8FrameInFlightCreateIndex);
			if (create_vulkan_buffer(0,
					1000 * sizeof(GameObjectShaderData),
					VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
					rQueues.u8QueueCount,
					rQueues.queueFamilyIndices.get(),
					&vk_ahGameObjectBuffers[u8FrameInFlightCreateIndex]))
				u8FrameInFlightCreateIndex++;
			else {
				RE_FATAL_ERROR("Failed to create game object buffer at frame-in-flight index ", u8FrameInFlightCreateIndex);
				break;
			}
		}
		if (u8FrameInFlightCreateIndex == RE_VK_FRAMES_IN_FLIGHT)
			return true;
		for (uint8_t u8FrameInFlightDestroyIndex = 0; u8FrameInFlightDestroyIndex < u8FrameInFlightCreateIndex; u8FrameInFlightDestroyIndex++) {
			PRINT_DEBUG("Destroying game object buffer at frame-in-flight index ", u8FrameInFlightDestroyIndex, " for failing creating all game object buffers");
			vkDestroyBuffer(vk_hDevice, vk_ahGameObjectBuffers[u8FrameInFlightDestroyIndex], nullptr);
		}
		return false;
	}

	void destroy_game_object_buffers() {
		for (VkBuffer &vk_rhGameObjectBuffer : vk_ahGameObjectBuffers)
			vkDestroyBuffer(vk_hDevice, vk_rhGameObjectBuffer, nullptr);
	}

}
