#include "RE_GPU_Internal.hpp"

namespace RE {

#define RE_VK_MIN_MEMORY_ALLOCS 4096
#define RE_VK_MIN_MEMORY_HEAP_BYTES 100000000
	
	std::vector<std::tuple<VkMemoryHeap, VkDeviceSize>> vulkanMemoryHeaps;
	std::vector<VkMemoryType> vulkanMemoryTypes;
	uint32_t u32VulkanMemoryAllocCount = 0;

	bool does_gpu_support_memory(const VkPhysicalDevice vk_hPhysicalDevice, const VkPhysicalDeviceLimits &vk_rPhysicalDeviceLimits, std::queue<std::string> &rMissingFeatures) {
		VkPhysicalDeviceMemoryProperties vk_physicalDeviceMemoryInfo;
		vkGetPhysicalDeviceMemoryProperties(vk_hPhysicalDevice, &vk_physicalDeviceMemoryInfo);
		VkDeviceSize vk_a2MemoryHeapSizes[2] = {};
		bool a2bMemoryTypesSupported[2] = {};
		for (uint32_t u32TypeIndex = 0; u32TypeIndex < vk_physicalDeviceMemoryInfo.memoryTypeCount; u32TypeIndex++) {
			const VkDeviceSize vk_memorySize = std::clamp<VkDeviceSize>(vk_physicalDeviceMemoryInfo.memoryHeaps[vk_physicalDeviceMemoryInfo.memoryTypes[u32TypeIndex].heapIndex].size, 0, RE_VK_MIN_MEMORY_HEAP_BYTES);
			if ((vk_physicalDeviceMemoryInfo.memoryTypes[u32TypeIndex].propertyFlags & VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT) 
					|| (vk_physicalDeviceMemoryInfo.memoryHeaps[vk_physicalDeviceMemoryInfo.memoryTypes[u32TypeIndex].heapIndex].flags & VK_MEMORY_HEAP_DEVICE_LOCAL_BIT)) {
				a2bMemoryTypesSupported[0] = true;
				if (vk_a2MemoryHeapSizes[0] < RE_VK_MIN_MEMORY_HEAP_BYTES)
					vk_a2MemoryHeapSizes[0] += vk_memorySize;
			}
			if ((vk_physicalDeviceMemoryInfo.memoryTypes[u32TypeIndex].propertyFlags & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT) != 0) {
				a2bMemoryTypesSupported[1] = true;
				if (vk_a2MemoryHeapSizes[1] < RE_VK_MIN_MEMORY_HEAP_BYTES)
					vk_a2MemoryHeapSizes[1] += vk_memorySize;
			}
		}
		if (!a2bMemoryTypesSupported[0])
			rMissingFeatures.emplace("There's no GPU memory available");
		else if (vk_a2MemoryHeapSizes[0] < RE_VK_MIN_MEMORY_HEAP_BYTES)
			rMissingFeatures.push(append_to_string("The GPU's memory should contain at least ", RE_VK_MIN_MEMORY_HEAP_BYTES, " bytes, but has only ", vk_a2MemoryHeapSizes[0]));
		if (!a2bMemoryTypesSupported[1])
			rMissingFeatures.emplace("There's no memory exposed to the CPU");
		else if (vk_a2MemoryHeapSizes[1] < RE_VK_MIN_MEMORY_HEAP_BYTES)
			rMissingFeatures.push(append_to_string("The memory exposed to the CPU should contain at least ", RE_VK_MIN_MEMORY_HEAP_BYTES, " bytes, but has only ", vk_a2MemoryHeapSizes[1]));
		if (!vk_rPhysicalDeviceLimits.maxMemoryAllocationCount)
			rMissingFeatures.emplace("The memory allocation limit by the physical device is zero, which disallows any allocation");
		return std::all_of(std::begin(a2bMemoryTypesSupported), std::end(a2bMemoryTypesSupported), [](const bool bValue) -> bool {return bValue;}) 
				&& std::all_of(std::begin(vk_a2MemoryHeapSizes), std::end(vk_a2MemoryHeapSizes), [](const VkDeviceSize vk_size) {return vk_size > RE_VK_MIN_MEMORY_HEAP_BYTES;}) 
				&& vk_rPhysicalDeviceLimits.maxMemoryAllocationCount;
	}

