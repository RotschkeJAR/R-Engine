#ifndef __RE_RENDERING_BUFFER_H__
#define __RE_RENDERING_BUFFER_H__

#include "RE_Vulkan.hpp"

namespace RE {
	
	class Rendering_Buffer {
		private:
			VkBuffer vk_hBuffer;
			VkDeviceMemory vk_hBufferMemory;

		public:
			Rendering_Buffer() = delete;
			Rendering_Buffer(VkDeviceSize vk_bufferSize, VkBufferUsageFlags vk_bufferUsage, VkSharingMode vk_bufferSharingMode, uint32_t u32QueueFamilyIndexCount, const uint32_t* pu32QueueFamilyIndices, VkMemoryPropertyFlags vk_memoryProperties);
			~Rendering_Buffer();
			VkBuffer get_buffer() const;
			VkDeviceMemory get_memory() const;
			bool is_valid() const;

			operator VkBuffer() const;
			operator VkDeviceMemory() const;
	};

}

#endif /* __RE_RENDERING_BUFFER_H__ */