#include "RE_GPU_Internal.hpp"

namespace RE {

	static VkResult alloc_vulkan_memory(const void *const pNext, const VkDeviceSize vk_size, const uint32_t u32MemoryTypeIndex, VkDeviceMemory *const vk_phMemory) {
		const VkMemoryAllocateInfo vk_allocInfo = {
			.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
			.pNext = pNext,
			.allocationSize = vk_size,
			.memoryTypeIndex = u32MemoryTypeIndex
		};
		return vkAllocateMemory(vk_hDevice, &vk_allocInfo, nullptr, vk_phMemory);
	}

	static bool find_matching_memory_type(const VkDeviceSize vk_leastSize, const VkMemoryPropertyFlags vk_mProperties, const uint32_t m32DesiredMemoryTypes, uint8_t &ru8MemoryType) {
		PRINT_DEBUG("Searching for Vulkan memory types supporting mandatory properties ", std::hex, vk_mProperties, " and are present within any type of ", m32DesiredMemoryTypes);
		std::optional<uint8_t> memoryTypeSelected;
		int32_t i32MostMinimalMismatchScore = std::numeric_limits<int32_t>::max();
		for (uint8_t u8MemoryTypeIndex = 0; u8MemoryTypeIndex < u8MemoryTypeCount; u8MemoryTypeIndex++) {
			PRINT_DEBUG("Checking if memory type at index ", u8MemoryTypeIndex, " supports required properties");
			const VkMemoryType &vk_rType = vulkanMemoryTypes[u8MemoryTypeIndex];
			if (!are_bits_true<uint32_t>(m32DesiredMemoryTypes, u8MemoryTypeIndex) || std::get<VkDeviceSize>(vulkanMemoryHeaps[vk_rType.heapIndex]) < vk_leastSize)
				continue;
			if ((vk_rType.propertyFlags & vk_mProperties) == vk_mProperties) {
				PRINT_DEBUG("Memory type at index ", u8MemoryTypeIndex, " supports mandatory properties");
				const VkMemoryPropertyFlags vk_mMismatchingProperties = vk_rType.propertyFlags & (~vk_mProperties);
				int32_t i32MismatchScore = 0;
				if ((vk_mMismatchingProperties & VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT) != 0)
					i32MismatchScore += 500;
				if ((vk_mMismatchingProperties & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT) != 0)
					i32MismatchScore += 500;
				if ((vk_mMismatchingProperties & VK_MEMORY_PROPERTY_HOST_COHERENT_BIT) != 0)
					i32MismatchScore += 1;
				if ((vk_mMismatchingProperties & VK_MEMORY_PROPERTY_HOST_CACHED_BIT) != 0)
					i32MismatchScore += 100;
				if ((vk_mMismatchingProperties & VK_MEMORY_PROPERTY_LAZILY_ALLOCATED_BIT) != 0)
					i32MismatchScore += 1;
				if ((vk_mMismatchingProperties & VK_MEMORY_PROPERTY_PROTECTED_BIT) != 0)
					i32MismatchScore += 1000;
				if ((vk_mMismatchingProperties & VK_MEMORY_PROPERTY_DEVICE_COHERENT_BIT_AMD) != 0)
					i32MismatchScore += 100;
				if ((vk_mMismatchingProperties & VK_MEMORY_PROPERTY_DEVICE_UNCACHED_BIT_AMD) != 0)
					i32MismatchScore += 100;
				if ((vk_mMismatchingProperties & VK_MEMORY_PROPERTY_RDMA_CAPABLE_BIT_NV) != 0)
					i32MismatchScore += 100;
				if (i32MismatchScore < i32MostMinimalMismatchScore) {
					PRINT_DEBUG("New minimum of mismatching properties found at memory type index ", u8MemoryTypeIndex, " with mismatch score ", i32MismatchScore);
					memoryTypeSelected = u8MemoryTypeIndex;
					i32MostMinimalMismatchScore = i32MismatchScore;
				}
			}
		}
		if (memoryTypeSelected.has_value()) {
			PRINT_DEBUG("Memory type supporting mandatory properties found. Returning index");
			ru8MemoryType = memoryTypeSelected.value();
			return true;
		}
		RE_ERROR("Failed to find a fitting memory type for allocating Vulkan memory or it ran out of space");
		return false;
	}
	
