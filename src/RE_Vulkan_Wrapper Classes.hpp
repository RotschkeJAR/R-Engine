#ifndef __RE_VULKAN_WRAPPER_CLASSES_H__
#define __RE_VULKAN_WRAPPER_CLASSES_H__

#include "RE_Vulkan_Instance.hpp"

namespace RE {
	
	class Vulkan_Buffer final {
		private:
			VkBuffer vk_hBuffer;
			VkDeviceMemory vk_hMemory;

		public:
			Vulkan_Buffer();
			Vulkan_Buffer(VkBufferCreateFlags vk_eCreateFlags, VkDeviceSize vk_size, VkBufferUsageFlags vk_eUsages, uint32_t u32QueueCount, const uint32_t *pau32Queues, VkMemoryPropertyFlags vk_eMemoryPropertyFlags);
			~Vulkan_Buffer();

			bool init(VkBufferCreateFlags vk_eCreateFlags, VkDeviceSize vk_size, VkBufferUsageFlags vk_eUsages, uint32_t u32QueueCount, const uint32_t *pau32Queues, VkMemoryPropertyFlags vk_eMemoryPropertyFlags);
			void destroy();
			bool map_memory(VkDeviceSize vk_offset, VkDeviceSize vk_size, void **ppData) const;
			void unmap_memory() const;
			[[nodiscard]]
			VkBuffer get_buffer() const;
			[[nodiscard]]
			const VkBuffer* get_buffer_ptr() const;
			[[nodiscard]]
			VkDeviceMemory get_memory() const;
			[[nodiscard]]
			bool is_valid() const;

			[[nodiscard]]
			operator VkBuffer() const;
			[[nodiscard]]
			operator const VkBuffer*() const;
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
			const VkCommandBuffer* get_command_buffer_ptr() const;
			[[nodiscard]]
			bool is_valid() const;

			[[nodiscard]]
			operator VkCommandBuffer() const;
			[[nodiscard]]
			operator const VkCommandBuffer*() const;
	};

	class Vulkan_Fence final {
		private:
			VkFence vk_hFence;

		public:
			Vulkan_Fence();
			Vulkan_Fence(VkFenceCreateFlags vk_eCreateFlags);
			~Vulkan_Fence();

			bool wait_for() const;
			void reset() const;
			[[nodiscard]]
			VkFence get_fence() const;
			[[nodiscard]]
			const VkFence* get_fence_ptr() const;
			[[nodiscard]]
			bool is_valid() const;

			[[nodiscard]]
			operator VkFence() const;
			[[nodiscard]]
			operator const VkFence*() const;
	};

	class Vulkan_TimelineSemaphore final {
		private:
			VkSemaphore vk_hTimelineSemaphore;

		public:
			Vulkan_TimelineSemaphore();
			Vulkan_TimelineSemaphore(uint64_t u64InitialValue);
			~Vulkan_TimelineSemaphore();

			bool wait_for_reaching(uint64_t u64Value) const;
			void set_to(uint64_t u64Value) const;
			[[nodiscard]]
			VkSemaphore get_timeline_semaphore() const;
			[[nodiscard]]
			const VkSemaphore* get_timeline_semaphore_ptr() const;
			[[nodiscard]]
			bool is_valid() const;

			[[nodiscard]]
			operator VkSemaphore() const;
			[[nodiscard]]
			operator const VkSemaphore*() const;
	};

}

#endif /* __RE_VULKAN_WRAPPER_CLASSES_H__ */