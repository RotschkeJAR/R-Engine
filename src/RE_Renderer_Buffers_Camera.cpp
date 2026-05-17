#include "RE_Renderer_Buffers_Internal.hpp"

namespace RE {
	
	VkBuffer vk_hCameraBuffer;
	VulkanMemory *pCameraBufferMemory;
	std::unique_ptr<CameraShaderData*[]> camerasShaderData;

	bool create_camera_buffers(const VulkanQueueCollection &rQueues) {
		PRINT_DEBUG("Creating Vulkan buffer for data of the camera");
		if (create_vulkan_buffer(0,
				next_multiple_inclusive<VkDeviceSize>(sizeof(CameraShaderData), vk_uniformBufferAlignment) * (get_max_camera_count() * RE_VK_FRAMES_IN_FLIGHT - 1) + sizeof(CameraShaderData),
				VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
				static_cast<uint32_t>(rQueues.u8QueueCount),
				rQueues.queueFamilyIndices.get(),
				&vk_hCameraBuffer)) {
			camerasShaderData = std::make_unique<CameraShaderData*[]>(get_max_camera_count() * RE_VK_FRAMES_IN_FLIGHT);
			return true;
		} else
			RE_FATAL_ERROR("Failed to create a Vulkan buffer for data of the camera");
		return false;
	}

	void destroy_camera_buffers() {
		PRINT_DEBUG("Destroying Vulkan buffer used for camera data");
		vkDestroyBuffer(vk_hDevice, vk_hCameraBuffer, nullptr);
		camerasShaderData.reset();
	}

}
