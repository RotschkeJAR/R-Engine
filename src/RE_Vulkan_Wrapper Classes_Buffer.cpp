#include "RE_Vulkan_Wrapper Classes.hpp"
#include "RE_GPU.hpp"
#include "RE_Vulkan_Wrapper Functions.hpp"

namespace RE {

	Vulkan_Buffer::Vulkan_Buffer() : vk_hBuffer(VK_NULL_HANDLE), vk_hMemory(VK_NULL_HANDLE) {}
	
	Vulkan_Buffer::Vulkan_Buffer(const VkBufferCreateFlags vk_eCreateFlags, const VkDeviceSize vk_size, const VkBufferUsageFlags vk_eUsages, const uint32_t u32QueueFamilyCount, const uint32_t *const pu32QueueFamilies, const VkMemoryPropertyFlags vk_eMemoryPropertyFlags) : vk_hBuffer(VK_NULL_HANDLE), vk_hMemory(VK_NULL_HANDLE) {
		PRINT_DEBUG_CLASS("Constructing Vulkan buffer wrapper");
		init(vk_eCreateFlags, vk_size, vk_eUsages, u32QueueFamilyCount, pu32QueueFamilies, vk_eMemoryPropertyFlags);
	}
	
	Vulkan_Buffer::~Vulkan_Buffer() {
		PRINT_DEBUG_CLASS("Destructing Vulkan buffer wrapper");
		if (is_valid())
			destroy();
	}

	bool Vulkan_Buffer::init(const VkBufferCreateFlags vk_eCreateFlags, const VkDeviceSize vk_size, const VkBufferUsageFlags vk_eUsages, const uint32_t u32QueueFamilyCount, const uint32_t *const pu32QueueFamilies, const VkMemoryPropertyFlags vk_eMemoryPropertyFlags) {
		PRINT_DEBUG_CLASS("Initializing Vulkan buffer wrapper");
		return create_vulkan_buffer(vk_eCreateFlags, vk_size, vk_eUsages, u32QueueFamilyCount, pu32QueueFamilies, vk_eMemoryPropertyFlags, &vk_hBuffer, &vk_hMemory);
	}

	void Vulkan_Buffer::destroy() {
		PRINT_DEBUG_CLASS("Destroying Vulkan buffer wrapper ", vk_hBuffer, " and its memory ", vk_hMemory);
		vkFreeMemory(vk_hDevice, vk_hMemory, nullptr);
		vkDestroyBuffer(vk_hDevice, vk_hBuffer, nullptr);
		vk_hMemory = VK_NULL_HANDLE;
		vk_hBuffer = VK_NULL_HANDLE;
	}

	bool Vulkan_Buffer::map_memory(const VkDeviceSize vk_offset, const VkDeviceSize vk_size, void **ppData) const {
		PRINT_DEBUG_CLASS("Mapping Vulkan buffer wrapper's memory ", vk_hMemory, " beginning at byte ", vk_offset, " and sized ", vk_size);
		return vkMapMemory(vk_hDevice, vk_hMemory, vk_offset, vk_size, 0, ppData) == VK_SUCCESS;
	}

	void Vulkan_Buffer::unmap_memory() const {
		PRINT_DEBUG_CLASS("Unmapping Vulkan buffer wrapper's memory ", vk_hMemory);
		vkUnmapMemory(vk_hDevice, vk_hMemory);
	}

	[[nodiscard]]
	VkBuffer Vulkan_Buffer::get_buffer() const {
		return vk_hBuffer;
	}

	[[nodiscard]]
	const VkBuffer* Vulkan_Buffer::get_buffer_ptr() const {
		return &vk_hBuffer;
	}
	
	[[nodiscard]]
	VkDeviceMemory Vulkan_Buffer::get_memory() const {
		return vk_hMemory;
	}

	[[nodiscard]]
	bool Vulkan_Buffer::is_valid() const {
		return vk_hBuffer && vk_hMemory;
	}

	[[nodiscard]]
	Vulkan_Buffer::operator VkBuffer() const {
		return get_buffer();
	}

	[[nodiscard]]
	Vulkan_Buffer::operator const VkBuffer*() const {
		return get_buffer_ptr();
	}

	[[nodiscard]]
	Vulkan_Buffer::operator VkDeviceMemory() const {
		return get_memory();
	}

}
