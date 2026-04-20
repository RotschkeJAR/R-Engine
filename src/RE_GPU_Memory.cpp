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

	void fetch_vulkan_memory_requirements(const VulkanStorageObject vulkanStorageObject, VkMemoryRequirements2 &vk_rMemoryRequirements) {
		const auto storageObjectIndex = vulkanStorageObject.index();
		PRINT_DEBUG("Fetching Vulkan storage object referenced by index ", storageObjectIndex);
		switch (storageObjectIndex) {
			case 0:
				{
					const VkBufferMemoryRequirementsInfo2 vk_bufferInfos = {
						.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_REQUIREMENTS_INFO_2,
						.pNext = nullptr,
						.buffer = std::get<VkBuffer>(vulkanStorageObject)
					};
					PRINT_DEBUG("Fetching memory requirements of Vulkan image ", vk_bufferInfos.buffer);
					vkGetBufferMemoryRequirements2(vk_hDevice, &vk_bufferInfos, std::addressof(vk_rMemoryRequirements));
				}
				break;
			case 1:
				{
					const VkImageMemoryRequirementsInfo2 vk_imageInfos = {
						.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_REQUIREMENTS_INFO_2,
						.pNext = nullptr,
						.image = std::get<VkImage>(vulkanStorageObject)
					};
					PRINT_DEBUG("Fetching memory requirements of Vulkan image ", vk_imageInfos.image);
					vkGetImageMemoryRequirements2(vk_hDevice, &vk_imageInfos, std::addressof(vk_rMemoryRequirements));
				}
				break;
			[[unlikely]] default:
				RE_ABORT("There's no Vulkan storage object for fetching memory requirements or the index is invalid");
		}
	}

	bool bind_vulkan_memory(const VulkanStorageObject vulkanStorageObject, const VkDeviceMemory vk_hMemory, const VkDeviceSize vk_memoryOffset) {
		const auto storageObjectIndex = vulkanStorageObject.index();
		PRINT_DEBUG("Binding Vulkan storage object referenced by index ", storageObjectIndex, " to memory");
		switch (storageObjectIndex) {
			case 0:
				{
					const VkBindBufferMemoryInfo vk_bufferBindInfo = {
						.sType = VK_STRUCTURE_TYPE_BIND_BUFFER_MEMORY_INFO,
						.pNext = nullptr,
						.buffer = std::get<VkBuffer>(vulkanStorageObject),
						.memory = vk_hMemory,
						.memoryOffset = vk_memoryOffset
					};
					PRINT_DEBUG("Binding Vulkan buffer ", vk_bufferBindInfo.buffer, " to memory ", vk_hMemory, " at byte ", vk_memoryOffset);
					const VkResult vk_eResult = vkBindBufferMemory2(vk_hDevice, 1, &vk_bufferBindInfo);
					if (vk_eResult == VK_SUCCESS)
						return true;
					else
						RE_ERROR("Failed binding Vulkan memory ", vk_bufferBindInfo.memory, " to the buffer ", vk_bufferBindInfo.buffer);
				}
				break;
			case 1:
				{
					const VkBindImageMemoryInfo vk_imageBindInfo = {
						.sType = VK_STRUCTURE_TYPE_BIND_IMAGE_MEMORY_INFO,
						.pNext = nullptr,
						.image = std::get<VkImage>(vulkanStorageObject),
						.memory = vk_hMemory,
						.memoryOffset = vk_memoryOffset
					};
					PRINT_DEBUG("Binding Vulkan image ", vk_imageBindInfo.image, " to memory ", vk_hMemory, " at byte ", vk_memoryOffset);
					const VkResult vk_eResult = vkBindImageMemory2(vk_hDevice, 1, &vk_imageBindInfo);
					if (vk_eResult == VK_SUCCESS)
						return true;
					else
						RE_ERROR("Failed binding Vulkan memory ", vk_imageBindInfo.memory, " to the image ", vk_imageBindInfo.image);
				}
				break;
			[[unlikely]] default:
				RE_ABORT("There's no Vulkan storage object to bind memory to or the index is invalid");
		}
		return false;
	}

	bool alloc_shared_vulkan_memory(const uint32_t u32SharedMemoryInfoCount, const SharedVulkanMemoryInfo *const paSharedMemoryInfos, const VkMemoryPropertyFlags vk_mMemoryProperties, size_t &rAllocatedMemoryCount, std::unique_ptr<VulkanMemory[]> &rAllocatedMemory, bool *pbVulkanStorageObjectsUnbound) {
		if (pbVulkanStorageObjectsUnbound)
			*pbVulkanStorageObjectsUnbound = true;
		std::vector<bool> infosProcessed;
		infosProcessed.resize(u32SharedMemoryInfoCount);
		std::fill(infosProcessed.begin(), infosProcessed.end(), false);
		struct MemoryDetails final {
			VkDeviceSize vk_alignment;
			VkDeviceSize vk_size;
			uint64_t u64SeparationIndex;
			uint8_t u8MemoryTypeIndex;
		};
		std::unique_ptr<MemoryDetails[]> detailsPerInfo;
		{
			VkMemoryRequirements2 vk_memoryRequirements2;
			vk_memoryRequirements2.sType = VK_STRUCTURE_TYPE_MEMORY_REQUIREMENTS_2;
			vk_memoryRequirements2.pNext = nullptr;
			const VkMemoryRequirements &vk_rMemoryRequirements = vk_memoryRequirements2.memoryRequirements;
			uint64_t u64CurrentSeparationIndex = 0;
			for (uint32_t u32CurrentInfoIndex = 0; u32CurrentInfoIndex < u32SharedMemoryInfoCount; u32CurrentInfoIndex++) {
				if (infosProcessed[u32CurrentInfoIndex] == true)
					continue;
				fetch_vulkan_memory_requirements(paSharedMemoryInfos[u32CurrentInfoIndex].vulkanStorageObject, vk_memoryRequirements2);
				auto memoryType = find_vulkan_memory_type(vk_mMemoryProperties, vk_rMemoryRequirements.memoryTypeBits);
				if (!memoryType.has_value()) {
					RE_ERROR("Failed to find a suitable Vulkan memory type for Vulkan storage object at info index ", u32CurrentInfoIndex);
					return false;
				}
				detailsPerInfo[u32CurrentInfoIndex].vk_alignment = vk_rMemoryRequirements.alignment;
				detailsPerInfo[u32CurrentInfoIndex].vk_size = vk_rMemoryRequirements.size;
				detailsPerInfo[u32CurrentInfoIndex].u64SeparationIndex = u64CurrentSeparationIndex;
				detailsPerInfo[u32CurrentInfoIndex].u8MemoryTypeIndex = *memoryType;
				infosProcessed[u32CurrentInfoIndex] = true;
				if (vk_rMemoryRequirements.size > vk_maxMemorySize)
					RE_WARNING("The required size for the Vulkan storage object at info index ", u32CurrentInfoIndex, " is beyond the maximum allocation size");
				const uint32_t u32CurrentRegionIndex = paSharedMemoryInfos[u32CurrentInfoIndex].u32RegionIndex;
				for (uint32_t u32SubcurrentInfoIndex = u32CurrentInfoIndex + 1; u32SubcurrentInfoIndex < u32SharedMemoryInfoCount; u32SubcurrentInfoIndex++) {
					if (infosProcessed[u32SubcurrentInfoIndex] == true || paSharedMemoryInfos[u32SubcurrentInfoIndex].u32RegionIndex != u32CurrentRegionIndex)
						continue;
					fetch_vulkan_memory_requirements(paSharedMemoryInfos[u32SubcurrentInfoIndex].vulkanStorageObject, vk_memoryRequirements2);
					if ((vk_rMemoryRequirements.memoryTypeBits & (1 << static_cast<uint32_t>(*memoryType))) == 0)
						continue;
					detailsPerInfo[u32SubcurrentInfoIndex].vk_alignment = vk_rMemoryRequirements.alignment;
					detailsPerInfo[u32SubcurrentInfoIndex].vk_size = vk_rMemoryRequirements.size;
					detailsPerInfo[u32SubcurrentInfoIndex].u64SeparationIndex = u64CurrentSeparationIndex;
					detailsPerInfo[u32SubcurrentInfoIndex].u8MemoryTypeIndex = *memoryType;
					infosProcessed[u32SubcurrentInfoIndex] = true;
				}
			}
		}
		std::fill(infosProcessed.begin(), infosProcessed.end(), false);
		std::list<VulkanMemory> allocatedMemories;
		for (uint32_t u32CurrentInfoIndex = 0; u32CurrentInfoIndex < u32SharedMemoryInfoCount; u32CurrentInfoIndex++) {
			if (infosProcessed[u32CurrentInfoIndex] == true)
				continue;
			VkDeviceSize vk_allocSize = detailsPerInfo[u32CurrentInfoIndex].vk_size;
			struct OffsetPerInfo final {
				VkDeviceSize vk_offset;
				uint32_t u32IndexToInfo;
			};
			std::deque<OffsetPerInfo> offsetPerInfo;
			const uint64_t u64CurrentSeparationIndex = detailsPerInfo[u32CurrentInfoIndex].u64SeparationIndex;
			offsetPerInfo.emplace_back(0, u32CurrentInfoIndex);
			infosProcessed[u32CurrentInfoIndex] = true;
			for (uint32_t u32SubcurrentInfoIndex = u32CurrentInfoIndex; u32SubcurrentInfoIndex < u32SharedMemoryInfoCount; u32SubcurrentInfoIndex++) {
				if (vk_allocSize > vk_maxMemorySize)
					break;
				if (infosProcessed[u32SubcurrentInfoIndex] == true || detailsPerInfo[u32SubcurrentInfoIndex].u64SeparationIndex != u64CurrentSeparationIndex)
					continue;
				const VkDeviceSize vk_size = detailsPerInfo[u32SubcurrentInfoIndex].vk_size,
					vk_alignment = detailsPerInfo[u32SubcurrentInfoIndex].vk_alignment,
					vk_offset = next_multiple_inclusive<VkDeviceSize>(vk_allocSize, vk_alignment);
				offsetPerInfo.emplace_back(vk_offset, u32SubcurrentInfoIndex);
				vk_allocSize = vk_offset + vk_size;
				infosProcessed[u32SubcurrentInfoIndex] = true;
			}
			VulkanMemory vulkanMemory;
			if (offsetPerInfo.size() == 1)
				goto DEDICATED_ALLOCATION;
			{
				VkResult vk_eResult = vulkanMemory.alloc(vk_allocSize, detailsPerInfo[u32CurrentInfoIndex].u8MemoryTypeIndex);
				if (vk_eResult == VK_ERROR_TOO_MANY_OBJECTS) {
					RE_ERROR("Failed to allocate Vulkan memory due to exceeding maximum allocations");
					return false;
				}
				while (vk_eResult != VK_SUCCESS) {
					switch (offsetPerInfo.size()) {
						case 0:
						case 1:
							RE_ERROR("Failed to allocate Vulkan memory for one or more Vulkan storage objects");
							return false;
						case 2:
							infosProcessed[offsetPerInfo.back().u32IndexToInfo] = false;
							offsetPerInfo.pop_back();
							goto DEDICATED_ALLOCATION;
						default:
							infosProcessed[offsetPerInfo.back().u32IndexToInfo] = false;
							offsetPerInfo.pop_back();
							vk_allocSize = offsetPerInfo.back().vk_offset + detailsPerInfo[offsetPerInfo.back().u32IndexToInfo].vk_size;
							vk_eResult = vulkanMemory.alloc(vk_allocSize, detailsPerInfo[u32CurrentInfoIndex].u8MemoryTypeIndex);
							break;
					}
				}
				std::vector<VkBindBufferMemoryInfo> bindBufferInfos;
				std::vector<VkBindImageMemoryInfo> bindImageInfos;
				bindImageInfos.reserve(offsetPerInfo.size() / 5 + 5);
				bindBufferInfos.reserve(bindImageInfos.capacity());
				for (const OffsetPerInfo &rOffsetPerInfo : offsetPerInfo) {
					const uint32_t u32IndexToInfo = rOffsetPerInfo.u32IndexToInfo;
					const auto &rVulkanStorageObject = paSharedMemoryInfos[u32IndexToInfo].vulkanStorageObject;
					switch (rVulkanStorageObject.index()) {
						case 0:
							bindBufferInfos.emplace_back(VK_STRUCTURE_TYPE_BIND_BUFFER_MEMORY_INFO, nullptr, std::get<VkBuffer>(rVulkanStorageObject), vulkanMemory.get(), rOffsetPerInfo.vk_offset);
							break;
						case 1:
							bindImageInfos.emplace_back(VK_STRUCTURE_TYPE_BIND_IMAGE_MEMORY_INFO, nullptr, std::get<VkImage>(rVulkanStorageObject), vulkanMemory.get(), rOffsetPerInfo.vk_offset);
							break;
						[[unlikely]] default:
							RE_ABORT("");
					}
				}
				if (!bindBufferInfos.empty())
					if (vkBindBufferMemory2(vk_hDevice, static_cast<uint32_t>(bindBufferInfos.size()), bindBufferInfos.data()) != VK_SUCCESS) {
						RE_ERROR("");
						if (pbVulkanStorageObjectsUnbound && bindBufferInfos.size() > 1)
							*pbVulkanStorageObjectsUnbound = false;
						return false;
					}
				if (!bindImageInfos.empty())
					if (vkBindImageMemory2(vk_hDevice, static_cast<uint32_t>(bindImageInfos.size()), bindImageInfos.data()) != VK_SUCCESS) {
						RE_ERROR("");
						if (pbVulkanStorageObjectsUnbound && bindImageInfos.size() > 1)
							*pbVulkanStorageObjectsUnbound = false;
						return false;
					}
			}
			goto ALLOCATION_SUCCESS;

		DEDICATED_ALLOCATION:
			{
				const auto &rVulkanStorageObject = paSharedMemoryInfos[offsetPerInfo.front().u32IndexToInfo].vulkanStorageObject;
				switch (rVulkanStorageObject.index()) {
					case 0:
						if (vulkanMemory.alloc_for_buffer(std::get<VkBuffer>(rVulkanStorageObject), vk_mMemoryProperties) != VK_SUCCESS) {
							RE_ERROR("");
							return false;
						}
						break;
					case 1:
						if (vulkanMemory.alloc_for_image(std::get<VkImage>(rVulkanStorageObject), vk_mMemoryProperties) != VK_SUCCESS) {
							RE_ERROR("");
							return false;
						}
						break;
					[[unlikely]] default:
						RE_ABORT("");
				}
			}

		ALLOCATION_SUCCESS:
			if (pbVulkanStorageObjectsUnbound)
				*pbVulkanStorageObjectsUnbound = false;
		}
		rAllocatedMemoryCount = allocatedMemories.size();
		rAllocatedMemory = std::make_unique<VulkanMemory[]>(rAllocatedMemoryCount);
		size_t memoryIndex = 0;
		while (!allocatedMemories.empty()) {
			rAllocatedMemory[memoryIndex] = std::move(allocatedMemories.front());
			allocatedMemories.pop_front();
			memoryIndex++;
		}
		return true;
	}

	std::optional<uint8_t> find_vulkan_memory_type(const VkMemoryPropertyFlags vk_mProperties, const uint32_t m32MemoryTypeBits, uint8_t *const pu8Mismatches) {
		std::optional<uint8_t> selectedMemoryTypeIndex;
		uint8_t u8LowestMismatchCount = std::numeric_limits<uint8_t>::max();
		for (uint8_t u8MemoryTypeIndex = 0; u8MemoryTypeIndex < u8MemoryTypeCount; u8MemoryTypeIndex++) {
			if ((vulkanMemoryTypes[u8MemoryTypeIndex].propertyFlags & vk_mProperties) == vk_mProperties && (m32MemoryTypeBits & (1 << u8MemoryTypeIndex)) != 0) {
				uint8_t u8Mismatches = std::popcount(vulkanMemoryTypes[u8MemoryTypeIndex].propertyFlags ^ vk_mProperties);
				if (u8Mismatches < u8LowestMismatchCount) {
					selectedMemoryTypeIndex = u8MemoryTypeIndex;
					u8LowestMismatchCount = u8Mismatches;
				}
			}
		}
		if (pu8Mismatches && selectedMemoryTypeIndex.has_value())
			*pu8Mismatches = u8LowestMismatchCount;
		return selectedMemoryTypeIndex;
	}

	bool do_memory_properties_exist(const VkMemoryPropertyFlags vk_mProperties, uint8_t *const pu8Mismatches) {
		PRINT_DEBUG("Checking if any memory type supports queried properties (", std::hex, vk_mProperties, ") and writes mismatch count to ", pu8Mismatches);
		return find_vulkan_memory_type(vk_mProperties, 0xFFFFFFFF, pu8Mismatches).has_value();
	}

	bool is_staging_before_gpu_use_necessary() {
		return SELECTED_PHYSICAL_VULKAN_DEVICE_TYPE != VK_PHYSICAL_DEVICE_TYPE_CPU 
				&& SELECTED_PHYSICAL_VULKAN_DEVICE_TYPE != VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU;
	}

	uint32_t get_remaining_vulkan_allocations() {
		return u32MaxMemoryAllocs - u32VulkanMemoryAllocCount;
	}

}
