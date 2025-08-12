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
			Vulkan_Buffer(VkDeviceSize vk_size, VkBufferUsageFlags vk_eUsages, uint32_t u32QueueCount, const uint32_t *pau32Queues, VkMemoryPropertyFlags vk_eMemoryPropertyFlags);
			~Vulkan_Buffer();

			bool map_memory(void **ppData) const;
			void unmap_memory() const;
			[[nodiscard]]
			VkBuffer get_buffer() const;
			[[nodiscard]]
			VkDeviceMemory get_memory() const;
			[[nodiscard]]
			bool is_valid() const;

			[[nodiscard]]
			operator VkBuffer() const;
			[[nodiscard]]
			operator VkDeviceMemory() const;
	};

	class Vulkan_CommandBuffer final {
		private:
			VkCommandBuffer vk_hCommandBuffer;

		public:
			const VkCommandPool vk_hCommandPool;

			Vulkan_CommandBuffer() = delete;
			Vulkan_CommandBuffer(VkCommandPool vk_hCommandPool, VkCommandBufferLevel vk_eCommandBufferLevel);
			~Vulkan_CommandBuffer();

			bool begin_recording(VkCommandBufferUsageFlags vk_eUsageFlags, const VkCommandBufferInheritanceInfo *vk_pInheritanceInfo) const;
			bool end_recording() const;
			[[nodiscard]]
			VkCommandBuffer get_command_buffer() const;
			[[nodiscard]]
			VkCommandBuffer* get_command_buffer_ptr();
			[[nodiscard]]
			bool is_valid() const;

			[[nodiscard]]
			operator VkCommandBuffer() const;
			[[nodiscard]]
			operator VkCommandBuffer*();
	};

	class Vulkan_Fence final {
		private:
			VkFence vk_hFence;

		public:
			Vulkan_Fence();
			Vulkan_Fence(VkFenceCreateFlags vk_eCreateFlags);
			~Vulkan_Fence();

			bool wait_for() const;
			[[nodiscard]]
			VkFence get_fence() const;
			[[nodiscard]]
			bool is_valid() const;

			[[nodiscard]]
			operator VkFence() const;
	};

}

#endif /* __RE_VULKAN_WRAPPER_CLASSES_H__ */