	VulkanMemory::VulkanMemory() : vk_hMemory(VK_NULL_HANDLE), vk_size(0) {}

	VulkanMemory::VulkanMemory(const VkDeviceSize vk_size, VkMemoryPropertyFlags vk_mProperties, const uint32_t m32DesiredMemoryTypes) : vk_hMemory(VK_NULL_HANDLE), vk_size(0) {
		this->alloc(vk_size, vk_mProperties, m32DesiredMemoryTypes);
	}

	VulkanMemory::VulkanMemory(const VkDeviceSize vk_size, const uint8_t u8MemoryType) : vk_hMemory(VK_NULL_HANDLE), vk_size(0) {
		this->alloc(vk_size, u8MemoryType);
	}

	VulkanMemory::VulkanMemory(VulkanMemory &&rrMemory) : vk_hMemory(rrMemory.vk_hMemory), vk_size(rrMemory.vk_size), u8MemoryType(rrMemory.u8MemoryType), bCoherent(rrMemory.bCoherent) {
		PRINT_DEBUG_CLASS("Moving ownership of Vulkan memory to recently constructed");
		rrMemory.vk_hMemory = VK_NULL_HANDLE;
		rrMemory.vk_size = 0;
	}

	VulkanMemory::~VulkanMemory() {
		this->free();
	}

	VkResult VulkanMemory::alloc(const VkDeviceSize vk_size, const VkMemoryPropertyFlags vk_mProperties, const uint32_t m32DesiredMemoryTypes) {
		PRINT_DEBUG_CLASS("Allocating Vulkan memory supporting properties ", std::hex, vk_mProperties, " and from types ", m32DesiredMemoryTypes);
		if (find_matching_memory_type(vk_size, vk_mProperties, m32DesiredMemoryTypes, u8MemoryType))
			return this->alloc(vk_size, u8MemoryType);
		return vk_mProperties & VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT ? VK_ERROR_OUT_OF_DEVICE_MEMORY : VK_ERROR_OUT_OF_HOST_MEMORY;
	}

	VkResult VulkanMemory::alloc(const VkDeviceSize vk_size, const uint8_t u8MemoryType) {
		if (valid())
			RE_ERROR("New memory is being allocated, even though the old Vulkan memory ", vk_hMemory, " occupying ", vk_size, " bytes hasn't been freed yet");
		PRINT_DEBUG_CLASS("Allocating Vulkan memory at type index ", u8MemoryType);
		this->u8MemoryType = u8MemoryType;
		bCoherent = (vulkanMemoryTypes[u8MemoryType].propertyFlags & VK_MEMORY_PROPERTY_HOST_COHERENT_BIT) != 0;
		const VkResult vk_eResult = alloc_vulkan_memory(nullptr, vk_size, static_cast<uint32_t>(u8MemoryType), &vk_hMemory);
		if (vk_eResult == VK_SUCCESS)
			this->vk_size = vk_size;
		return vk_eResult;
	}

