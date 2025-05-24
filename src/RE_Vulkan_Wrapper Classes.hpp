#ifndef __RE_VULKAN_WRAPPER_CLASSES_H__
#define __RE_VULKAN_WRAPPER_CLASSES_H__

namespace RE {
	
	class Vulkan_Buffer final {
		private:
			VkBuffer vk_hBuffer;
			VkDeviceMemory vk_hMemory;

		public:
			const VkDeviceSize vk_sizeInBytes;

			Vulkan_Buffer() = delete;
			Vulkan_Buffer(const VkBufferCreateFlags vk_eCreateFlags, const VkDeviceSize vk_sizeInBytes, const VkBufferUsageFlags vk_usage, const uint32_t u32QueueTypeCount, const uint32_t *pu32BufferQueueTypeIndices, const VkMemoryPropertyFlags vk_memoryProperties);
			~Vulkan_Buffer();
			bool map_memory(void **ppData) const;
			void unmap_memory() const;
			bool is_valid() const;
			VkBuffer get_buffer_handle() const;
			VkDeviceMemory get_memory_handle() const;
			VkBuffer* get_buffer_handle_ptr() const;
			VkDeviceMemory* get_memory_handle_ptr() const;

			operator VkBuffer() const;
			operator VkDeviceMemory() const;
			operator VkBuffer*() const;
			operator VkDeviceMemory*() const;
	};

}

#endif /* __RE_VULKAN_WRAPPER_CLASSES_H__ */