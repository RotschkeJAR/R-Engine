#ifndef __RE_VULKAN_WRAPPER_CLASSES_H__
#define __RE_VULKAN_WRAPPER_CLASSES_H__

#include "RE_Vulkan_Instance.hpp"

namespace RE {
	
	class Vulkan_Buffer final {
		private:
			VkBuffer vk_hBuffer;
			VkDeviceMemory vk_hMemory;

		public:
			Vulkan_Buffer() = delete;
			Vulkan_Buffer(const VkDeviceSize vk_size, const VkBufferUsageFlags vk_eUsages, const uint32_t u32QueueCount, const uint32_t *pu32Queues, const VkMemoryPropertyFlags vk_eMemoryPropertyFlags);
			~Vulkan_Buffer();

			bool is_valid() const;
	};

}

#endif /* __RE_VULKAN_WRAPPER_CLASSES_H__ */