	VkResult VulkanMemory::alloc_for_buffer(const VkBuffer vk_hBuffer, const VkMemoryPropertyFlags vk_mProperties) {
		if (valid())
			RE_ERROR("New memory is being allocated, even though the old Vulkan memory ", vk_hMemory, " occupying ", vk_size, " bytes hasn't been freed yet");
		PRINT_DEBUG_CLASS("Fetching requirements for memory for Vulkan buffer ", std::hex, vk_hBuffer);
		VkResult vk_eLatestResult = vk_mProperties & VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT ? VK_ERROR_OUT_OF_DEVICE_MEMORY : VK_ERROR_OUT_OF_HOST_MEMORY;
		const VkBufferMemoryRequirementsInfo2 vk_bufferInfo = {
			.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_REQUIREMENTS_INFO_2,
			.pNext = nullptr,
			.buffer = vk_hBuffer
		};
		VkMemoryRequirements2 vk_memoryRequirements;
		vk_memoryRequirements.sType = VK_STRUCTURE_TYPE_MEMORY_REQUIREMENTS_2;
		vk_memoryRequirements.pNext = nullptr;
		vkGetBufferMemoryRequirements2(vk_hDevice, &vk_bufferInfo, &vk_memoryRequirements);
		PRINT_DEBUG_CLASS("Finding matching Vulkan memory type supporting properties ", std::hex, vk_mProperties);
		vk_size = vk_memoryRequirements.memoryRequirements.size;
		if (find_matching_memory_type(vk_size, vk_mProperties, vk_memoryRequirements.memoryRequirements.memoryTypeBits, u8MemoryType)) {
			PRINT_DEBUG_CLASS("Allocating Vulkan memory of type ", u8MemoryType);
			const VkMemoryDedicatedAllocateInfo vk_dedicatedMemoryInfo = {
				.sType = VK_STRUCTURE_TYPE_MEMORY_DEDICATED_ALLOCATE_INFO,
				.pNext = nullptr,
				.image = VK_NULL_HANDLE,
				.buffer = vk_hBuffer
			};
			vk_eLatestResult = alloc_vulkan_memory(&vk_dedicatedMemoryInfo, vk_size, u8MemoryType, &vk_hMemory);
			if (vk_eLatestResult == VK_SUCCESS) {
				PRINT_DEBUG_CLASS("Binding Vulkan memory ", vk_hMemory, " to buffer");
				const VkBindBufferMemoryInfo vk_bindBufferInfo = {
					.sType = VK_STRUCTURE_TYPE_BIND_BUFFER_MEMORY_INFO,
					.pNext = nullptr,
					.buffer = vk_hBuffer,
					.memory = vk_hMemory,
					.memoryOffset = 0
				};
				vk_eLatestResult = vkBindBufferMemory2(vk_hDevice, 1, &vk_bindBufferInfo);
				if (vk_eLatestResult == VK_SUCCESS)
					return VK_SUCCESS;
				else
					RE_ERROR("Failed to bind Vulkan memory ", vk_hMemory, " to buffer ", vk_hBuffer);
			} else
				RE_ERROR("Failed to allocate Vulkan memory for buffer ", vk_hBuffer);
		} else
			RE_ERROR("Failed to find matching Vulkan memory type for buffer ", vk_hBuffer);
		return vk_eLatestResult;
	}

	VkResult VulkanMemory::alloc_for_image(const VkImage vk_hImage, const VkMemoryPropertyFlags vk_mProperties) {
		if (valid())
			RE_ERROR("New memory is being allocated, even though the old Vulkan memory ", vk_hMemory, " occupying ", vk_size, " bytes hasn't been freed yet");
		PRINT_DEBUG_CLASS("Fetching requirements for memory for Vulkan image ", std::hex, vk_hImage);
		VkResult vk_eLatestResult = vk_mProperties & VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT ? VK_ERROR_OUT_OF_DEVICE_MEMORY : VK_ERROR_OUT_OF_HOST_MEMORY;
		const VkImageMemoryRequirementsInfo2 vk_imageInfo = {
			.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_REQUIREMENTS_INFO_2,
			.pNext = nullptr,
			.image = vk_hImage
		};
		VkMemoryRequirements2 vk_memoryRequirements;
		vk_memoryRequirements.sType = VK_STRUCTURE_TYPE_MEMORY_REQUIREMENTS_2;
		vk_memoryRequirements.pNext = nullptr;
		vkGetImageMemoryRequirements2(vk_hDevice, &vk_imageInfo, &vk_memoryRequirements);
		PRINT_DEBUG_CLASS("Finding matching Vulkan memory type supporting properties ", std::hex, vk_mProperties);
		vk_size = vk_memoryRequirements.memoryRequirements.size;
		if (find_matching_memory_type(vk_size, vk_mProperties, vk_memoryRequirements.memoryRequirements.memoryTypeBits, u8MemoryType)) {
			PRINT_DEBUG_CLASS("Allocating Vulkan memory of type ", u8MemoryType);
			const VkMemoryDedicatedAllocateInfo vk_dedicatedMemoryInfo = {
				.sType = VK_STRUCTURE_TYPE_MEMORY_DEDICATED_ALLOCATE_INFO,
				.pNext = nullptr,
				.image = vk_hImage,
				.buffer = VK_NULL_HANDLE
			};
			vk_eLatestResult = alloc_vulkan_memory(&vk_dedicatedMemoryInfo, vk_size, u8MemoryType, &vk_hMemory);
			if (vk_eLatestResult == VK_SUCCESS) {
				PRINT_DEBUG_CLASS("Binding Vulkan memory ", vk_hMemory, " to the image");
				const VkBindImageMemoryInfo vk_bindImageInfo = {
					.sType = VK_STRUCTURE_TYPE_BIND_IMAGE_MEMORY_INFO,
					.pNext = nullptr,
					.image = vk_hImage,
					.memory = vk_hMemory,
					.memoryOffset = 0
				};
				vk_eLatestResult = vkBindImageMemory2(vk_hDevice, 1, &vk_bindImageInfo);
				if (vk_eLatestResult == VK_SUCCESS)
					return VK_SUCCESS;
				else
					RE_ERROR("Failed to bind Vulkan memory ", vk_hMemory, " to image ", vk_hImage);
			} else
				RE_ERROR("Failed to allocate Vulkan memory for image ", vk_hImage);
		} else
			RE_ERROR("Failed to find matching Vulkan memory type for image ", vk_hImage);
		return vk_eLatestResult;
	}

