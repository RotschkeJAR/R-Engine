#include "RE_GPU_Internal.hpp"

namespace RE {
	
	VulkanMemory::VulkanMemory() : vk_hMemory(VK_NULL_HANDLE) {}

	VulkanMemory::VulkanMemory(const VkDeviceSize vk_size, const VulkanMemoryType eType, const VkMemoryPropertyFlags vk_eProperties, const uint32_t u32DesiredMemoryTypes) {
		this->alloc(vk_size, eType, vk_eProperties, u32DesiredMemoryTypes);
	}

	VulkanMemory::VulkanMemory(const VkDeviceSize vk_size, const uint8_t u8MemoryTypeIndex) {
		this->alloc(vk_size, u8MemoryTypeIndex);
	}

	VulkanMemory::VulkanMemory(VulkanMemory &&rrMemory) : vk_hMemory(rrMemory.vk_hMemory), vk_size(rrMemory.vk_size), u8MemoryTypeIndex(rrMemory.u8MemoryTypeIndex), bCoherent(rrMemory.bCoherent) {
		PRINT_DEBUG_CLASS("Moving ownership of Vulkan memory to recently constructed");
		rrMemory.vk_hMemory = VK_NULL_HANDLE;
	}

	VulkanMemory::~VulkanMemory() {
		if (!valid())
			return;
		this->free();
	}

