#include "RE_Renderer_RenderProcedure_Internal.hpp"
#include "RE_Window.hpp"

namespace RE {
	
	bool render_procedure() {
		PRINT_DEBUG("Waiting for rendering finishing at fence index ", uCurrentFrameInFlightIndex);
		switch (vkWaitForFences(vk_hDevice, 1, &vk_ahRenderFences[uCurrentFrameInFlightIndex], VK_TRUE, std::chrono::nanoseconds(RENDERING_TIMEOUT_SEC).count())) {
			case VK_SUCCESS:
				if (!acquire_next_swapchain_image())
					return true;
				calculate_camera_matrices();
				apGameObjectsCountBufferData[uCurrentFrameInFlightIndex]->u32Count = 1;
				if (aRenderTasks[uCurrentFrameInFlightIndex].record(
						RENDER_TASK_SUBINDEX_BUFFER_TRANSFER,
						VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
						[&](VkCommandBuffer vk_hCommandBuffer, uint8_t u8PreviousLogicalQueue, uint8_t u8CurrentLogicalQueue, uint8_t u8NextLogicalQueue) {
							PRINT_DEBUG("Recording the buffer transfer subprocedure into Vulkan command buffer ", vk_hCommandBuffer);
							copy_staging_game_objects(vk_hCommandBuffer);
						})) {
					if (aRenderTasks[uCurrentFrameInFlightIndex].record(
							RENDER_TASK_SUBINDEX_PROCESSING,
							VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
							[&](VkCommandBuffer vk_hCommandBuffer, uint8_t u8PreviousLogicalQueue, uint8_t u8CurrentLogicalQueue, uint8_t u8NextLogicalQueue) {
								PRINT_DEBUG("Recording the processing subprocedure into Vulkan command buffer ", vk_hCommandBuffer);
								process_game_objects(vk_hCommandBuffer);
								const VkBufferMemoryBarrier2 vk_bufferBarrierInfo = {
									.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER_2,
									.pNext = nullptr,
									.srcStageMask = VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT,
									.srcAccessMask = VK_ACCESS_2_SHADER_STORAGE_WRITE_BIT,
									.dstStageMask = VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT,
									.dstAccessMask = VK_ACCESS_2_SHADER_STORAGE_READ_BIT | VK_ACCESS_2_SHADER_STORAGE_WRITE_BIT,
									.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
									.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
									.buffer = vk_ahSortableDepthBuffers[uCurrentFrameInFlightIndex],
									.offset = 0,
									.size = VK_WHOLE_SIZE
								};
								const VkDependencyInfo vk_dependencyInfo = {
									.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO,
									.pNext = nullptr,
									.dependencyFlags = 0,
									.memoryBarrierCount = 0,
									.pMemoryBarriers = nullptr,
									.bufferMemoryBarrierCount = 1,
									.pBufferMemoryBarriers = &vk_bufferBarrierInfo,
									.imageMemoryBarrierCount = 0,
									.pImageMemoryBarriers = nullptr
								};
								vkCmdPipelineBarrier2(vk_hCommandBuffer, &vk_dependencyInfo);
								sort_game_objects(vk_hCommandBuffer);
							})) {
						if (aRenderTasks[uCurrentFrameInFlightIndex].record(
								RENDER_TASK_SUBINDEX_RENDERING,
								VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
								[&](VkCommandBuffer vk_hCommandBuffer, uint8_t u8PreviousLogicalQueue, uint8_t u8CurrentLogicalQueue, uint8_t u8NextLogicalQueue) {
									PRINT_DEBUG("Recording the rendering subprocedure into Vulkan command buffer ", vk_hCommandBuffer);
									render_pass(vk_hCommandBuffer);
									swapchain_render_pass(vk_hCommandBuffer);
								})) {
							if (aRenderTasks[uCurrentFrameInFlightIndex].record(
									RENDER_TASK_SUBINDEX_IMAGE_BLIT,
									VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
									[&](VkCommandBuffer vk_hCommandBuffer, uint8_t u8PreviousLogicalQueue, uint8_t u8CurrentLogicalQueue, uint8_t u8NextLogicalQueue) {
										PRINT_DEBUG("Recording the image blitting subprocedure into Vulkan command buffer ", vk_hCommandBuffer);
										blit_render_image(vk_hCommandBuffer);
										const VkImageMemoryBarrier vk_swapchainImageBarrier = {
											.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
											.pNext = nullptr,
											.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT,
											.dstAccessMask = VK_ACCESS_NONE,
											.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
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
												vk_hCommandBuffer,
												VK_PIPELINE_STAGE_TRANSFER_BIT,
												VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
												0,
												0,
												nullptr,
												0,
												nullptr,
												1,
												&vk_swapchainImageBarrier);
									})) {
								PRINT_DEBUG("Submitting rendering task at frame-in-flight index ", uCurrentFrameInFlightIndex);
								if (vkResetFences(vk_hDevice, 1, &vk_ahRenderFences[uCurrentFrameInFlightIndex]) == VK_SUCCESS) {
									constexpr VkPipelineStageFlags2 vk_aeInternalWaitStages[] = {
										VK_PIPELINE_STAGE_2_TRANSFER_BIT,
										VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT,
										VK_PIPELINE_STAGE_2_BOTTOM_OF_PIPE_BIT
									};
									const VkSemaphoreSubmitInfo vk_waitSwapchainSemaphoreInfo = {
										.sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO,
										.pNext = nullptr,
										.semaphore = swapchainSemaphores[u32CurrentSwapchainSemaphoreIndex * RE_VK_SEMAPHORES_PER_SWAPCHAIN_IMAGE],
										.stageMask = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
										.deviceIndex = 0
									},
									vk_signalSwapchainSemaphoreInfo = {
										.sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO,
										.pNext = nullptr,
										.semaphore = swapchainSemaphores[u32CurrentSwapchainSemaphoreIndex * RE_VK_SEMAPHORES_PER_SWAPCHAIN_IMAGE + 1],
										.stageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
										.deviceIndex = 0
									};
									if (aRenderTasks[uCurrentFrameInFlightIndex].submit(
											1,
											&vk_waitSwapchainSemaphoreInfo,
											vk_aeInternalWaitStages,
											1,
											&vk_signalSwapchainSemaphoreInfo,
											vk_ahRenderFences[uCurrentFrameInFlightIndex])) {
										present_swapchain_image();
										return true;
									} else
										RE_FATAL_ERROR("Failed submitting a rendering task");
								} else
									RE_FATAL_ERROR("Failed to reset a Vulkan fence used for synchronizing rendering");
							} else
								RE_FATAL_ERROR("Failed to record command buffer of Vulkan task at frame-in-flight index ", uCurrentFrameInFlightIndex, " for image blitting");
						} else
							RE_FATAL_ERROR("Failed to record command buffer of Vulkan task at frame-in-flight index ", uCurrentFrameInFlightIndex, " for rendering");
					} else
						RE_FATAL_ERROR("Failed to record command buffer of Vulkan task at frame-in-flight index ", uCurrentFrameInFlightIndex, " for computing before rendering");
				} else
					RE_FATAL_ERROR("Failed to record command buffer of Vulkan task at frame-in-flight index ", uCurrentFrameInFlightIndex, " for transferring data to the GPU");
				break;
			case VK_TIMEOUT:
				RE_ABORT("Rendering timed out after ", RENDERING_TIMEOUT_SEC, " seconds");
			case VK_ERROR_DEVICE_LOST:
				RE_ABORT("Failed to synchronize with rendering process. Suspecting fatal error (device lost)");
			default:
				RE_FATAL_ERROR("Failed to wait for the Vulkan fence to synchronize rendering");
				break;
		}
		return false;
	}

}
