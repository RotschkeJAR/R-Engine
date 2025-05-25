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
			Vulkan_Buffer(const VkDeviceSize vk_sizeInBytes, const VkBufferUsageFlags vk_usage, const uint32_t u32QueueTypeCount, const uint32_t *pu32BufferQueueTypeIndices, const VkMemoryPropertyFlags vk_memoryProperties);
			~Vulkan_Buffer();
			bool map_memory(void **ppData) const;
			void unmap_memory() const;
			bool is_valid() const;
			VkBuffer get_buffer_handle() const;
			VkDeviceMemory get_memory_handle() const;
			VkBuffer const* get_buffer_handle_ptr() const;
			VkDeviceMemory const* get_memory_handle_ptr() const;

			operator VkBuffer() const;
			operator VkDeviceMemory() const;
			operator VkBuffer const*() const;
			operator VkDeviceMemory const*() const;
	};

	class Vulkan_Semaphore final {
		private:
			VkSemaphore vk_hSemaphore;

		public:
			Vulkan_Semaphore();
			~Vulkan_Semaphore();
			bool signal() const;
			bool is_valid() const;
			VkSemaphore get_semaphore_handle() const;
			VkSemaphore const* get_semaphore_handle_ptr() const;

			operator VkSemaphore() const;
			operator VkSemaphore const*() const;
	};

	class Vulkan_Fence final {
		private:
			VkFence vk_hFence;

		public:
			Vulkan_Fence();
			Vulkan_Fence(const VkFenceCreateFlags vk_eCreateFlags);
			~Vulkan_Fence();
			void wait_for() const;
			void reset() const;
			void wait_for_and_reset() const;
			bool signal() const;
			bool is_valid() const;
			VkFence get_fence_handle() const;
			VkFence const* get_fence_handle_ptr() const;

			operator VkFence() const;
			operator VkFence const*() const;
	};

	class Vulkan_CommandBuffer final {
		private:
			VkCommandBuffer vk_hCommandBuffer;

		public:
			const VkCommandPool vk_hCommandPool;

			Vulkan_CommandBuffer() = delete;
			Vulkan_CommandBuffer(const VkCommandPool vk_hCommandPool, const VkCommandBufferLevel vk_eCommandBufferLevel);
			~Vulkan_CommandBuffer();
			bool begin_recording(const VkCommandBufferUsageFlags vk_eUsageFlags, const VkCommandBufferInheritanceInfo *vk_pInheritanceInfo) const;
			void end_recording() const;
			void cmd_copy_buffers(const VkBuffer vk_hSrcBuffer, const VkBuffer vk_hDstBuffer, const VkDeviceSize vk_bytesToCopy) const;
			bool is_valid() const;
			VkCommandBuffer get_command_buffer_handle() const;
			VkCommandBuffer const* get_command_buffer_handle_ptr() const;

			operator VkCommandBuffer() const;
			operator VkCommandBuffer const*() const;
	};

}

#endif /* __RE_VULKAN_WRAPPER_CLASSES_H__ */