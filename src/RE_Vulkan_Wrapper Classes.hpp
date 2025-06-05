#ifndef __RE_VULKAN_WRAPPER_CLASSES_H__
#define __RE_VULKAN_WRAPPER_CLASSES_H__

#include "RE_Vulkan_Instance.hpp"

namespace RE {
	
	class Vulkan_Buffer final {
		private:
			VkBuffer vk_hBuffer;
			VkDeviceMemory vk_hMemory;

		public:
			const VkDeviceSize vk_size;

			Vulkan_Buffer() = delete;
			Vulkan_Buffer(const VkDeviceSize vk_size, const VkBufferUsageFlags vk_eUsages, const uint32_t u32QueueCount, const uint32_t *pu32Queues, const VkMemoryPropertyFlags vk_eMemoryPropertyFlags);
			~Vulkan_Buffer();

			bool map_memory(void **ppData) const;
			void unmap_memory() const;
			VkBuffer get_buffer() const;
			VkDeviceMemory get_memory() const;
			bool is_valid() const;

			operator VkBuffer() const;
			operator VkDeviceMemory() const;
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
			bool end_recording() const;
			VkCommandBuffer get_command_buffer() const;
			VkCommandBuffer* get_command_buffer_ptr();
			bool is_valid() const;

			operator VkCommandBuffer() const;
			operator VkCommandBuffer*();
	};

	class Vulkan_Fence final {
		private:
			VkFence vk_hFence;

		public:
			Vulkan_Fence();
			Vulkan_Fence(const VkFenceCreateFlags vk_eCreateFlags);
			~Vulkan_Fence();

			bool wait_for() const;
			VkFence get_fence() const;
			bool is_valid() const;

			operator VkFence() const;
	};

}

#endif /* __RE_VULKAN_WRAPPER_CLASSES_H__ */