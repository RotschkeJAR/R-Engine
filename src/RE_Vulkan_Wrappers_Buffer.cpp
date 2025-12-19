#include "RE_Vulkan_Wrappers.hpp"
#include "RE_Console.hpp"

namespace RE {

	bool create_vulkan_buffer(const VkBufferCreateFlags vk_eFlags, 
			const VkDeviceSize vk_size, 
			const VkBufferUsageFlags vk_eUsages, 
			const uint32_t u32QueueFamilyCount, 
			const uint32_t *const pau32QueueFamilies, 
			const VulkanMemoryType eMemoryType, 
			const VkMemoryPropertyFlags vk_eMemoryPropertyFlags, 
			VkBuffer *const vk_phBuffer, 
			VulkanMemory *const pMemory) {
		PRINT_DEBUG("Creating a Vulkan buffer");
		VkBufferCreateInfo vk_createInfo;
		vk_createInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		vk_createInfo.pNext = nullptr;
		vk_createInfo.flags = vk_eFlags;
		vk_createInfo.size = vk_size;
		vk_createInfo.usage = vk_eUsages;
		if (u32QueueFamilyCount == 1)
			vk_createInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		else {
			vk_createInfo.sharingMode = VK_SHARING_MODE_CONCURRENT;
			vk_createInfo.queueFamilyIndexCount = u32QueueFamilyCount;
			vk_createInfo.pQueueFamilyIndices = pau32QueueFamilies;
		}
		if (vkCreateBuffer(vk_hDevice, &vk_createInfo, nullptr, vk_phBuffer) == VK_SUCCESS) {
			PRINT_DEBUG("Fetching memory requirements for Vulkan buffer and allocating memory for it");
			VkMemoryRequirements vk_memoryRequirements;
			vkGetBufferMemoryRequirements(vk_hDevice, *vk_phBuffer, &vk_memoryRequirements);
			if (pMemory->alloc(vk_memoryRequirements.size, eMemoryType, vk_eMemoryPropertyFlags, vk_memoryRequirements.memoryTypeBits)) {
				PRINT_DEBUG("Binding memory to Vulkan buffer");
				if (vkBindBufferMemory(vk_hDevice, *vk_phBuffer, pMemory->get(), 0) == VK_SUCCESS)
					return true;
				else
					RE_FATAL_ERROR("Failed to bind memory to a Vulkan buffer");
				PRINT_DEBUG("Freeing memory after failing to bind memory to Vulkan buffer");
				pMemory->free();
			}
			PRINT_DEBUG("Destroying Vulkan buffer after failing allocating memory for it");
			vkDestroyBuffer(vk_hDevice, *vk_phBuffer, nullptr);
			*vk_phBuffer = VK_NULL_HANDLE;
		} else
			RE_FATAL_ERROR("Failed creating a Vulkan buffer");
		return false;
	}

	Vulkan_Buffer::Vulkan_Buffer() : vk_hBuffer(VK_NULL_HANDLE) {}
	
	Vulkan_Buffer::Vulkan_Buffer(const VkBufferCreateFlags vk_eFlags, 
			const VkDeviceSize vk_size, 
			const VkBufferUsageFlags vk_eUsages, 
			const uint32_t u32QueueFamilyCount, 
			const uint32_t *const pau32QueueFamilies, 
			const VulkanMemoryType eMemoryType, 
			const VkMemoryPropertyFlags vk_eMemoryPropertyFlags) : Vulkan_Buffer() {
		PRINT_DEBUG_CLASS("Constructing Vulkan buffer wrapper");
		create(vk_eFlags, 
				vk_size, 
				vk_eUsages, 
				u32QueueFamilyCount, 
				pau32QueueFamilies, 
				eMemoryType, 
				vk_eMemoryPropertyFlags);
	}

	Vulkan_Buffer::Vulkan_Buffer(Vulkan_Buffer &&rrCopy) : vk_hBuffer(rrCopy.vk_hBuffer), memory(std::move(rrCopy.memory)) {
		PRINT_DEBUG("Constructing Vulkan buffer wrapper by moving ownership from another wrapper");
		rrCopy.vk_hBuffer = VK_NULL_HANDLE;
	}
	
	Vulkan_Buffer::~Vulkan_Buffer() {
		PRINT_DEBUG_CLASS("Destructing Vulkan buffer wrapper");
		if (valid())
			destroy();
	}

	bool Vulkan_Buffer::create(const VkBufferCreateFlags vk_eFlags, 
			const VkDeviceSize vk_size, 
			const VkBufferUsageFlags vk_eUsages, 
			const uint32_t u32QueueFamilyCount, 
			const uint32_t *const pau32QueueFamilies, 
			const VulkanMemoryType eMemoryType, 
			const VkMemoryPropertyFlags vk_eMemoryPropertyFlags) {
#ifndef RE_DISABLE_PRINT_DEBUGS
		if (valid())
			RE_ERROR("Creating another Vulkan buffer wrapper, when the old one hasn't been destroyed yet");
#endif
		PRINT_DEBUG_CLASS("Creating Vulkan buffer wrapper");
		return create_vulkan_buffer(vk_eFlags, 
				vk_size, 
				vk_eUsages, 
				u32QueueFamilyCount, 
				pau32QueueFamilies, 
				eMemoryType, 
				vk_eMemoryPropertyFlags, 
				&vk_hBuffer, 
				&memory);
	}

	void Vulkan_Buffer::destroy() {
		PRINT_DEBUG_CLASS("Destroying Vulkan buffer wrapper ", vk_hBuffer, " and its memory");
		vkDestroyBuffer(vk_hDevice, vk_hBuffer, nullptr);
		vk_hBuffer = VK_NULL_HANDLE;
		memory.free();
	}

	[[nodiscard]]
	VkBuffer Vulkan_Buffer::get() const noexcept {
		return vk_hBuffer;
	}

	[[nodiscard]]
	const VkBuffer* Vulkan_Buffer::get_ptr() const noexcept {
		return &vk_hBuffer;
	}
	
	[[nodiscard]]
	VulkanMemory& Vulkan_Buffer::get_memory() noexcept {
		return memory;
	}

	[[nodiscard]]
	bool Vulkan_Buffer::valid() const noexcept {
		return vk_hBuffer && memory.valid();
	}

	[[nodiscard]]
	Vulkan_Buffer::operator VkBuffer() const {
		return get();
	}

	[[nodiscard]]
	Vulkan_Buffer::operator const VkBuffer*() const {
		return get_ptr();
	}

	[[nodiscard]]
	Vulkan_Buffer::operator VulkanMemory&() {
		return get_memory();
	}

	[[nodiscard]]
	Vulkan_Buffer::operator bool() const {
		return valid();
	}

}
