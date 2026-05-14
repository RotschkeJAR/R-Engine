#include "RE_Renderer_Buffers_Internal.hpp"

namespace RE {

	static std::unique_ptr<VulkanMemory[]> localBufferMemories;
	
	bool alloc_memory_for_buffers_renderer() {
		PRINT_DEBUG("Filling buffer information for allocating shared memory");
		constexpr uint8_t u8BufferTypeCount = 4,
			u8CameraIndex = 0,
			u8RawGameObjectIndex = 1,
			u8GameObjectIndex = 2,
			u8SortableDepthIndex = 3;
		constexpr uint16_t u16BufferCount = 7;
		SharedVulkanMemoryInfo localBufferInfos[u16BufferCount];
		VulkanMemoryAllocationInfo bufferAllocs[u16BufferCount];
		uint16_t u16InfoIndex = 0;
		for (uint8_t u8BufferTypeIndex = 0; u8BufferTypeIndex < u8BufferTypeCount; u8BufferTypeIndex++) {
			for (uint8_t u8FrameInFlightIndex = 0; u8FrameInFlightIndex < RE_VK_FRAMES_IN_FLIGHT; u8FrameInFlightIndex++) {
				switch (u8BufferTypeIndex) {
					case u8CameraIndex:
						if (u8FrameInFlightIndex)
							continue;
						localBufferInfos[u16InfoIndex].vulkanStorageObject = vk_hCameraBuffer;
						break;
					case u8RawGameObjectIndex:
						localBufferInfos[u16InfoIndex].vulkanStorageObject = vk_ahRawGameObjectBuffers[u8FrameInFlightIndex];
						break;
					case u8GameObjectIndex:
						localBufferInfos[u16InfoIndex].vulkanStorageObject = vk_ahGameObjectBuffers[u8FrameInFlightIndex];
						break;
					case u8SortableDepthIndex:
						localBufferInfos[u16InfoIndex].vulkanStorageObject = vk_ahSortableDepthBuffers[u8FrameInFlightIndex];
						break;
					[[unlikely]] default:
					 	RE_ABORT("Unknown buffer type index for allocating memory: ", u8BufferTypeIndex);
				}
				localBufferInfos[u16InfoIndex].u32RegionIndex = 0;
				u16InfoIndex++;
			}
		}
		PRINT_DEBUG("Allocating local Vulkan memory for buffers used by renderer");
		size_t numberOfAllocatedMemories;
		if (alloc_shared_vulkan_memory(sizeof(localBufferInfos) / sizeof(localBufferInfos[0]),
				localBufferInfos,
				VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
				numberOfAllocatedMemories,
				localBufferMemories,
				bufferAllocs) == VK_SUCCESS) {
			PRINT_LN(numberOfAllocatedMemories);
			std::unique_ptr<void*[]> bufferMemoryPointers = std::make_unique<void*[]>(numberOfAllocatedMemories);
			size_t memoryIndex = 0;
			for (; memoryIndex < numberOfAllocatedMemories; memoryIndex++)
				if (!localBufferMemories[memoryIndex].map(0, 0, VK_WHOLE_SIZE, &bufferMemoryPointers[memoryIndex])) {
					RE_FATAL_ERROR("Failed to map buffer memory at index ", memoryIndex, " to host");
					break;
				}
			if (memoryIndex == numberOfAllocatedMemories) {
				uint16_t u16InfoIndex = 0;
				for (uint8_t u8BufferTypeIndex = 0; u8BufferTypeIndex < u8BufferTypeCount; u8BufferTypeIndex++)
					for (uint8_t u8FrameInFlightIndex = 0; u8FrameInFlightIndex < RE_VK_FRAMES_IN_FLIGHT; u8FrameInFlightIndex++) {
						const size_t indexToMemory = bufferAllocs[u8BufferTypeIndex].indexToMemory;
						switch (u8BufferTypeIndex) {
							case u8CameraIndex:
								if (u8FrameInFlightIndex)
									continue;
								{
									pCameraBufferMemory = std::addressof(localBufferMemories[indexToMemory]);
									camerasShaderData[0] = reinterpret_cast<CameraShaderData*>(reinterpret_cast<uint8_t*>(bufferMemoryPointers[indexToMemory]) + bufferAllocs[u16InfoIndex].vk_memoryOffset);
									uint8_t *const pFirstByte = reinterpret_cast<uint8_t*>(camerasShaderData[0]);
									for (uint16_t u16CameraDataIndex = 1; u16CameraDataIndex < get_max_camera_count() * RE_VK_FRAMES_IN_FLIGHT; u16CameraDataIndex++)
										camerasShaderData[u16CameraDataIndex] = reinterpret_cast<CameraShaderData*>(pFirstByte + next_multiple_inclusive<VkDeviceSize>(sizeof(CameraShaderData), vk_uniformBufferAlignment) * u16CameraDataIndex);
								}
								break;
							case u8RawGameObjectIndex:
								apaGameObjectBufferInstanceData[u8FrameInFlightIndex] = reinterpret_cast<RawGameObjectShaderData*>(reinterpret_cast<uint8_t*>(bufferMemoryPointers[indexToMemory]) + bufferAllocs[u16InfoIndex].vk_memoryOffset);
								break;
							case u8GameObjectIndex:
							case u8SortableDepthIndex:
								break;
							[[unlikely]] default:
								RE_ABORT("Unknown buffer type index for allocating memory: ", u8BufferTypeIndex);
						}
						u16InfoIndex++;
					}
				return true;
			}
			localBufferMemories.reset();
		} else
			RE_FATAL_ERROR("Failed to allocate memory for Vulkan buffers residing on the CPU");
		return false;
	}

	void free_memory_for_buffers_renderer() {
		PRINT_DEBUG("Freeing Vulkan memory used for buffers in renderer");
		localBufferMemories.reset();
	}

}