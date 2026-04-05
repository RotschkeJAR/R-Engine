#include "RE_Renderer_Internal.hpp"
#include "RE_Main.hpp"

namespace RE {

	VkQueue vk_hPresentQueue;
	std::unique_ptr<VkSemaphore[]> swapchainSemaphores;
	uint32_t u32IndexToSelectedSurfaceFormat, u32CurrentSwapchainSemaphoreIndex = 0, u32SwapchainImageIndex;
	bool bVsyncEnabled = true;

	bool setup_presentation() {
		PRINT_DEBUG("Creating ", RE_VK_SWAPCHAIN_SEMAPHORE_COUNT, " Vulkan swapchain semaphores");
		const VkSemaphoreCreateInfo vk_swapchainSemaphoreCreateInfo = {
			.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO
		};
		swapchainSemaphores = std::make_unique<VkSemaphore[]>(RE_VK_SWAPCHAIN_SEMAPHORE_COUNT);
		uint16_t u16SemaphoreCreateIndex = 0;
		while (u16SemaphoreCreateIndex < RE_VK_SWAPCHAIN_SEMAPHORE_COUNT) {
			if (vkCreateSemaphore(vk_hDevice, &vk_swapchainSemaphoreCreateInfo, nullptr, &swapchainSemaphores[u16SemaphoreCreateIndex]) == VK_SUCCESS) {
				u16SemaphoreCreateIndex++;
				continue;
			} else
				RE_FATAL_ERROR("Failed creating a Vulkan semaphore for synchronizing presentation operations at index ", u16SemaphoreCreateIndex);
			break;
		}
		if (u16SemaphoreCreateIndex == RE_VK_SWAPCHAIN_SEMAPHORE_COUNT) {
			vk_hPresentQueue = vk_pahQueues[renderTasks[0].logical_queue_index_for_presentation()];
			return true;
		}
		for (uint16_t u16SemaphoreDestroyIndex = 0; u16SemaphoreDestroyIndex < u16SemaphoreCreateIndex; u16SemaphoreDestroyIndex++)
			vkDestroySemaphore(vk_hDevice, swapchainSemaphores[u16SemaphoreDestroyIndex], nullptr);
		return false;
	}

	void destroy_presentation() {
		PRINT_DEBUG("Destroying ", RE_VK_SWAPCHAIN_SEMAPHORE_COUNT, " Vulkan swapchain semaphores");
		for (uint16_t u16SemaphoreDestroyIndex = 0; u16SemaphoreDestroyIndex < RE_VK_SWAPCHAIN_SEMAPHORE_COUNT; u16SemaphoreDestroyIndex++)
			vkDestroySemaphore(vk_hDevice, swapchainSemaphores[u16SemaphoreDestroyIndex], nullptr);
		swapchainSemaphores.reset();
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
		const VkResult vk_eAcquireResult = vkAcquireNextImage2KHR(vk_hDevice, &vk_acquireInfo, &u32SwapchainImageIndex);
		switch (vk_eAcquireResult) {
			case VK_SUCCESS:
				return true;
			case VK_SUBOPTIMAL_KHR:
			case VK_ERROR_OUT_OF_DATE_KHR:
				mark_swapchain_dirty();
				return true;
			default:
				RE_FATAL_ERROR("Failed to acquire the index of the next swapchain image for displaying. Return code: ", std::hex, vk_eAcquireResult);
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
		const VkResult vk_ePresentResult = vkQueuePresentKHR(vk_hPresentQueue, &vk_presentInfo);
		switch (vk_ePresentResult) {
			case VK_SUCCESS:
				return true;
			case VK_SUBOPTIMAL_KHR:
			case VK_ERROR_OUT_OF_DATE_KHR:
				mark_swapchain_dirty();
				return true;
			default:
				RE_FATAL_ERROR("Failed to submit swapchain image at index ", u32SwapchainImageIndex, " to presentation. Return code: ", std::hex, vk_ePresentResult);
				return false;
		}
	}

	void enable_vsync(const bool bEnableVsync) {
		bVsyncEnabled = bEnableVsync;
		bSwapchainDirty = bRunning;
	}

	[[nodiscard]]
	bool is_vsync_enabled() {
		return bVsyncEnabled;
	}

}
