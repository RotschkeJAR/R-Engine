#include "RE_Renderer_RenderProcedure_Internal.hpp"
#include "RE_Window.hpp"

namespace RE {

#ifdef RENDERER_INCLUDE_EMPTY_PRESENT

	bool present_empty() {
		if (!acquire_next_swapchain_image())
			return true;
		PRINT_DEBUG("Waiting for Vulkan fence ", vk_hEmptyPresentFence, " to synchronize empty presentation");
		switch (vkWaitForFences(vk_hDevice, 1, &vk_hEmptyPresentFence, VK_TRUE, std::chrono::nanoseconds(RENDERING_TIMEOUT_SEC).count())) {
			case VK_SUCCESS:
				PRINT_DEBUG("Resetting Vulkan command pool ", vk_hCommandPoolEmptyPresent, " for empty presentation");
				if (vkResetCommandPool(vk_hDevice, vk_hCommandPoolEmptyPresent, 0) == VK_SUCCESS) {
					PRINT_DEBUG("Beginning recording of Vulkan command buffer ", vk_hCommandBufferEmptyPresent, " for empty presentation");
					if (begin_recording_vulkan_command_buffer(
							vk_hCommandBufferEmptyPresent,
							VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
							nullptr)) {
						PRINT_DEBUG("Recording pipeline barrier for image layout transition in Vulkan command buffer ", vk_hCommandBufferEmptyPresent, " for empty presentation");
						const VkImageMemoryBarrier vk_swapchainImageLayoutTransferBarrier = {
							.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
							.pNext = nullptr,
							.srcAccessMask = VK_ACCESS_NONE,
							.dstAccessMask = VK_ACCESS_NONE,
							.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED,
							.newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
							.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
							.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
							.image = std_swapchainImages[u32CurrentSwapchainImageIndex],
							.subresourceRange = {
								.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
								.baseMipLevel = 0,
								.levelCount = 1,
								.baseArrayLayer = 0,
								.layerCount = 1
							}
						};
						vkCmdPipelineBarrier(
								vk_hCommandBufferEmptyPresent,
								VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
								VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
								0,
								0,
								nullptr,
								0,
								nullptr,
								1,
								&vk_swapchainImageLayoutTransferBarrier);
						PRINT_DEBUG("Ending recording of Vulkan command buffer ", vk_hCommandBufferEmptyPresent, " for empty presentation");
						if (vkEndCommandBuffer(vk_hCommandBufferEmptyPresent) == VK_SUCCESS) {
							PRINT_DEBUG("Resetting Vulkan fence ", vk_hEmptyPresentFence, " for empty presentation");
							if (vkResetFences(vk_hDevice, 1, &vk_hEmptyPresentFence) == VK_SUCCESS) {
								PRINT_DEBUG("Submitting command buffer to queue on GPU for empty presentation");
								constexpr VkPipelineStageFlags vk_amWaitStages[] = {
									VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT
								};
								const VkSubmitInfo vk_emptyPresentInfo = {
									.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
									.pNext = nullptr,
									.waitSemaphoreCount = 1,
									.pWaitSemaphores = &swapchainSemaphores[u32CurrentSwapchainSemaphoreIndex * RE_VK_SEMAPHORES_PER_SWAPCHAIN_IMAGE],
									.pWaitDstStageMask = vk_amWaitStages,
									.commandBufferCount = 1,
									.pCommandBuffers = &vk_hCommandBufferEmptyPresent,
									.signalSemaphoreCount = 1,
									.pSignalSemaphores = &swapchainSemaphores[u32CurrentSwapchainSemaphoreIndex * RE_VK_SEMAPHORES_PER_SWAPCHAIN_IMAGE + 1]
								};
								if (vkQueueSubmit(
										std_queues[aRenderTasks[0].logical_queue_index_for_function(RENDER_TASK_SUBINDEX_IMAGE_BLIT)],
										1,
										&vk_emptyPresentInfo,
										vk_hEmptyPresentFence) == VK_SUCCESS) {
									present_swapchain_image();
									return true;
								} else
									RE_FATAL_ERROR("Failed to submit the Vulkan command buffer for empty presentation");
							} else
								RE_FATAL_ERROR("Failed to reset Vulkan fence ", vk_hEmptyPresentFence, " used to synchronize empty presentation");
						} else
							RE_FATAL_ERROR("Failed to end recording Vulkan command buffer ", vk_hCommandBufferEmptyPresent, " for empty presentation");
					} else
						RE_FATAL_ERROR("Failed to begin recording Vulkan command buffer ", vk_hCommandBufferEmptyPresent, " for empty presentation");
				} else
					RE_FATAL_ERROR("Failed to reset Vulkan command pool ", vk_hCommandPoolEmptyPresent, " for empty presentation");
				break;
			case VK_TIMEOUT:
				RE_ABORT("Empty presentation timed out after ", RENDERING_TIMEOUT_SEC, " seconds");
			case VK_ERROR_DEVICE_LOST:
				RE_ABORT("Failed to synchronize with empty presentation. Suspecting fatal error (device lost)");
			default:
				RE_FATAL_ERROR("Failed to wait for the signaling of Vulkan fence ", vk_hEmptyPresentFence, " used to synchronize empty presentation");
				break;
		}
		return false;
	}

#endif

}