	int32_t rate_gpu_memory_capacity(const VkPhysicalDevice vk_hPhysicalDevice, const VkPhysicalDeviceProperties &vk_rPhysicalDeviceProperties) {
		int32_t i32Score;
		if (vk_rPhysicalDeviceProperties.limits.maxMemoryAllocationCount >= RE_VK_MIN_MEMORY_ALLOCS)
			i32Score = static_cast<int32_t>(std::round(static_cast<double>(vk_rPhysicalDeviceProperties.limits.maxMemoryAllocationCount - RE_VK_MIN_MEMORY_ALLOCS) / std::numeric_limits<uint32_t>::max() * 1000.0));
		else
			i32Score = -2000;
		VkPhysicalDeviceMemoryProperties vk_physicalDeviceMemoryInfo;
		vkGetPhysicalDeviceMemoryProperties(vk_hPhysicalDevice, &vk_physicalDeviceMemoryInfo);
		for (uint32_t u32HeapIndex = 0; u32HeapIndex < vk_physicalDeviceMemoryInfo.memoryHeapCount; u32HeapIndex++)
			i32Score += vk_physicalDeviceMemoryInfo.memoryHeaps[u32HeapIndex].size / RE_VK_MIN_MEMORY_HEAP_BYTES;
		{ // Test physical memory regions
			bool a2bPhysicalMemorySupports[2] = {};
			for (uint32_t u32TypeIndex = 0; u32TypeIndex < vk_physicalDeviceMemoryInfo.memoryTypeCount; u32TypeIndex++) {
				const uint32_t u32HeapIndex = vk_physicalDeviceMemoryInfo.memoryTypes[u32TypeIndex].heapIndex;
				const bool bPhysicalMemory = (vk_physicalDeviceMemoryInfo.memoryTypes[u32TypeIndex].propertyFlags & VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT) 
						&& (vk_physicalDeviceMemoryInfo.memoryHeaps[u32HeapIndex].flags & VK_MEMORY_HEAP_DEVICE_LOCAL_BIT);
				const bool bHostMemory = (vk_physicalDeviceMemoryInfo.memoryTypes[u32TypeIndex].propertyFlags & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT) 
						&& !(vk_physicalDeviceMemoryInfo.memoryHeaps[u32HeapIndex].flags & VK_MEMORY_HEAP_DEVICE_LOCAL_BIT);
				if (vk_rPhysicalDeviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_CPU && bPhysicalMemory && bHostMemory) {
					a2bPhysicalMemorySupports[0] = true;
					a2bPhysicalMemorySupports[1] = true;
					break;
				} else {
					if (bPhysicalMemory && !bHostMemory)
						a2bPhysicalMemorySupports[0] = true;
					else if (!bPhysicalMemory && bHostMemory)
						a2bPhysicalMemorySupports[1] = true;
				}
			}
			if (std::all_of(std::begin(a2bPhysicalMemorySupports), std::end(a2bPhysicalMemorySupports), [](const bool bValue) -> bool {return bValue;}))
				i32Score += 1000;
		}
		// Test memory properties
		constexpr VkMemoryPropertyFlags vk_aeSuitableMemoryProperties[] = {
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT | VK_MEMORY_PROPERTY_LAZILY_ALLOCATED_BIT
		};
		constexpr uint32_t u32SuitableMemoryCount = sizeof(vk_aeSuitableMemoryProperties) / sizeof(vk_aeSuitableMemoryProperties[0]);
		bool abSupportedMemoryProperties[u32SuitableMemoryCount] = {};
		for (uint32_t u32MemoryTypeIndex = 0; u32MemoryTypeIndex < vk_physicalDeviceMemoryInfo.memoryTypeCount; u32MemoryTypeIndex++)
			for (uint32_t u32SuitableMemoryIndex = 0; u32SuitableMemoryIndex < u32SuitableMemoryCount; u32SuitableMemoryIndex++)
				if (are_bits_true<VkMemoryPropertyFlags>(vk_physicalDeviceMemoryInfo.memoryTypes[u32MemoryTypeIndex].propertyFlags, vk_aeSuitableMemoryProperties[u32SuitableMemoryIndex]))
					if (!abSupportedMemoryProperties[u32SuitableMemoryIndex]) {
						abSupportedMemoryProperties[u32SuitableMemoryIndex] = true;
						switch (u32SuitableMemoryIndex) {
							case 0:
								i32Score += 400;
								break;
							case 1:
								i32Score += 1000;
								break;
							case 2:
								i32Score += 1000;
								break;
							default:
								break;
						}
						if (std::all_of(std::begin(abSupportedMemoryProperties), std::end(abSupportedMemoryProperties), [](const bool bValue) -> bool {return bValue;}))
							goto RETURN_DEVICE_SCORE;
					}
		RETURN_DEVICE_SCORE:
			return i32Score;
	}

