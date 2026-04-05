#include "RE_GPU_Internal.hpp"

namespace RE {

	std::unique_ptr<std::tuple<VkMemoryHeap, VkDeviceSize>[]> vulkanMemoryHeaps;
	std::unique_ptr<VkMemoryType[]> vulkanMemoryTypes;
	uint32_t u32VulkanMemoryAllocCount = 0;
	uint8_t u8MemoryHeapCount, u8MemoryTypeCount;

	void alloc_gpu_memory_info() {
		PRINT_DEBUG("Fetching memory properties of selected physical Vulkan device");
		VkPhysicalDeviceMemoryProperties2 vk_physicalDeviceMemoryInfo;
		vk_physicalDeviceMemoryInfo.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MEMORY_PROPERTIES_2;
		vk_physicalDeviceMemoryInfo.pNext = nullptr;
		vkGetPhysicalDeviceMemoryProperties2(SELECTED_PHYSICAL_VULKAN_DEVICE, &vk_physicalDeviceMemoryInfo);
		u8MemoryHeapCount = static_cast<uint8_t>(vk_physicalDeviceMemoryInfo.memoryProperties.memoryHeapCount);
		vulkanMemoryHeaps = std::make_unique<std::tuple<VkMemoryHeap, VkDeviceSize>[]>(u8MemoryHeapCount);
		u8MemoryTypeCount = static_cast<uint8_t>(vk_physicalDeviceMemoryInfo.memoryProperties.memoryTypeCount);
		vulkanMemoryTypes = std::make_unique<VkMemoryType[]>(u8MemoryTypeCount);
		for (uint32_t u32HeapIndex = 0; u32HeapIndex < vk_physicalDeviceMemoryInfo.memoryProperties.memoryHeapCount; u32HeapIndex++) {
			std::get<VkMemoryHeap>(vulkanMemoryHeaps[u32HeapIndex]) = vk_physicalDeviceMemoryInfo.memoryProperties.memoryHeaps[u32HeapIndex];
			if (SELECTED_PHYSICAL_VULKAN_DEVICE_TYPE == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
				std::get<VkDeviceSize>(vulkanMemoryHeaps[u32HeapIndex]) = vk_physicalDeviceMemoryInfo.memoryProperties.memoryHeaps[u32HeapIndex].size / 4 * 3;
			else
				std::get<VkDeviceSize>(vulkanMemoryHeaps[u32HeapIndex]) = vk_physicalDeviceMemoryInfo.memoryProperties.memoryHeaps[u32HeapIndex].size / 2;
		}
		for (uint32_t u32TypeIndex = 0; u32TypeIndex < vk_physicalDeviceMemoryInfo.memoryProperties.memoryTypeCount; u32TypeIndex++)
			vulkanMemoryTypes[u32TypeIndex] = vk_physicalDeviceMemoryInfo.memoryProperties.memoryTypes[u32TypeIndex];
	}

	void free_gpu_memory_info() {
		PRINT_DEBUG("Freeing GPU memory information");
		vulkanMemoryHeaps.reset();
		vulkanMemoryTypes.reset();
	}

	bool alloc_shared_vulkan_memory(const uint32_t u32SharedMemoryInfoCount, const SharedVulkanMemoryInfo *const paSharedMemoryInfos, uint32_t &ru32AllocatedMemoryCount, std::unique_ptr<VulkanMemory[]> &rAllocatedMemory) {
		std::vector<bool> sharedMemoryInfosProcessed;
		sharedMemoryInfosProcessed.resize(u32SharedMemoryInfoCount - 1);
		std::fill(sharedMemoryInfosProcessed.begin(), sharedMemoryInfosProcessed.end(), false);
		for (uint32_t u32CurrentInfoIndex = 0; u32CurrentInfoIndex < u32SharedMemoryInfoCount; u32CurrentInfoIndex++) {
			if (u32CurrentInfoIndex > 0 && sharedMemoryInfosProcessed[sharedMemoryInfosProcessed - 1] == true)
				continue;
		}
	}

	bool do_memory_properties_exist(const VkMemoryPropertyFlags vk_mProperties, uint8_t *const pu8Mismatches) {
		uint8_t u8LowestMismatchCount = std::numeric_limits<uint8_t>::max();
		for (uint8_t u8MemoryTypeIndex = 0; u8MemoryTypeIndex < u8MemoryTypeCount; u8MemoryTypeIndex++) {
			if ((vulkanMemoryTypes[u8MemoryTypeIndex].propertyFlags & vk_mProperties) == vk_mProperties) {
				uint8_t u8Mismatches = std::popcount(vulkanMemoryTypes[u8MemoryTypeIndex].propertyFlags ^ vk_mProperties);
				if (u8Mismatches < u8LowestMismatchCount)
					u8LowestMismatchCount = u8Mismatches;
			}
		}
		if (pu8Mismatches)
			*pu8Mismatches = u8LowestMismatchCount;
		return u8LowestMismatchCount != std::numeric_limits<uint8_t>::max();
	}

	bool is_staging_before_gpu_use_necessary() {
		return SELECTED_PHYSICAL_VULKAN_DEVICE_TYPE != VK_PHYSICAL_DEVICE_TYPE_CPU 
				&& SELECTED_PHYSICAL_VULKAN_DEVICE_TYPE != VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU;
	}

	uint32_t get_remaining_vulkan_allocations() {
		return u32MaxMemoryAllocs - u32VulkanMemoryAllocCount;
	}

}
