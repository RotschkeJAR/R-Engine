#ifndef __RE_VULKAN_SYNC_OBJECTS_H__
#define __RE_VULKAN_SYNC_OBJECTS_H__

#include "RE_Vulkan.hpp"

namespace RE {
	
	class Vulkan_Semaphore final {
		private:
			VkSemaphore vk_hSemaphore;

		public:
			Vulkan_Semaphore();
			~Vulkan_Semaphore();
			VkSemaphore get_semaphore() const;
			bool is_valid() const;

			operator VkSemaphore() const;
	};

	class Vulkan_Fence final {
		private:
			VkFence vk_hFence;

		public:
			Vulkan_Fence();
			Vulkan_Fence(VkFenceCreateFlagBits vk_eFenceCreateFlags);
			~Vulkan_Fence();
			void wait_for_fence() const;
			void reset_fence() const;
			void wait_for_and_reset_fence() const;
			bool is_fence_signaled() const;
			VkFence get_fence() const;
			bool is_valid() const;

			operator VkFence() const;
	};

}

#endif /* __RE_VULKAN_SYNC_OBJECTS_H__ */
