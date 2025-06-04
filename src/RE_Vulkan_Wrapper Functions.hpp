#ifndef __RE_VULKAN_WRAPPER_FUNCS_H__
#define __RE_VULKAN_WRAPPER_FUNCS_H__

#include "RE_Vulkan_Instance.hpp"

namespace RE {
	
	bool create_vulkan_buffer(const VkDeviceSize vk_size, const VkBufferUsageFlags vk_eUsages, const uint32_t u32QueueCount, const uint32_t *pu32Queues, const VkMemoryPropertyFlags vk_eMemoryPropertyFlags, VkBuffer *vk_phBuffer, VkDeviceMemory *vk_phMemory);

}

#endif /* __RE_VULKAN_WRAPPER_FUNCS_H__ */