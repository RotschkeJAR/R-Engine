#include "RE_Renderer_Buffers_Internal.hpp"

namespace RE {

	static std::unique_ptr<VulkanMemory[]> std_localBufferMemories,
		std_deviceBufferMemories;
	
	bool alloc_memory_for_renderer_buffers() {
		PRINT_DEBUG("Filling buffer information for allocating shared memory");
		constexpr unsigned uCameraBufferIndex = 0,
			uStagingRawGameObjectBufferIndex = 1,
			uGameObjectCountBufferIndex = 2;
	#ifndef NDEBUG
		constexpr unsigned uDebugBufferIndex = 3;
	#endif
		SharedVulkanMemoryInfo aLocalBufferInfos[] = {
			{
				.vulkanStorageObject = vk_hCameraBuffer,
				.uRegionIndex = 0
			}, {
				.vulkanStorageObject = vk_hStagingGameObjectsBuffer,
				.uRegionIndex = 0
			}, {
				.vulkanStorageObject = vk_hGameObjectCountBuffer,
				.uRegionIndex = 0
			},
		#ifndef NDEBUG
			{
				.vulkanStorageObject = vk_hDebugBuffer,
				.uRegionIndex = 0
			},
		#endif
		};
		VulkanMemoryAllocationInfo aLocalBufferAllocs[sizeof(aLocalBufferInfos) / sizeof(aLocalBufferInfos[0])];
		PRINT_DEBUG("Allocating local Vulkan memory for buffers used by renderer");
		size_t numberOfAllocatedMemories;
		if (alloc_shared_vulkan_memory(
				sizeof(aLocalBufferInfos) / sizeof(aLocalBufferInfos[0]),
				aLocalBufferInfos,
				VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
				numberOfAllocatedMemories,
				std_localBufferMemories,
				aLocalBufferAllocs) == VK_SUCCESS) {
			std::unique_ptr<void*[]> std_bufferMemoryPointers = std::make_unique<void*[]>(numberOfAllocatedMemories);
			size_t memoryIndex = 0;
			for (; memoryIndex < numberOfAllocatedMemories; memoryIndex++)
				if (!std_localBufferMemories[memoryIndex].map(0, 0, VK_WHOLE_SIZE, &std_bufferMemoryPointers[memoryIndex])) {
					RE_FATAL_ERROR("Failed to map buffer memory at index ", memoryIndex, " to host");
					break;
				}
			if (memoryIndex == numberOfAllocatedMemories) {
				{
					const size_t cameraIndexToMemory = aLocalBufferAllocs[uCameraBufferIndex].indexToMemory,
						stagingGameObjectIndexToMemory = aLocalBufferAllocs[uStagingRawGameObjectBufferIndex].indexToMemory,
						gameObjectCountIndexToMemory = aLocalBufferAllocs[uGameObjectCountBufferIndex].indexToMemory;
					pCameraBufferMemory = &std_localBufferMemories[cameraIndexToMemory];
					for (unsigned uCameraDataIndex = 0; uCameraDataIndex < get_max_camera_count() * RE_VK_FRAMES_IN_FLIGHT; uCameraDataIndex++)
						camerasShaderData[uCameraDataIndex] = reinterpret_cast<CameraShaderData*>(
								reinterpret_cast<char*>(std_bufferMemoryPointers[cameraIndexToMemory])
									+ aLocalBufferAllocs[uCameraBufferIndex].vk_memoryOffset
									+ next_multiple_inclusive<VkDeviceSize>(sizeof(CameraShaderData), vk_uniformBufferAlignment) * uCameraDataIndex);
					pStagingGameObjectsBufferMemory = &std_localBufferMemories[stagingGameObjectIndexToMemory];
					paStagingGameObjectsBufferData = reinterpret_cast<GameObjectShaderData*>(
							reinterpret_cast<char*>(std_bufferMemoryPointers[stagingGameObjectIndexToMemory]) + aLocalBufferAllocs[uStagingRawGameObjectBufferIndex].vk_memoryOffset);
					pGameObjectsCountBufferMemory = &std_localBufferMemories[gameObjectCountIndexToMemory];
					for (unsigned uFramesInFlightIndex = 0; uFramesInFlightIndex < RE_VK_FRAMES_IN_FLIGHT; uFramesInFlightIndex++)
						apGameObjectsCountBufferData[uFramesInFlightIndex] = reinterpret_cast<GameObjectCountShaderData*>(
								reinterpret_cast<char*>(std_bufferMemoryPointers[gameObjectCountIndexToMemory])
									+ aLocalBufferAllocs[uGameObjectCountBufferIndex].vk_memoryOffset
									+ next_multiple_inclusive<VkDeviceSize>(sizeof(GameObjectCountShaderData), vk_uniformBufferAlignment) * uFramesInFlightIndex);
				#ifndef NDEBUG
					const size_t debugIndexToMemory = aLocalBufferAllocs[uDebugBufferIndex].indexToMemory;
					pDebugBufferMemory = &std_localBufferMemories[debugIndexToMemory];
					pDebugBufferContent = reinterpret_cast<void*>(
							reinterpret_cast<char*>(std_bufferMemoryPointers[debugIndexToMemory]) + aLocalBufferAllocs[uDebugBufferIndex].vk_memoryOffset);
				#endif
				}
				constexpr unsigned uDeviceBufferCount = 3,
					uGameObjectsBufferIndex = 0,
					uGameObjectsModelMatrixBufferIndex = 1,
					uSortableDepthBufferIndex = 2;
				SharedVulkanMemoryInfo deviceBufferInfos[uDeviceBufferCount * RE_VK_FRAMES_IN_FLIGHT];
				VulkanMemoryAllocationInfo deviceBufferAllocs[sizeof(deviceBufferInfos) / sizeof(deviceBufferInfos[0])];
				for (unsigned uDeviceBufferIndex = 0; uDeviceBufferIndex < uDeviceBufferCount; uDeviceBufferIndex++)
					for (unsigned uFramesInFlightIndex = 0; uFramesInFlightIndex < RE_VK_FRAMES_IN_FLIGHT; uFramesInFlightIndex++) {
						const unsigned uInfoIndex = uDeviceBufferIndex * RE_VK_FRAMES_IN_FLIGHT + uFramesInFlightIndex;
						switch (uDeviceBufferIndex) {
							case uGameObjectsBufferIndex:
								deviceBufferInfos[uInfoIndex].vulkanStorageObject = vk_ahGameObjectsBuffers[uFramesInFlightIndex];
								break;
							case uGameObjectsModelMatrixBufferIndex:
								deviceBufferInfos[uInfoIndex].vulkanStorageObject = vk_ahGameObjectsModelMatrixBuffers[uFramesInFlightIndex];
								break;
							case uSortableDepthBufferIndex:
								deviceBufferInfos[uInfoIndex].vulkanStorageObject = vk_ahSortableDepthBuffers[uFramesInFlightIndex];
								break;
						}
						deviceBufferInfos[uInfoIndex].uRegionIndex = 0;
					}
				if (alloc_shared_vulkan_memory(
						sizeof(deviceBufferInfos) / sizeof(deviceBufferInfos[0]),
						deviceBufferInfos,
						VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
						numberOfAllocatedMemories,
						std_deviceBufferMemories,
						deviceBufferAllocs) == VK_SUCCESS)
					return true;
				else
					RE_FATAL_ERROR("Failed to allocate memory for Vulkan buffers residing on the GPU");
			}
			std_localBufferMemories.reset();
		} else
			RE_FATAL_ERROR("Failed to allocate memory for Vulkan buffers residing on the CPU");
		return false;
	}

	void free_memory_for_renderer_buffers() {
		PRINT_DEBUG("Freeing Vulkan memory used for buffers in renderer");
		std_localBufferMemories.reset();
		std_deviceBufferMemories.reset();
	}

}