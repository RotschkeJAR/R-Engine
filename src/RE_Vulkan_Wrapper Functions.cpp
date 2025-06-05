#include "RE_Vulkan_Wrapper Functions.hpp"
#include "RE_Render System.hpp"

namespace RE {

	static std::vector<uint32_t> get_queue_family_indices(const uint32_t u32QueueCount, const uint32_t *pu32Queues) {
		std::vector<uint32_t> queueFamilies;
		for (uint32_t u32QueueIndex = 0U; u32QueueIndex < u32QueueCount; u32QueueIndex++)
			if (std::find(queueFamilies.begin(), queueFamilies.end(), u32DeviceQueueFamilyIndices[pu32Queues[u32QueueIndex]]) == queueFamilies.end())
				queueFamilies.push_back(u32DeviceQueueFamilyIndices[pu32Queues[u32QueueIndex]]);
		return queueFamilies;
	}

	static uint32_t find_vulkan_device_memory_type(const uint32_t u32MemoryTypeBits, const VkMemoryPropertyFlags vk_eMemoryPropertyFlags) {
		for (uint32_t u32PhysicalDeviceMemoryTypeIndex = 0U; u32PhysicalDeviceMemoryTypeIndex < vk_physicalDeviceMemoryProperties.memoryTypeCount; u32PhysicalDeviceMemoryTypeIndex++)
			if ((u32MemoryTypeBits & (1U << u32PhysicalDeviceMemoryTypeIndex)) && (vk_physicalDeviceMemoryProperties.memoryTypes[u32PhysicalDeviceMemoryTypeIndex].propertyFlags & vk_eMemoryPropertyFlags) == vk_eMemoryPropertyFlags)
				return u32PhysicalDeviceMemoryTypeIndex;
		RE_FATAL_ERROR("Failed to find required memory type");
		return VK_MAX_MEMORY_TYPES;
	}
	
	bool create_vulkan_buffer(const VkDeviceSize vk_size, const VkBufferUsageFlags vk_eUsages, const uint32_t u32QueueCount, const uint32_t *pu32Queues, const VkMemoryPropertyFlags vk_eMemoryPropertyFlags, VkBuffer *vk_phBuffer, VkDeviceMemory *vk_phMemory) {
		VkBufferCreateInfo vk_createInfo = {
			.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
			.size = vk_size,
			.usage = vk_eUsages
		};
		std::vector<uint32_t> queueFamilyIndices = get_queue_family_indices(u32QueueCount, pu32Queues);
		if (queueFamilyIndices.size() == 1U)
			vk_createInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		else {
			vk_createInfo.sharingMode = VK_SHARING_MODE_CONCURRENT;
			vk_createInfo.queueFamilyIndexCount = queueFamilyIndices.size();
			vk_createInfo.pQueueFamilyIndices = queueFamilyIndices.data();
		}
		if (vkCreateBuffer(vk_hDevice, &vk_createInfo, nullptr, vk_phBuffer)) {
			VkMemoryRequirements vk_memoryRequirements;
			vkGetBufferMemoryRequirements(vk_hDevice, *vk_phBuffer, &vk_memoryRequirements);
			const uint32_t u32MemoryTypeIndex = find_vulkan_device_memory_type(vk_memoryRequirements.memoryTypeBits, vk_eMemoryPropertyFlags);
			if (u32MemoryTypeIndex < VK_MAX_MEMORY_TYPES) {
				const VkMemoryAllocateInfo vk_allocInfo = {
					.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
					.allocationSize = vk_memoryRequirements.size,
					.memoryTypeIndex = u32MemoryTypeIndex
				};
				if (vkAllocateMemory(vk_hDevice, &vk_allocInfo, nullptr, vk_phMemory)) {
					if (vkBindBufferMemory(vk_hDevice, *vk_phBuffer, *vk_phMemory, 0UL))
						return true;
					vkFreeMemory(vk_hDevice, *vk_phMemory, nullptr);
					*vk_phMemory = VK_NULL_HANDLE;
				}
			}
			vkDestroyBuffer(vk_hDevice, *vk_phBuffer, nullptr);
			*vk_phBuffer = VK_NULL_HANDLE;
		}
		return false;
	}

}
