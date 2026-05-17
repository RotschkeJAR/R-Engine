#include "RE_Renderer_Buffers_Internal.hpp"

namespace RE {

	static std::unique_ptr<VulkanMemory[]> localBufferMemories,
		deviceBufferMemories;
	
	bool alloc_memory_for_renderer_buffers() {
		PRINT_DEBUG("Filling buffer information for allocating shared memory");
		constexpr uint8_t u8CameraBufferIndex = 0,
			u8StagingRawGameObjectBufferIndex = 1;
		SharedVulkanMemoryInfo localBufferInfos[] = {
			{
				.vulkanStorageObject = vk_hCameraBuffer,
				.u32RegionIndex = 0
			}, {
				.vulkanStorageObject = vk_hStagingGameObjectsBuffer,
				.u32RegionIndex = 0
			}
		};
		VulkanMemoryAllocationInfo localBufferAllocs[sizeof(localBufferInfos) / sizeof(localBufferInfos[0])];
		PRINT_DEBUG("Allocating local Vulkan memory for buffers used by renderer");
		size_t numberOfAllocatedMemories;
		if (alloc_shared_vulkan_memory(sizeof(localBufferInfos) / sizeof(localBufferInfos[0]),
				localBufferInfos,
				VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
				numberOfAllocatedMemories,
				localBufferMemories,
				localBufferAllocs) == VK_SUCCESS) {
			std::unique_ptr<void*[]> bufferMemoryPointers = std::make_unique<void*[]>(numberOfAllocatedMemories);
			size_t memoryIndex = 0;
			for (; memoryIndex < numberOfAllocatedMemories; memoryIndex++)
				if (!localBufferMemories[memoryIndex].map(0, 0, VK_WHOLE_SIZE, &bufferMemoryPointers[memoryIndex])) {
					RE_FATAL_ERROR("Failed to map buffer memory at index ", memoryIndex, " to host");
					break;
				}
			if (memoryIndex == numberOfAllocatedMemories) {
				pCameraBufferMemory = &localBufferMemories[localBufferAllocs[u8CameraBufferIndex].indexToMemory];
				pStagingGameObjectsBufferMemory = &localBufferMemories[localBufferAllocs[u8StagingRawGameObjectBufferIndex].indexToMemory];
				paStagingGameObjectsBufferData = reinterpret_cast<GameObjectShaderData*>(bufferMemoryPointers[localBufferAllocs[u8StagingRawGameObjectBufferIndex].indexToMemory]);
				constexpr uint8_t u8DeviceBufferCount = 3,
					u8GameObjectsBufferIndex = 0,
					u8GameObjectsModelMatrixBufferIndex = 1,
					u8SortableDepthBufferIndex = 2;
				SharedVulkanMemoryInfo deviceBufferInfos[u8DeviceBufferCount * RE_VK_FRAMES_IN_FLIGHT];
				VulkanMemoryAllocationInfo deviceBufferAllocs[sizeof(deviceBufferInfos) / sizeof(deviceBufferInfos[0])];
				for (uint8_t u8DeviceBufferIndex = 0; u8DeviceBufferIndex < u8DeviceBufferCount; u8DeviceBufferIndex++)
					for (uint8_t u8FramesInFlightIndex = 0; u8FramesInFlightIndex < RE_VK_FRAMES_IN_FLIGHT; u8FramesInFlightIndex++) {
						const uint16_t u16InfoIndex = u8DeviceBufferIndex * RE_VK_FRAMES_IN_FLIGHT + u8FramesInFlightIndex;
						switch (u8DeviceBufferIndex) {
							case u8GameObjectsBufferIndex:
								deviceBufferInfos[u16InfoIndex].vulkanStorageObject = vk_ahGameObjectsBuffers[u8FramesInFlightIndex];
								break;
							case u8GameObjectsModelMatrixBufferIndex:
								deviceBufferInfos[u16InfoIndex].vulkanStorageObject = vk_ahGameObjectsModelMatrixBuffers[u8FramesInFlightIndex];
								break;
							case u8SortableDepthBufferIndex:
								deviceBufferInfos[u16InfoIndex].vulkanStorageObject = vk_ahSortableDepthBuffers[u8FramesInFlightIndex];
								break;
						}
						deviceBufferInfos[u16InfoIndex].u32RegionIndex = 0;
					}
				if (alloc_shared_vulkan_memory(sizeof(deviceBufferInfos) / sizeof(deviceBufferInfos[0]),
						deviceBufferInfos,
						VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
						numberOfAllocatedMemories,
						deviceBufferMemories,
						deviceBufferAllocs) == VK_SUCCESS)
					return true;
				else
					RE_FATAL_ERROR("Failed to allocate memory for Vulkan buffers residing on the GPU");
			}
			localBufferMemories.reset();
		} else
			RE_FATAL_ERROR("Failed to allocate memory for Vulkan buffers residing on the CPU");
		return false;
	}

	void free_memory_for_renderer_buffers() {
		PRINT_DEBUG("Freeing Vulkan memory used for buffers in renderer");
		localBufferMemories.reset();
		deviceBufferMemories.reset();
	}

}