#include "RE_Rendering_Queue.hpp"

namespace RE {
	
	Rendering_Queue::Rendering_Queue(const VkQueue vk_hQueue, const uint32_t u32QueueIndex) : vk_hQueue(vk_hQueue), u32QueueIndex(u32QueueIndex) {}
	Rendering_Queue::~Rendering_Queue() {}

	bool Rendering_Queue::submit_to_queue(const VkSubmitInfo *vk_pSubmitInfo, const VkFence vk_hFence) const {
		return CHECK_VK_RESULT(vkQueueSubmit(vk_hQueue, 1U, vk_pSubmitInfo, vk_hFence));
	}

	bool Rendering_Queue::submit_to_queue(const uint32_t u32SemaphoresToWaitForCount, const VkSemaphore *vk_phSemaphoresToWaitFor, const VkPipelineStageFlags *vk_pePipelineStageFlags, const uint32_t u32CommandBufferCount, const VkCommandBuffer *vk_phCommandBuffers, const uint32_t u32SemaphoresToSignalCount, const VkSemaphore *vk_phSemaphoresToSignal, const VkFence vk_hFence) const {
		VkSubmitInfo vk_queueGraphicsSubmitInfo = {};
		vk_queueGraphicsSubmitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		vk_queueGraphicsSubmitInfo.waitSemaphoreCount = u32SemaphoresToWaitForCount;
		vk_queueGraphicsSubmitInfo.pWaitSemaphores = vk_phSemaphoresToWaitFor;
		vk_queueGraphicsSubmitInfo.pWaitDstStageMask = vk_pePipelineStageFlags;
		vk_queueGraphicsSubmitInfo.commandBufferCount = u32CommandBufferCount;
		vk_queueGraphicsSubmitInfo.pCommandBuffers = vk_phCommandBuffers;
		vk_queueGraphicsSubmitInfo.signalSemaphoreCount = u32SemaphoresToSignalCount;
		vk_queueGraphicsSubmitInfo.pSignalSemaphores = vk_phSemaphoresToSignal;
		return CATCH_SIGNAL_AND_RETURN(submit_to_queue(&vk_queueGraphicsSubmitInfo, vk_hFence), bool);
	}
	
	bool Rendering_Queue::submit_to_queue(const uint32_t u32SemaphoresToWaitForCount, const Rendering_Semaphore *pSemaphoresToWaitFor, const VkPipelineStageFlags *vk_pePipelineStageFlags, const uint32_t u32CommandBufferCount, const Rendering_CommandBuffer *pCommandBuffers, const uint32_t u32SemaphoresToSignalCount, const Rendering_Semaphore *pSemaphoresToSignal, const Rendering_Fence *pFence) const {
		VkSemaphore *const vk_phSemaphoresToWaitFor = new VkSemaphore[u32SemaphoresToWaitForCount];
		for (uint32_t u32SemaphoresToWaitForIndex = 0U; u32SemaphoresToWaitForIndex < u32SemaphoresToWaitForCount; u32SemaphoresToWaitForIndex++)
			vk_phSemaphoresToWaitFor[u32SemaphoresToWaitForIndex] = pSemaphoresToWaitFor[u32SemaphoresToWaitForIndex].get_semaphore();

		VkCommandBuffer *const vk_phCommandBuffers = new VkCommandBuffer[u32CommandBufferCount];
		for (uint32_t u32CommandBufferIndex = 0U; u32CommandBufferIndex < u32CommandBufferCount; u32CommandBufferIndex++)
			vk_phCommandBuffers[u32CommandBufferIndex] = pCommandBuffers[u32CommandBufferIndex].get_command_buffer();

		VkSemaphore *const vk_phSemaphoresToSignal = new VkSemaphore[u32SemaphoresToSignalCount];
		for (uint32_t u32SemaphoresToSignalIndex = 0U; u32SemaphoresToSignalIndex < u32SemaphoresToSignalCount; u32SemaphoresToSignalIndex++)
			vk_phSemaphoresToSignal[u32SemaphoresToSignalIndex] = pSemaphoresToSignal[u32SemaphoresToSignalIndex].get_semaphore();

		const bool bSuccessResult = CATCH_SIGNAL_AND_RETURN(submit_to_queue(u32SemaphoresToWaitForCount, vk_phSemaphoresToWaitFor, vk_pePipelineStageFlags, u32CommandBufferCount, vk_phCommandBuffers, u32SemaphoresToSignalCount, vk_phSemaphoresToSignal, pFence->get_fence()), bool);
		delete[] vk_phSemaphoresToWaitFor;
		delete[] vk_phCommandBuffers;
		delete[] vk_phSemaphoresToSignal;
		return bSuccessResult;
	}

	void Rendering_Queue::submit_to_present_queue(const VkPresentInfoKHR *const vk_pPresentInfo) const {
		CATCH_SIGNAL(vkQueuePresentKHR(vk_hQueue, vk_pPresentInfo));
	}

	void Rendering_Queue::submit_to_present_queue(const uint32_t u32SemaphoresToWaitForCount, const VkSemaphore *vk_phSemaphoresToWaitFor, const uint32_t u32SwapchainCount, const VkSwapchainKHR *vk_phSwapchains, const uint32_t *pu32SwapchainImageIndices) const {
		VkPresentInfoKHR vk_queuePresentSubmitInfo = {};
		vk_queuePresentSubmitInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
		vk_queuePresentSubmitInfo.waitSemaphoreCount = u32SemaphoresToWaitForCount;
		vk_queuePresentSubmitInfo.pWaitSemaphores = vk_phSemaphoresToWaitFor;
		vk_queuePresentSubmitInfo.swapchainCount = u32SwapchainCount;
		vk_queuePresentSubmitInfo.pSwapchains = vk_phSwapchains;
		vk_queuePresentSubmitInfo.pImageIndices = pu32SwapchainImageIndices;
		CATCH_SIGNAL(submit_to_present_queue(&vk_queuePresentSubmitInfo));
	}

	void Rendering_Queue::submit_to_present_queue(const uint32_t u32SemaphoresToWaitForCount, const Rendering_Semaphore *pSemaphoresToWaitFor, const uint32_t u32SwapchainCount, const VkSwapchainKHR *vk_phSwapchains, const uint32_t *pu32SwapchainImageIndices) const {
		VkSemaphore *const vk_phSemaphoresToWaitFor = new VkSemaphore[u32SemaphoresToWaitForCount];
		for (uint32_t u32SemaphoreToWaitForIndex = 0U; u32SemaphoreToWaitForIndex < u32SemaphoresToWaitForCount; u32SemaphoreToWaitForIndex++)
			vk_phSemaphoresToWaitFor[u32SemaphoreToWaitForIndex] = pSemaphoresToWaitFor[u32SemaphoreToWaitForIndex].get_semaphore();

		CATCH_SIGNAL(submit_to_present_queue(u32SemaphoresToWaitForCount, vk_phSemaphoresToWaitFor, u32SwapchainCount, vk_phSwapchains, pu32SwapchainImageIndices));
		delete[] vk_phSemaphoresToWaitFor;
	}

	void Rendering_Queue::wait_for_idle_queue() const {
		CATCH_SIGNAL(vkQueueWaitIdle(vk_hQueue));
	}

	VkQueue Rendering_Queue::get_queue() const {
		return vk_hQueue;
	}

	bool Rendering_Queue::is_valid() const {
		return vk_hQueue != VK_NULL_HANDLE;
	}

	Rendering_Queue::operator VkQueue() const {
		return this->vk_hQueue;
	}

}
