#ifndef __RE_RENDERING_QUEUE_H__
#define __RE_RENDERING_QUEUE_H__

#include "RE_Vulkan.hpp"
#include "RE_Rendering_Sync Objects.hpp"
#include "RE_Rendering_Command Buffer.hpp"

namespace RE {
	
	class Rendering_Queue final {
		public:
			const VkQueue vk_hQueue;
			const uint32_t u32QueueIndex;

			Rendering_Queue() = delete;
			Rendering_Queue(const VkQueue vk_hQueue, const uint32_t u32QueueIndex);
			~Rendering_Queue();
			bool submit_to_queue(const VkSubmitInfo *vk_pSubmitInfo, const VkFence vk_hFence) const;
			bool submit_to_queue(const uint32_t u32SemaphoresToWaitForCount, const VkSemaphore *vk_phSemaphoresToWaitFor, const VkPipelineStageFlags *vk_pePipelineStageFlags, const uint32_t u32CommandBufferCount, const VkCommandBuffer *vk_phCommandBuffers, const uint32_t u32SemaphoresToSignalCount, const VkSemaphore *vk_phSemaphoresToSignal, const VkFence vk_hFence) const;
			bool submit_to_queue(const uint32_t u32SemaphoresToWaitForCount, const Rendering_Semaphore *pSemaphoresToWaitFor, const VkPipelineStageFlags *vk_pePipelineStageFlags, const uint32_t u32CommandBufferCount, const Rendering_CommandBuffer *pCommandBuffers, const uint32_t u32SemaphoresToSignalCount, const Rendering_Semaphore *pSemaphoresToSignal, const Rendering_Fence *pFence) const;
			void submit_to_present_queue(const VkPresentInfoKHR *vk_pPresentInfo) const;
			void submit_to_present_queue(const uint32_t u32SemaphoresToWaitForCount, const VkSemaphore *vk_phSemaphoresToWaitFor, const uint32_t u32SwapchainCount, const VkSwapchainKHR *vk_phSwapchains, const uint32_t *pu32SwapchainImageIndices) const;
			void submit_to_present_queue(const uint32_t u32SemaphoresToWaitForCount, const Rendering_Semaphore *pSemaphoresToWaitFor, const uint32_t u32SwapchainCount, const VkSwapchainKHR *vk_phSwapchains, const uint32_t *pu32SwapchainImageIndices) const;
			void wait_for_idle_queue() const;
			VkQueue get_queue() const;
			bool is_valid() const;

			operator VkQueue() const;
	};

}

#endif /* __RE_RENDERING_QUEUE_H__ */
