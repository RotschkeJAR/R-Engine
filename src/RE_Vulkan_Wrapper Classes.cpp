#include "RE_Vulkan_Wrapper Classes.hpp"
#include "RE_Render System.hpp"
#include "RE_Vulkan_Wrapper Functions.hpp"

namespace RE {
	
	Vulkan_Buffer::Vulkan_Buffer(const VkDeviceSize vk_size, const VkBufferUsageFlags vk_eUsages, const uint32_t u32QueueCount, const uint32_t *pu32Queues, const VkMemoryPropertyFlags vk_eMemoryPropertyFlags) : vk_hBuffer(VK_NULL_HANDLE), vk_hMemory(VK_NULL_HANDLE) {
		CATCH_SIGNAL(create_vulkan_buffer(vk_size, vk_eUsages, u32QueueCount, pu32Queues, vk_eMemoryPropertyFlags, &vk_hBuffer, &vk_hMemory));
	}
	
	Vulkan_Buffer::~Vulkan_Buffer() {
		if (!is_valid())
			return;
		vkFreeMemory(vk_hDevice, vk_hMemory, nullptr);
		vkDestroyBuffer(vk_hDevice, vk_hBuffer, nullptr);
	}

	bool Vulkan_Buffer::is_valid() const {
		return vk_hBuffer && vk_hMemory;
	}

}
