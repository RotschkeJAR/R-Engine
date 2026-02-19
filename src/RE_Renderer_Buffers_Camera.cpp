#include "RE_Renderer_Buffers_Internal.hpp"

namespace RE {
	
	VkBuffer vk_ahCameraBuffers[RE_VK_FRAMES_IN_FLIGHT];
	VulkanMemory cameraBuffersMemory;
	VkDeviceSize vk_aCameraBufferMemoryOffsets[RE_VK_FRAMES_IN_FLIGHT];

	bool create_camera_buffers() {
		PRINT_DEBUG("Querying queues, where camera buffers are being used in");
		constexpr uint32_t au32FunctionIndices[] = {
			RENDER_TASK_SUBINDEX_PROCESSING, 
			RENDER_TASK_SUBINDEX_RENDERING
		};
		const VulkanQueueCollection queues = renderTasks[0].queues_of_functions(au32FunctionIndices, sizeof(au32FunctionIndices) / sizeof(au32FunctionIndices[0]));
		const VkBufferCreateInfo vk_camBufferCreateInfo = {
			.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
			.pNext = nullptr,
			.flags = 0,
			.size = RE_VK_CAMERA_MATRICES_SIZE_BYTES,
			.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
			.sharingMode = queues.vk_eSharingMode,
			.queueFamilyIndexCount = static_cast<uint32_t>(queues.u8QueueCount),
			.pQueueFamilyIndices = queues.queueFamilyIndices.get()
		};
		VkMemoryRequirements2 vk_aBufferMemoryRequirements[RE_VK_FRAMES_IN_FLIGHT];
		VkDeviceSize vk_requiredSize = 0;
		uint32_t b32AvailableMemoryTypes = 0xFFFFFFFF;
		uint8_t u8FrameInFlightIndex = 0;
		while (u8FrameInFlightIndex < RE_VK_FRAMES_IN_FLIGHT) {
			PRINT_DEBUG("Creating camera buffer at frame-in-flight index ", u8FrameInFlightIndex);
			if (vkCreateBuffer(vk_hDevice, &vk_camBufferCreateInfo, nullptr, &vk_ahCameraBuffers[u8FrameInFlightIndex]) == VK_SUCCESS) {
				PRINT_DEBUG("Fetching memory requirements of camera buffer at frame-in-flight index ", u8FrameInFlightIndex);
				vk_aBufferMemoryRequirements[u8FrameInFlightIndex].sType = VK_STRUCTURE_TYPE_MEMORY_REQUIREMENTS_2;
				vk_aBufferMemoryRequirements[u8FrameInFlightIndex].pNext = nullptr;
				const VkBufferMemoryRequirementsInfo2 vk_bufferInfo = {
					.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_REQUIREMENTS_INFO_2,
					.pNext = nullptr,
					.buffer = vk_ahCameraBuffers[u8FrameInFlightIndex]
				};
				vkGetBufferMemoryRequirements2(vk_hDevice, &vk_bufferInfo, &vk_aBufferMemoryRequirements[u8FrameInFlightIndex]);
				vk_requiredSize = (is_multiple_of<VkDeviceSize>(vk_requiredSize, vk_aBufferMemoryRequirements[u8FrameInFlightIndex].memoryRequirements.alignment)
						? vk_requiredSize
						: next_multiple<VkDeviceSize>(vk_requiredSize, vk_aBufferMemoryRequirements[u8FrameInFlightIndex].memoryRequirements.alignment))
					+ vk_aBufferMemoryRequirements[u8FrameInFlightIndex].memoryRequirements.size;
				b32AvailableMemoryTypes &= vk_aBufferMemoryRequirements[u8FrameInFlightIndex].memoryRequirements.memoryTypeBits;
				u8FrameInFlightIndex++;
			} else {
				RE_FATAL_ERROR("Failed to create a Vulkan buffer at frame-in-flight index ", u8FrameInFlightIndex, " for data of the camera");
				break;
			}
		}
		if (u8FrameInFlightIndex == RE_VK_FRAMES_IN_FLIGHT) {
			if (b32AvailableMemoryTypes) {
				PRINT_DEBUG("Allocating memory for all camera buffers");
				if (cameraBuffersMemory.alloc(vk_requiredSize, RE_VK_CPU_RAM, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, b32AvailableMemoryTypes)) {
					void *pCameraBuffersMemory;
					if (cameraBuffersMemory.map(0, 0, VK_WHOLE_SIZE, &pCameraBuffersMemory)) {
						PRINT_DEBUG("Binding memory to all camera buffers");
						VkBindBufferMemoryInfo vk_aBindInfos[RE_VK_FRAMES_IN_FLIGHT];
						VkDeviceSize vk_memoryOffset = 0;
						for (u8FrameInFlightIndex = 0; u8FrameInFlightIndex < RE_VK_FRAMES_IN_FLIGHT; u8FrameInFlightIndex++) {
							if (!is_multiple_of<VkDeviceSize>(vk_memoryOffset, vk_aBufferMemoryRequirements[u8FrameInFlightIndex].memoryRequirements.alignment))
								vk_memoryOffset = next_multiple<VkDeviceSize>(vk_memoryOffset, vk_aBufferMemoryRequirements[u8FrameInFlightIndex].memoryRequirements.alignment);
							vk_aBindInfos[u8FrameInFlightIndex].sType = VK_STRUCTURE_TYPE_BIND_BUFFER_MEMORY_INFO;
							vk_aBindInfos[u8FrameInFlightIndex].pNext = nullptr;
							vk_aBindInfos[u8FrameInFlightIndex].buffer = vk_ahCameraBuffers[u8FrameInFlightIndex];
							vk_aBindInfos[u8FrameInFlightIndex].memory = cameraBuffersMemory.get();
							vk_aBindInfos[u8FrameInFlightIndex].memoryOffset = vk_memoryOffset;
							apafCameraMatrices[u8FrameInFlightIndex] = reinterpret_cast<float*>(reinterpret_cast<uint8_t*>(pCameraBuffersMemory) + vk_memoryOffset);
							vk_aCameraBufferMemoryOffsets[u8FrameInFlightIndex] = vk_memoryOffset;
						}
						if (vkBindBufferMemory2(vk_hDevice, RE_VK_FRAMES_IN_FLIGHT, vk_aBindInfos) == VK_SUCCESS) {
							init_camera_matrices();
							return true;
						} else
							RE_FATAL_ERROR("Failed binding Vulkan memory to all camera buffers");
					} else
						RE_FATAL_ERROR("Failed mapping memory of camera buffers to the CPU");
				} else
					RE_FATAL_ERROR("Failed allocating memory for the camera buffers");
			} else
				RE_FATAL_ERROR("No type memory can be allocated from is suitable for all camera buffers");
		}
		return false;
	}

	void destroy_camera_buffers() {
		PRINT_DEBUG("Destroying all camera buffers and freeing their memory");
		for (VkBuffer &vk_rhCamBuffer : vk_ahCameraBuffers)
			vkDestroyBuffer(vk_hDevice, vk_rhCamBuffer, nullptr);
		cameraBuffersMemory.free();
	}

}
