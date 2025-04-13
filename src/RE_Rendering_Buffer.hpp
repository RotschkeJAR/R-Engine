#ifndef __RE_RENDERING_BUFFER_H__
#define __RE_RENDERING_BUFFER_H__

#include "RE_Vulkan.hpp"

namespace RE {
	
	class Rendering_Buffer final {
		private:
			VkBuffer vk_hBuffer;
			VkDeviceMemory vk_hBufferMemory;

		public:
			const VkDeviceSize vk_bufferSizeInBytes;

			Rendering_Buffer() = delete;
			Rendering_Buffer(const VkDeviceSize vk_bufferSizeInBytes, const VkBufferUsageFlags vk_bufferUsage, const VkSharingMode vk_bufferSharingMode, const uint32_t u32QueueFamilyIndexCount, const uint32_t* pu32QueueFamilyIndices, const VkMemoryPropertyFlags vk_memoryProperties);
			~Rendering_Buffer();
			void upload_data(const void *const pData, const VkDeviceSize vk_uploadSize) const;
			VkBuffer get_buffer() const;
			VkDeviceMemory get_memory() const;
			bool is_valid() const;

			operator VkBuffer() const;
			operator VkDeviceMemory() const;
	};

}

#endif /* __RE_RENDERING_BUFFER_H__ */