	void fetch_gpu_memory_info() {
		VkPhysicalDeviceMemoryProperties vk_physicalDeviceMemoryInfo;
		vkGetPhysicalDeviceMemoryProperties(vk_hPhysicalDeviceSelected, &vk_physicalDeviceMemoryInfo);
		vulkanMemoryHeaps.reserve(vk_physicalDeviceMemoryInfo.memoryHeapCount);
		for (uint32_t u32HeapIndex = 0; u32HeapIndex < vk_physicalDeviceMemoryInfo.memoryHeapCount; u32HeapIndex++)
			vulkanMemoryHeaps.emplace_back(vk_physicalDeviceMemoryInfo.memoryHeaps[u32HeapIndex], vk_physicalDeviceMemoryInfo.memoryHeaps[u32HeapIndex].size / 5 * 4);
		vulkanMemoryTypes.reserve(vk_physicalDeviceMemoryInfo.memoryTypeCount);
		for (uint32_t u32TypeIndex = 0; u32TypeIndex < vk_physicalDeviceMemoryInfo.memoryTypeCount; u32TypeIndex++)
			vulkanMemoryTypes.push_back(vk_physicalDeviceMemoryInfo.memoryTypes[u32TypeIndex]);

	}

	bool does_memory_type_exist(const VkMemoryPropertyFlags vk_eProperties) {
		return std::any_of(vulkanMemoryTypes.begin(), vulkanMemoryTypes.end(), [=](const VkMemoryType &vk_rType) -> bool {
				return are_bits_true<VkMemoryPropertyFlags>(vk_rType.propertyFlags, vk_eProperties);
			});
	}

	bool does_have_vulkan_memory_type(const VulkanMemoryType eMemoryType) {
		switch (eMemoryType) {
			case RE_VK_GPU_RAM:
				return std::any_of(vulkanMemoryTypes.begin(), vulkanMemoryTypes.end(), [](const VkMemoryType &vk_rType) -> bool {
						return (vk_rType.propertyFlags & VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT) && (std::get<VkMemoryHeap>(vulkanMemoryHeaps[vk_rType.heapIndex]).flags & VK_MEMORY_HEAP_DEVICE_LOCAL_BIT);
					});
			case RE_VK_CPU_RAM:
				return std::any_of(vulkanMemoryTypes.begin(), vulkanMemoryTypes.end(), [](const VkMemoryType &vk_rType) -> bool {
						return (vk_rType.propertyFlags & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT) && !(std::get<VkMemoryHeap>(vulkanMemoryHeaps[vk_rType.heapIndex]).flags & VK_MEMORY_HEAP_DEVICE_LOCAL_BIT);
					});
			case RE_VK_SHARED_RAM:
				return std::any_of(vulkanMemoryTypes.begin(), vulkanMemoryTypes.end(), [](const VkMemoryType &vk_rType) -> bool {
						return are_bits_true<VkMemoryPropertyFlags, VkMemoryPropertyFlags, VkMemoryPropertyFlags>(vk_rType.propertyFlags, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
					});
			default:
				RE_FATAL_ERROR("Unknown Vulkan memory type queried ", std::hex, eMemoryType);
				return false;
		}
	}

	uint32_t get_remaining_vulkan_allocations() {
		return vk_physicalDeviceProperties.limits.maxMemoryAllocationCount - u32VulkanMemoryAllocCount;
	}

}
