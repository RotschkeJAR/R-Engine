#include "RE_Renderer_Buffers_Internal.hpp"

namespace RE {
	
	VkBuffer vk_ahCameraBuffers[RE_VK_FRAMES_IN_FLIGHT];
	VkDeviceSize vk_aCameraBufferMemoryOffsets[RE_VK_FRAMES_IN_FLIGHT];
	VulkanMemory (*apCameraBufferMemories)[RE_VK_FRAMES_IN_FLIGHT];

	bool create_camera_buffers(const VulkanQueueCollection &rQueues) {
		uint8_t u8FrameInFlightCreateIndex = 0;
		for (; u8FrameInFlightCreateIndex < RE_VK_FRAMES_IN_FLIGHT; u8FrameInFlightCreateIndex++) {
			PRINT_DEBUG("Creating Vulkan buffer at frame-in-flight index ", u8FrameInFlightCreateIndex, " for data of the camera");
			if (!create_vulkan_buffer(0,
					sizeof(CameraShaderData),
					VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
					static_cast<uint32_t>(rQueues.u8QueueCount),
					rQueues.queueFamilyIndices.get(),
					&vk_ahCameraBuffers[u8FrameInFlightCreateIndex])) {
				RE_FATAL_ERROR("Failed to create a Vulkan buffer at frame-in-flight index ", u8FrameInFlightIndex, " for data of the camera");
				break;
			}
		}
		if (u8FrameInFlightCreateIndex == RE_VK_FRAMES_IN_FLIGHT)
			return true;
		PRINT_DEBUG("Destroying Vulkan buffers used for the camera due to failure creating all");
		uint8_t u8FrameInFlightDestroyIndex = 0;
		while (u8FrameInFlightDestroyIndex < u8FrameInFlightCreateIndex) {
			vkDestroyBuffer(vk_hDevice, vk_ahCameraBuffers[u8FrameInFlightDestroyIndex], nullptr);
			u8FrameInFlightDestroyIndex++;
		}
		return false;
	}

	void destroy_camera_buffers() {
		PRINT_DEBUG("Destroying all camera buffers and freeing their memory");
		for (VkBuffer &vk_rhCamBuffer : vk_ahCameraBuffers)
			vkDestroyBuffer(vk_hDevice, vk_rhCamBuffer, nullptr);
	}

}
