#include "RE_Vulkan_Wrappers.hpp"
#include "RE_Console.hpp"

namespace RE {

	bool create_vulkan_buffer(const VkBufferCreateFlags vk_mFlags, 
			const VkDeviceSize vk_size, 
			const VkBufferUsageFlags vk_mUsages, 
			const uint32_t u32QueueFamilyCount, 
			const uint32_t *const pau32QueueFamilies, 
			VkBuffer *const vk_phBuffer) {
		PRINT_DEBUG("Creating a Vulkan buffer");
		VkBufferCreateInfo vk_createInfo;
		vk_createInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		vk_createInfo.pNext = nullptr;
		vk_createInfo.flags = vk_mFlags;
		vk_createInfo.size = vk_size;
		vk_createInfo.usage = vk_mUsages;
		if (u32QueueFamilyCount == 1)
			vk_createInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		else {
			vk_createInfo.sharingMode = VK_SHARING_MODE_CONCURRENT;
			vk_createInfo.queueFamilyIndexCount = u32QueueFamilyCount;
			vk_createInfo.pQueueFamilyIndices = pau32QueueFamilies;
		}
		if (vkCreateBuffer(vk_hDevice, &vk_createInfo, nullptr, vk_phBuffer) == VK_SUCCESS)
			return true;
		else
			RE_ERROR("Failed to create Vulkan buffer");
		return false;
	}

	Vulkan_Buffer::Vulkan_Buffer() : vk_hBuffer(VK_NULL_HANDLE) {}
	
	Vulkan_Buffer::Vulkan_Buffer(const VkBufferCreateFlags vk_mFlags, 
			const VkDeviceSize vk_size, 
			const VkBufferUsageFlags vk_mUsages, 
			const uint32_t u32QueueFamilyCount, 
			const uint32_t *const pau32QueueFamilies, 
			const VkMemoryPropertyFlags vk_mMemoryPropertyFlags) : Vulkan_Buffer() {
		PRINT_DEBUG_CLASS("Constructing Vulkan buffer wrapper");
		create(vk_mFlags, 
				vk_size, 
				vk_mUsages, 
				u32QueueFamilyCount, 
				pau32QueueFamilies, 
				vk_mMemoryPropertyFlags);
	}

	Vulkan_Buffer::Vulkan_Buffer(Vulkan_Buffer &&rrCopy) : vk_hBuffer(rrCopy.vk_hBuffer), memory(std::move(rrCopy.memory)) {
		PRINT_DEBUG("Constructing Vulkan buffer wrapper by moving ownership from another wrapper");
		rrCopy.vk_hBuffer = VK_NULL_HANDLE;
	}
	
	Vulkan_Buffer::~Vulkan_Buffer() {
		PRINT_DEBUG_CLASS("Destructing Vulkan buffer wrapper");
		destroy();
	}

	bool Vulkan_Buffer::create(const VkBufferCreateFlags vk_mFlags, 
			const VkDeviceSize vk_size, 
			const VkBufferUsageFlags vk_mUsages, 
			const uint32_t u32QueueFamilyCount, 
			const uint32_t *const pau32QueueFamilies, 
			const VkMemoryPropertyFlags vk_mMemoryPropertyFlags) {
#ifndef RE_DISABLE_PRINT_DEBUGS
		if (valid())
			RE_ERROR("Creating another Vulkan buffer wrapper, when the old buffer ", vk_hBuffer, " hasn't been destroyed yet");
		if ((vk_mFlags & VK_BUFFER_CREATE_SPARSE_BINDING_BIT)) {
			RE_ERROR("A sparse-bound Vulkan buffer had to be created in a wrapper");
			return false;
		}
#endif
		PRINT_DEBUG_CLASS("Creating Vulkan buffer in wrapper class");
		if (create_vulkan_buffer(vk_mFlags, 
				vk_size, 
				vk_mUsages, 
				u32QueueFamilyCount, 
				pau32QueueFamilies, 
				&vk_hBuffer)) {
			PRINT_DEBUG_CLASS("Allocating memory for Vulkan buffer ", std::hex, vk_hBuffer, " in wrapper class");
			if (memory.alloc_for_buffer(vk_hBuffer, vk_mMemoryPropertyFlags) == VK_SUCCESS)
				return true;
			else
				RE_ERROR("Failed to allocate memory for Vulkan buffer ", std::hex, vk_hBuffer, " in wrapper class");
		} else
			RE_ERROR("Failed to create Vulkan buffer in wrapper class");
		return false;
	}

	void Vulkan_Buffer::destroy() {
		PRINT_DEBUG_CLASS("Destroying Vulkan buffer wrapper ", vk_hBuffer, " and its memory");
		vkDestroyBuffer(vk_hDevice, vk_hBuffer, nullptr);
		vk_hBuffer = VK_NULL_HANDLE;
		memory.free();
	}

	VkBuffer Vulkan_Buffer::get() const {
		return vk_hBuffer;
	}

	const VkBuffer* Vulkan_Buffer::get_ptr() const {
		return &vk_hBuffer;
	}
	
	VulkanMemory& Vulkan_Buffer::get_memory() {
		return memory;
	}

	bool Vulkan_Buffer::valid() const {
		return vk_hBuffer && memory.valid();
	}

	Vulkan_Buffer::operator VkBuffer() const {
		return get();
	}

	Vulkan_Buffer::operator const VkBuffer*() const {
		return get_ptr();
	}

	Vulkan_Buffer::operator VulkanMemory&() {
		return get_memory();
	}

	Vulkan_Buffer::operator bool() const {
		return valid();
	}

	VkBuffer Vulkan_Buffer::operator()() const {
		return get();
	}

}
