#include "RE_Renderer_WindowFrame_Memory.hpp"

namespace RE {

	static std::unique_ptr<VulkanMemory[]> std_windowFrameResourceMemoriesLocal,
		std_windowFrameResourceMemories;

	bool alloc_window_frame_memory() {
		constexpr size_t sBufferIndex = 0;
		const SharedVulkanMemoryInfo aLocalMemoryInfos[] = {
			{
				.vulkanStorageObject = vk_hWindowFrameBuffer,
				.u32RegionIndex = 0
			}
		};
		constexpr size_t sLocalResourcesCount = sizeof(aLocalMemoryInfos) / sizeof(aLocalMemoryInfos[0]);
		VulkanMemoryAllocationInfo aAllocInfos[sLocalResourcesCount];
		size_t sMemoryAllocated;
		if (alloc_shared_vulkan_memory(
				sLocalResourcesCount,
				aLocalMemoryInfos,
				VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
				sMemoryAllocated,
				std_windowFrameResourceMemoriesLocal,
				aAllocInfos) == VK_SUCCESS) {
			std::unique_ptr<void*[]> std_bufferMemories = std::make_unique<void*[]>(sMemoryAllocated);
			size_t sMemoryIndex;
			for (sMemoryIndex = 0; sMemoryIndex < sMemoryAllocated; sMemoryIndex++)
				if (!std_windowFrameResourceMemoriesLocal.map(0, 0, VK_WHOLE_SIZE, &std_bufferMemories[sMemoryIndex]))
					break;
			if (sMemoryIndex == sMemoryAllocated) {
				const size_t sBufferIndexToMemory = aAllocInfos[sBufferIndex].indexToMemory;
				pWindowFrameBufferMemory = std::addressof(std_windowFrameResourceMemoriesLocal[sBufferIndexToMemory]);
				pWindowFrameUniformData = reinterpret_cast<WindowFrameUniformData*>(
						reinterpret_cast<uint8_t*>(std_bufferMemories[sBufferIndexToMemory]) + aAllocInfos[sBufferIndex].vk_memoryOffset);
				pIndirectDrawWindowTitle = reinterpret_cast<VkDrawIndirectCommand*>(
						reinterpret_cast<uint8_t*>(pWindowFrameUniformData) + sizeof(WindowFrameUniformData));
				const SharedVulkanMemoryInfo aMemoryInfos[] = {
					{
						.vulkanStorageObject = vk_hWindowButtonImage,
						.u32RegionIndex = 0
					}
				};
				if (alloc_shared_vulkan_memory(
						sizeof(aMemoryInfos) / sizeof(aMemoryInfos[0]),
						aMemoryInfos,
						VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
						sMemoryAllocated,
						std_windowFrameResourceMemories) == VK_SUCCESS) {
					return true;
				}
				pWindowFrameUniformData = nullptr;
				pIndirectDrawWindowTitle = nullptr;
			}
			std_windowFrameResourceMemoriesLocal.reset();
		}
		return false;
	}

	void free_window_frame_memory() {
		pWindowFrameUniformData = nullptr;
		pIndirectDrawWindowTitle = nullptr;
		std_windowFrameResourceMemories.reset();
		std_windowFrameResourceMemoriesLocal.reset();
	}

}
