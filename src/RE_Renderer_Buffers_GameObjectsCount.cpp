#include "RE_Renderer_Buffers_Internal.hpp"
#include "RE_Vulkan_Wrappers.hpp"

namespace RE {

	VkBuffer vk_hGameObjectCountBuffer;
	VulkanMemory *pGameObjectsCountBufferMemory;
	GameObjectCountShaderData *apGameObjectsCountBufferData[RE_VK_FRAMES_IN_FLIGHT];

	bool create_game_object_count_buffer(const VulkanQueueCollection &rQueues) {
		VkDeviceSize vk_bufferSize = 0;
		for (unsigned uIndex = 0; uIndex < RE_VK_FRAMES_IN_FLIGHT; uIndex++)
			vk_bufferSize = next_multiple_inclusive<VkDeviceSize>(vk_bufferSize, vk_uniformBufferAlignment) + sizeof(GameObjectCountShaderData);
		PRINT_DEBUG("Creating Vulkan uniform buffer for storing the count of game objects");
		if (create_vulkan_buffer(
				0,
				vk_bufferSize,
				VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
				rQueues.u8QueueCount,
				rQueues.queueFamilyIndices.get(),
				&vk_hGameObjectCountBuffer)) {
			return true;
		} else
			RE_FATAL_ERROR("Failed to create Vulkan uniform buffer storing the count of game objects");
		return false;
	}

	void destroy_game_object_count_buffer() {
		PRINT_DEBUG("Destroying Vulkan uniform buffer ", vk_hGameObjectCountBuffer, " used for storing the count of game objects");
		vkDestroyBuffer(vk_hDevice, vk_hGameObjectCountBuffer, nullptr);
	}

}
