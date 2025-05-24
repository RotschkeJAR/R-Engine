#include "RE_Vulkan.hpp"
#include "RE_Render System.hpp"

namespace RE {
	
	Vulkan_Buffer::Vulkan_Buffer(const VkDeviceSize vk_sizeInBytes, const VkBufferUsageFlags vk_eUsage, const uint32_t u32QueueTypeCount, const uint32_t *pu32BufferQueueTypeIndices, const VkMemoryPropertyFlags vk_eMemoryProperties) : vk_hBuffer(VK_NULL_HANDLE), vk_hMemory(VK_NULL_HANDLE), vk_sizeInBytes(vk_sizeInBytes) {
		if (!vk_create_buffer(vk_sizeInBytes, vk_eUsage, u32QueueTypeCount, pu32BufferQueueTypeIndices, vk_eMemoryProperties, &vk_hBuffer, &vk_hMemory)) {
			vk_hBuffer = VK_NULL_HANDLE;
			vk_hMemory = VK_NULL_HANDLE;
			return;
		}
		CATCH_SIGNAL(vkBindBufferMemory(vk_hDevice, vk_hBuffer, vk_hMemory, vk_sizeInBytes));
	}
	
	Vulkan_Buffer::~Vulkan_Buffer() {
		if (!is_valid())
			return;
		CATCH_SIGNAL(vkBindBufferMemory(vk_hDevice, vk_hBuffer, VK_NULL_HANDLE, 0UL));
		CATCH_SIGNAL(vkFreeMemory(vk_hDevice, vk_hMemory, nullptr));
		CATCH_SIGNAL(vkDestroyBuffer(vk_hDevice, vk_hBuffer, nullptr));
	}
	
	bool Vulkan_Buffer::map_memory(void **ppData) const {
		return CHECK_VK_RESULT(vkMapMemory(vk_hDevice, vk_hMemory, 0UL, vk_sizeInBytes, 0, ppData));
	}
	
	void Vulkan_Buffer::unmap_memory() const {
		CATCH_SIGNAL(vkUnmapMemory(vk_hDevice, vk_hMemory));
	}
	
	bool Vulkan_Buffer::is_valid() const {
		return vk_hBuffer && vk_hMemory;
	}
	
	VkBuffer Vulkan_Buffer::get_buffer_handle() const {
		return vk_hBuffer;
	}
	
	VkDeviceMemory Vulkan_Buffer::get_memory_handle() const {
		return vk_hMemory;
	}
	
	VkBuffer* Vulkan_Buffer::get_buffer_handle_ptr() const {
		return &vk_hBuffer;
	}
	
	VkDeviceMemory* Vulkan_Buffer::get_memory_handle_ptr() const {
		return &vk_hMemory;
	}

	Vulkan_Buffer::operator VkBuffer() const {
		return get_buffer_handle();
	}
	
	Vulkan_Buffer::operator VkDeviceMemory() const {
		return get_memory_handle();
	}
	
	Vulkan_Buffer::operator VkBuffer*() const {
		return get_buffer_handle_ptr();
	}
	
	Vulkan_Buffer::operator VkDeviceMemory*() const {
		return get_memory_handle_ptr();
	}

}
