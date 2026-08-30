#include "RE_Renderer_Presentation.hpp"
#include "RE_Main.hpp"

namespace RE {

	VkQueue vk_hPresentQueue;
	std::unique_ptr<VkSemaphore[]> swapchainSemaphores;
	uint32_t u32CurrentSwapchainSemaphoreIndex = 0,
		u32CurrentSwapchainImageIndex;

	bool setup_presentation() {
		PRINT_DEBUG("Creating ", RE_VK_SWAPCHAIN_SEMAPHORE_COUNT, " Vulkan swapchain semaphores");
		const VkSemaphoreCreateInfo vk_swapchainSemaphoreCreateInfo = {
			.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO
		};
		swapchainSemaphores = std::make_unique<VkSemaphore[]>(RE_VK_SWAPCHAIN_SEMAPHORE_COUNT);
		unsigned uSemaphoreCreateIndex = 0;
		while (uSemaphoreCreateIndex < RE_VK_SWAPCHAIN_SEMAPHORE_COUNT) {
			if (vkCreateSemaphore(vk_hDevice, &vk_swapchainSemaphoreCreateInfo, nullptr, &swapchainSemaphores[uSemaphoreCreateIndex]) == VK_SUCCESS) {
				uSemaphoreCreateIndex++;
				continue;
			} else
				RE_FATAL_ERROR("Failed creating a Vulkan semaphore for synchronizing presentation operations at index ", uSemaphoreCreateIndex);
			break;
		}
		if (uSemaphoreCreateIndex == RE_VK_SWAPCHAIN_SEMAPHORE_COUNT) {
			vk_hPresentQueue = std_queues[aRenderTasks[0].logical_queue_index_for_presentation()];
			return true;
		}
		for (unsigned uSemaphoreDestroyIndex = 0; uSemaphoreDestroyIndex < uSemaphoreCreateIndex; uSemaphoreDestroyIndex++)
			vkDestroySemaphore(vk_hDevice, swapchainSemaphores[uSemaphoreDestroyIndex], nullptr);
		return false;
	}

	void destroy_presentation() {
		PRINT_DEBUG("Destroying ", RE_VK_SWAPCHAIN_SEMAPHORE_COUNT, " Vulkan swapchain semaphores");
		for (unsigned uSemaphoreDestroyIndex = 0; uSemaphoreDestroyIndex < RE_VK_SWAPCHAIN_SEMAPHORE_COUNT; uSemaphoreDestroyIndex++)
			vkDestroySemaphore(vk_hDevice, swapchainSemaphores[uSemaphoreDestroyIndex], nullptr);
		swapchainSemaphores.reset();
		u32CurrentSwapchainSemaphoreIndex = 0;
	}

	bool acquire_next_swapchain_image() {
		PRINT_DEBUG("Acquiring index to the next Vulkan swapchain image");
		const VkResult vk_eAcquireResult = vkAcquireNextImageKHR(vk_hDevice, vk_hSwapchain, std::numeric_limits<uint64_t>::max(), swapchainSemaphores[u32CurrentSwapchainSemaphoreIndex * RE_VK_SEMAPHORES_PER_SWAPCHAIN_IMAGE], VK_NULL_HANDLE, &u32CurrentSwapchainImageIndex);
		switch (vk_eAcquireResult) {
			case VK_SUCCESS:
				return true;
			case VK_SUBOPTIMAL_KHR:
				mark_swapchain_dirty();
				return true;
			case VK_ERROR_OUT_OF_DATE_KHR:
				mark_swapchain_dirty();
				return false;
			default:
				RE_FATAL_ERROR("Failed to acquire the index of the next swapchain image for displaying. Return code: ", std::hex, vk_eAcquireResult);
				return false;
		}
	}

	bool present_swapchain_image() {
		PRINT_DEBUG("Submitting swapchain image at index ", u32CurrentSwapchainImageIndex, " to presentation");
		const VkPresentInfoKHR vk_presentInfo = {
			.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
			.waitSemaphoreCount = 1,
			.pWaitSemaphores = &swapchainSemaphores[u32CurrentSwapchainSemaphoreIndex * RE_VK_SEMAPHORES_PER_SWAPCHAIN_IMAGE + 1],
			.swapchainCount = 1,
			.pSwapchains = &vk_hSwapchain,
			.pImageIndices = &u32CurrentSwapchainImageIndex
		};
		const VkResult vk_ePresentResult = vkQueuePresentKHR(vk_hPresentQueue, &vk_presentInfo);
		switch (vk_ePresentResult) {
			case VK_SUCCESS:
				return true;
			case VK_SUBOPTIMAL_KHR:
			case VK_ERROR_OUT_OF_DATE_KHR:
				mark_swapchain_dirty();
				return true;
			default:
				RE_FATAL_ERROR("Failed to submit swapchain image at index ", u32CurrentSwapchainImageIndex, " to presentation. Return code: ", std::hex, vk_ePresentResult);
				return false;
		}
	}

}
