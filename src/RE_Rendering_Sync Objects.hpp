#ifndef __RE_RENDERING_SYNC_OBJECTS_H__
#define __RE_RENDERING_SYNC_OBJECTS_H__

#include "RE_Vulkan.hpp"

namespace RE {
	
	class Rendering_Semaphore {
		private:
			VkSemaphore vk_hSemaphore;

		public:
			Rendering_Semaphore();
			~Rendering_Semaphore();
			VkSemaphore get_semaphore() const;
			bool is_valid() const;

			operator VkSemaphore() const;
	};

	class Rendering_Fence {
		private:
			VkFence vk_hFence;

		public:
			Rendering_Fence();
			Rendering_Fence(VkFenceCreateFlagBits vk_eFenceCreateFlags);
			~Rendering_Fence();
			void wait_for_fence();
			void reset_fence();
			void wait_for_and_reset_fence();
			bool is_fence_signaled();
			VkFence get_fence() const;
			bool is_valid() const;

			operator VkFence() const;
	};

}

#endif /* __RE_RENDERING_SYNC_OBJECTS_H__ */