	bool VulkanMemory::alloc(const VkDeviceSize vk_size, const VulkanMemoryType eType, VkMemoryPropertyFlags vk_eProperties, const uint32_t u32DesiredMemoryTypes) {
		if ((vk_eProperties & VK_MEMORY_PROPERTY_LAZILY_ALLOCATED_BIT) && !does_memory_type_exist(VK_MEMORY_PROPERTY_LAZILY_ALLOCATED_BIT)) {
			PRINT_DEBUG_CLASS("Removing 'lazily allocated'-flag for allocating Vulkan memory");
			vk_eProperties &= ~VK_MEMORY_PROPERTY_LAZILY_ALLOCATED_BIT;
		}
		std::optional<uint8_t> memoryTypeSelected;
		const VkMemoryPropertyFlags vk_eMandatoryProperties = vk_eProperties & ~(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
		for (uint8_t u8MemoryTypeIndex = 0; u8MemoryTypeIndex < vulkanMemoryTypes.size(); u8MemoryTypeIndex++) {
			PRINT_DEBUG_CLASS("Checking if memory type at index ", u8MemoryTypeIndex, " supports required properties");
			const VkMemoryType &vk_rType = vulkanMemoryTypes[u8MemoryTypeIndex];
			if (!are_bits_true<uint32_t>(u32DesiredMemoryTypes, u8MemoryTypeIndex) || std::get<VkDeviceSize>(vulkanMemoryHeaps[vk_rType.heapIndex]) < vk_size)
				continue;
			if (vk_physicalDeviceProperties.deviceType != VK_PHYSICAL_DEVICE_TYPE_CPU) {
				switch (eType) {
					case RE_VK_GPU_RAM:
						PRINT_DEBUG_CLASS("Checking if memory type resides on GPU");
						if ((vk_rType.propertyFlags & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT) || (std::get<VkMemoryHeap>(vulkanMemoryHeaps[vk_rType.heapIndex]).flags & VK_MEMORY_HEAP_DEVICE_LOCAL_BIT) == 0)
							continue;
						break;
					case RE_VK_CPU_RAM:
						PRINT_DEBUG_CLASS("Checking if memory type resides on CPU");
						if ((vk_rType.propertyFlags & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT) == 0 || (std::get<VkMemoryHeap>(vulkanMemoryHeaps[vk_rType.heapIndex]).flags & VK_MEMORY_HEAP_DEVICE_LOCAL_BIT))
							continue;
						break;
					case RE_VK_SHARED_RAM:
						PRINT_DEBUG_CLASS("Checking if memory type is shared between CPU and GPU");
						if ((vk_rType.propertyFlags & (VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT)) == 0)
							continue;
						break;
				}
			}
			if (((vk_rType.propertyFlags ^ vk_eMandatoryProperties) & vk_eMandatoryProperties) == 0) {
				PRINT_DEBUG_CLASS("Memory type at index ", u8MemoryTypeIndex, " supports mandatory properties");
				memoryTypeSelected = u8MemoryTypeIndex;
				if (((vk_eProperties ^ vk_rType.propertyFlags) & VK_MEMORY_PROPERTY_HOST_COHERENT_BIT) == 0) {
					PRINT_DEBUG_CLASS("Memory type supports queried host coherence. Definite memory type found");
					break;
				}
			}
		}
		if (memoryTypeSelected.has_value())
			return this->alloc(vk_size, memoryTypeSelected.value());
		RE_FATAL_ERROR("Failed to find a fitting memory type for allocating Vulkan memory or it ran out of space");
		return false;
	}

	bool VulkanMemory::alloc(const VkDeviceSize vk_size, const uint8_t u8MemoryTypeIndex) {
		if (valid())
			RE_ERROR("New Vulkan memory has been allocated, even though the old one hasn't been freed yet");
		PRINT_DEBUG_CLASS("Allocating Vulkan memory at type index ", u8MemoryTypeIndex);
		this->vk_size = vk_size;
		this->u8MemoryTypeIndex = u8MemoryTypeIndex;
		bCoherent = (vulkanMemoryTypes[u8MemoryTypeIndex].propertyFlags & VK_MEMORY_PROPERTY_HOST_COHERENT_BIT) != 0;
		const VkMemoryAllocateInfo vk_allocInfo = {
			.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
			.allocationSize = vk_size,
			.memoryTypeIndex = static_cast<uint32_t>(u8MemoryTypeIndex)
		};
		const VkResult vk_allocResult = vkAllocateMemory(vk_hDevice, &vk_allocInfo, nullptr, &vk_hMemory);
		switch (vk_allocResult) {
			case VK_SUCCESS:
				std::get<VkDeviceSize>(vulkanMemoryHeaps[vulkanMemoryTypes[u8MemoryTypeIndex].heapIndex]) -= vk_size;
				return true;
			case VK_ERROR_OUT_OF_HOST_MEMORY:
			case VK_ERROR_OUT_OF_DEVICE_MEMORY:
				RE_ABORT("Ran out of Vulkan memory");
				break;
			default:
				RE_FATAL_ERROR("Failed to allocate Vulkan memory (success code ", std::hex, vk_allocResult, ")");
				break;
		}
		return false;
	}

	void VulkanMemory::free() {
		PRINT_DEBUG_CLASS("Freeing Vulkan memory");
		vkFreeMemory(vk_hDevice, vk_hMemory, nullptr);
		vk_hMemory = VK_NULL_HANDLE;
		std::get<VkDeviceSize>(vulkanMemoryHeaps[vulkanMemoryTypes[u8MemoryTypeIndex].heapIndex]) += vk_size;
	}

	bool VulkanMemory::map(const VkMemoryMapFlags vk_eFlags, const VkDeviceSize vk_offset, const VkDeviceSize vk_size, void **const ppData) const {
		PRINT_DEBUG_CLASS("Mapping Vulkan memory to CPU");
		return vkMapMemory(vk_hDevice, vk_hMemory, vk_offset, vk_size, vk_eFlags, ppData) == VK_SUCCESS;
	}
	
	void VulkanMemory::unmap() {
		PRINT_DEBUG_CLASS("Unmapping Vulkan memory");
		vkUnmapMemory(vk_hDevice, vk_hMemory);
	}

	bool VulkanMemory::valid() const noexcept {
		return vk_hMemory != VK_NULL_HANDLE;
	}

	VkDeviceMemory VulkanMemory::get() const noexcept {
		return vk_hMemory;
	}

	VkDeviceSize VulkanMemory::size() const noexcept {
		return vk_size;
	}

	uint8_t VulkanMemory::type_index() const noexcept {
		return u8MemoryTypeIndex;
	}

	bool VulkanMemory::cpu_coherent() const noexcept {
		return bCoherent;
	}

	void VulkanMemory::operator =(VulkanMemory &&rrMemory) noexcept {
		if (valid())
			RE_ERROR("Memory has been moved into this instance, even though the old one hasn't been freed yet");
		PRINT_DEBUG_CLASS("Moving ownership of Vulkan memory to this instance");
		vk_hMemory = rrMemory.vk_hMemory;
		rrMemory.vk_hMemory = VK_NULL_HANDLE;
		vk_size = rrMemory.vk_size;
		u8MemoryTypeIndex = rrMemory.u8MemoryTypeIndex;
		bCoherent = rrMemory.bCoherent;
	}

}
