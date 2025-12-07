#include "RE_Renderer_Internal.hpp"

namespace RE {

	VkQueue vk_hPresentQueue;
	std::vector<VkSemaphore> swapchainSemaphores;
	uint32_t u32CurrentSwapchainSemaphoreIndex = 0, u32SwapchainImageIndex;

	bool setup_presentation() {
		PRINT_DEBUG("Creating ", RE_VK_SWAPCHAIN_SEMAPHORE_COUNT, " Vulkan swapchain semaphores");
		const VkSemaphoreCreateInfo vk_swapchainSemaphoreCreateInfo = {
			.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO
		};
		swapchainSemaphores.reserve(RE_VK_SWAPCHAIN_SEMAPHORE_COUNT);
		while (swapchainSemaphores.size() < RE_VK_SWAPCHAIN_SEMAPHORE_COUNT) {
			VkSemaphore vk_hCreatedSemaphore;
			if (vkCreateSemaphore(vk_hDevice, &vk_swapchainSemaphoreCreateInfo, nullptr, &vk_hCreatedSemaphore) != VK_SUCCESS) {
				RE_FATAL_ERROR("Failed creating a Vulkan semaphore for synchronizing presentation operations");
				break;
			}
			swapchainSemaphores.push_back(vk_hCreatedSemaphore);
		}
		if (swapchainSemaphores.size() == RE_VK_SWAPCHAIN_SEMAPHORE_COUNT) {
			vk_hPresentQueue = vk_pahQueues[renderTasks[0].get_logical_queue_index_for_presentation()];
			return true;
		}
		destroy_presentation();
		return false;
	}

	void destroy_presentation() {
		PRINT_DEBUG("Destroying ", RE_VK_SWAPCHAIN_SEMAPHORE_COUNT, " Vulkan swapchain semaphores");
		for (const VkSemaphore vk_hSwapchainSemaphore : swapchainSemaphores)
			vkDestroySemaphore(vk_hDevice, vk_hSwapchainSemaphore, nullptr);
		swapchainSemaphores.clear();
		swapchainSemaphores.shrink_to_fit();
		u32CurrentSwapchainSemaphoreIndex = 0;
	}

	bool acquire_next_swapchain_image() {
		PRINT_DEBUG("Acquiring next Vulkan swapchain image");
		const VkAcquireNextImageInfoKHR vk_acquireInfo = {
			.sType = VK_STRUCTURE_TYPE_ACQUIRE_NEXT_IMAGE_INFO_KHR,
			.swapchain = vk_hSwapchain,
			.timeout = std::numeric_limits<uint64_t>::max(),
			.semaphore = swapchainSemaphores[u32CurrentSwapchainSemaphoreIndex * RE_VK_SEMAPHORES_PER_SWAPCHAIN_IMAGE],
			.deviceMask = 1
		};
		switch (vkAcquireNextImage2KHR(vk_hDevice, &vk_acquireInfo, &u32SwapchainImageIndex)) {
			case VK_SUCCESS:
				return true;
			case VK_SUBOPTIMAL_KHR:
			case VK_ERROR_OUT_OF_DATE_KHR:
				mark_swapchain_dirty();
				return true;
			default:
				return false;
		}
	}

	bool present_swapchain_image() {
		PRINT_DEBUG("Submitting swapchain image at index ", u32SwapchainImageIndex, " to presentation");
		const VkPresentInfoKHR vk_presentInfo = {
			.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
			.waitSemaphoreCount = 1,
			.pWaitSemaphores = &swapchainSemaphores[u32CurrentSwapchainSemaphoreIndex * RE_VK_SEMAPHORES_PER_SWAPCHAIN_IMAGE + 1],
			.swapchainCount = 1,
			.pSwapchains = &vk_hSwapchain,
			.pImageIndices = &u32SwapchainImageIndex
		};
		switch (vkQueuePresentKHR(vk_hPresentQueue, &vk_presentInfo)) {
			case VK_SUCCESS:
				return true;
			case VK_SUBOPTIMAL_KHR:
			case VK_ERROR_OUT_OF_DATE_KHR:
				mark_swapchain_dirty();
				return true;
			default:
				return false;
		}
	}

}
