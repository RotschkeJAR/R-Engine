#include "RE_Renderer_Buffers_Internal.hpp"

namespace RE {

	static std::unique_ptr<VulkanMemory[]> localBufferMemories;
	
	bool alloc_memory_for_buffers_renderer() {
		PRINT_DEBUG("Filling buffer information for allocating shared memory");
		constexpr uint16_t u16BufferTypeCount = 1,
			u16CameraIndex = 0;
		constexpr uint32_t u32BufferCount = u16BufferTypeCount * RE_VK_FRAMES_IN_FLIGHT;
		SharedVulkanMemoryInfo localBufferInfos[u32BufferCount];
		VulkanMemoryAllocationInfo bufferAllocs[u32BufferCount];
		for (uint16_t u16BufferTypeIndex = 0; u16BufferTypeIndex < u16BufferTypeCount; u16BufferTypeIndex++) {
			for (uint8_t u8FrameInFlightIndex = 0; u8FrameInFlightIndex < RE_VK_FRAMES_IN_FLIGHT; u8FrameInFlightIndex++) {
				const uint32_t u32BufferInfoIndex = u16BufferTypeIndex * u8FrameInFlightIndex;
				switch (u16BufferTypeIndex) {
					case u16CameraIndex:
						localBufferInfos[u32BufferInfoIndex].vulkanStorageObject = vk_ahCameraBuffers[u8FrameInFlightIndex];
						break;
					[[unlikely]] default:
					 	RE_ABORT("Unknown buffer type index for allocating memory: ", u16BufferTypeIndex);
				}
				localBufferInfos[u32BufferInfoIndex].u32RegionIndex = 0;
			}
		}
		PRINT_DEBUG("Allocating local Vulkan memory for buffers used by renderer");
		size_t numberOfAllocatedMemories;
		if (alloc_shared_vulkan_memory(sizeof(localBufferInfos) / sizeof(localBufferInfos[0]),
				VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
				numberOfAllocatedMemories,
				localBufferMemories,
				bufferAllocs) == VK_SUCCESS) {
			std::unique_ptr<void*[]> bufferMemoryPointers = std::make_unique<void*[]>(numberOfAllocatedMemories);
			size_t memoryIndex = 0;
			for (; memoryIndex < numberOfAllocatedMemories; memoryIndex++)
				if (!localBufferMemories.map(0, 0, VK_WHOLE_SIZE, &bufferMemoryPointers[memoryIndex])) {
					RE_FATAL_ERROR("Failed to map buffer memory at index ", memoryIndex, " to host");
					break;
				}
			if (memoryIndex == numberOfAllocatedMemories) {
				for (uint16_t u16BufferTypeIndex = 0; u16BufferTypeIndex < u16BufferTypeCount; u16BufferTypeIndex++)
					for (uint8_t u8FrameInFlightIndex = 0; u8FrameInFlightIndex < RE_VK_FRAMES_IN_FLIGHT; u8FrameInFlightIndex++) {
						const size_t indexToMemory = bufferAllocs[u32BufferInfoIndex].indexToMemory;
						const uint32_t u32BufferInfoIndex = u16BufferTypeIndex * u8FrameInFlightIndex;
						switch (u16BufferTypeIndex) {
							case u16CameraIndex:
								apCameraShaderData[u8FrameInFlightIndex] = reinterpret_cast<CameraShaderData*>(reinterpret_cast<uint8_t*>(bufferMemoryPointers[indexToMemory]) + bufferAllocs[u32BufferInfoIndex].vk_memoryOffset);
								apCameraBufferMemories[u8FrameInFlightIndex] = std::addressof(localBufferMemories[indexToMemory]);
								break;
							[[unlikely]] default:
								RE_ABORT("Unknown buffer type index for allocating memory: ", u16BufferTypeIndex);
						}
					}
				return true;
			}
			localBufferMemories.reset();
		}
		return false;
	}

	void free_memory_for_buffers_renderer() {
		PRINT_DEBUG("Freeing Vulkan memory used for buffers in renderer");
		localBufferMemories.reset();
	}

}