	void VulkanMemory::free() {
		PRINT_DEBUG_CLASS("Freeing Vulkan memory");
		vkFreeMemory(vk_hDevice, vk_hMemory, nullptr);
		std::get<VkDeviceSize>(vulkanMemoryHeaps[vulkanMemoryTypes[u8MemoryType].heapIndex]) += vk_size;
		vk_hMemory = VK_NULL_HANDLE;
		vk_size = 0;
	}

	bool VulkanMemory::map(const VkMemoryMapFlags vk_eFlags, const VkDeviceSize vk_offset, const VkDeviceSize vk_size, void **const ppData) const {
		PRINT_DEBUG_CLASS("Mapping Vulkan memory to CPU");
		return vkMapMemory(vk_hDevice, vk_hMemory, vk_offset, vk_size, vk_eFlags, ppData) == VK_SUCCESS;
	}
	
	void VulkanMemory::unmap() {
		PRINT_DEBUG_CLASS("Unmapping Vulkan memory");
		vkUnmapMemory(vk_hDevice, vk_hMemory);
	}

	bool VulkanMemory::valid() const {
		return vk_hMemory != VK_NULL_HANDLE && vk_size > 0;
	}

	VkDeviceMemory VulkanMemory::get() const {
		return vk_hMemory;
	}

	VkDeviceSize VulkanMemory::size() const {
		return vk_size;
	}

	uint8_t VulkanMemory::type_index() const {
		return u8MemoryType;
	}

	bool VulkanMemory::cpu_coherent() const {
		return bCoherent;
	}

	VulkanMemory& VulkanMemory::operator =(VulkanMemory &&rrMemory) {
		if (valid())
			RE_ERROR("Memory has been moved into instance ", this, ", even though the Vulkan memory ", vk_hMemory, " occupying ", vk_size, " bytes hasn't been freed yet");
		PRINT_DEBUG_CLASS("Moving ownership of Vulkan memory from ", std::addressof(rrMemory), " to this instance ", this);
		vk_hMemory = rrMemory.vk_hMemory;
		rrMemory.vk_hMemory = VK_NULL_HANDLE;
		vk_size = rrMemory.vk_size;
		rrMemory.vk_size = 0;
		u8MemoryType = rrMemory.u8MemoryType;
		bCoherent = rrMemory.bCoherent;
		return *this;
	}

	VulkanMemory::operator bool() const {
		return valid();
	}

	VkDeviceMemory VulkanMemory::operator()() const {
		return get();
	}

}
