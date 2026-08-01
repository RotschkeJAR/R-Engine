#include "RE_Renderer_RenderProcedure.hpp"
#include "RE_Window.hpp"

namespace RE {
	
	bool render_procedure() {
		PRINT_DEBUG("Waiting for rendering finishing at fence index ", uCurrentFrameInFlightIndex);
		constexpr uint64_t u64RenderTimeoutSec = 5;
		switch (vkWaitForFences(vk_hDevice, 1, &vk_ahRenderFences[uCurrentFrameInFlightIndex], VK_TRUE, u64RenderTimeoutSec * 1000000000UL)) {
			case VK_SUCCESS:
				if (!acquire_next_swapchain_image())
					return true;
				if (aRenderTasks[uCurrentFrameInFlightIndex].record(
						RENDER_TASK_SUBINDEX_RENDERING,
						VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
						[&](VkCommandBuffer vk_hCommandBuffer, uint8_t u8PreviousLogicalQueue, uint8_t u8CurrentLogicalQueue, uint8_t u8NextLogicalQueue) {
							VkClearValue vk_aClears[] = {
								{
									.color = {
										.float32 = {
											backgroundClearColor[0],
											backgroundClearColor[1],
											backgroundClearColor[2],
											backgroundClearColor[3]
										}
									}
								}, {
									.depthStencil = {
										.depth = 1.0f,
										.stencil = 0
									}
								}
							};
							VkRenderPassBeginInfo vk_renderPassBeginInfo = {
								.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
								.pNext = nullptr,
								.renderPass = vk_hRenderPass,
								.framebuffer = vk_ahFramebuffers[uCurrentFrameInFlightIndex],
								.renderArea = {
									.offset = {
										.x = 0,
										.y = 0
									},
									.extent = {
										.width = renderImageSize[0],
										.height = renderImageSize[1]
									}
								},
								.clearValueCount = sizeof(vk_aClears) / sizeof(vk_aClears[0]),
								.pClearValues = vk_aClears
							};
							const VkSubpassBeginInfo vk_subpassBeginInfo = {
								.sType = VK_STRUCTURE_TYPE_SUBPASS_BEGIN_INFO,
								.pNext = nullptr,
								.contents = VK_SUBPASS_CONTENTS_INLINE
							};
							const VkSubpassEndInfo vk_subpassEndInfo = {
								.sType = VK_STRUCTURE_TYPE_SUBPASS_END_INFO,
								.pNext = nullptr
							};
							vkCmdBeginRenderPass2(vk_hCommandBuffer, &vk_renderPassBeginInfo, &vk_subpassBeginInfo);
							vkCmdEndRenderPass2(vk_hCommandBuffer, &vk_subpassEndInfo);
							vk_renderPassBeginInfo.renderPass = vk_hSwapchainRenderPass;
							vk_renderPassBeginInfo.framebuffer = std_swapchainFramebuffers[u32CurrentSwapchainImageIndex];
							vk_renderPassBeginInfo.renderArea.extent = vk_swapchainResolution;
							vk_aClears[0].color.float32[0] = 0.0f;
							vk_aClears[0].color.float32[1] = 0.0f;
							vk_aClears[0].color.float32[2] = 0.0f;
							vk_aClears[0].color.float32[3] = 1.0f;
							vkCmdBeginRenderPass2(vk_hCommandBuffer, &vk_renderPassBeginInfo, &vk_subpassBeginInfo);
							if (should_render_window_frame())
								render_window_frame(vk_hCommandBuffer);
							vkCmdEndRenderPass2(vk_hCommandBuffer, &vk_subpassEndInfo);
						})) {
					if (aRenderTasks[uCurrentFrameInFlightIndex].record(
							RENDER_TASK_SUBINDEX_IMAGE_BLIT,
							VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
							[&](VkCommandBuffer vk_hCommandBuffer, uint8_t u8PreviousLogicalQueue, uint8_t u8CurrentLogicalQueue, uint8_t u8NextLogicalQueue) {
								if (!RENDER_IMAGE_SIZE_EQUALS_SWAPCHAIN()) {
									VkImageBlit vk_blitInfo;
									vk_blitInfo.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
									vk_blitInfo.srcSubresource.mipLevel = 0;
									vk_blitInfo.srcSubresource.baseArrayLayer = uCurrentFrameInFlightIndex;
									vk_blitInfo.srcSubresource.layerCount = 1;
									vk_blitInfo.srcOffsets[0].x = 0;
									vk_blitInfo.srcOffsets[0].y = 0;
									vk_blitInfo.srcOffsets[0].z = 0;
									vk_blitInfo.srcOffsets[1].x = static_cast<int32_t>(renderImageSize[0]);
									vk_blitInfo.srcOffsets[1].y = static_cast<int32_t>(renderImageSize[1]);
									vk_blitInfo.srcOffsets[1].z = 1;
									vk_blitInfo.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
									vk_blitInfo.dstSubresource.mipLevel = 0;
									vk_blitInfo.dstSubresource.baseArrayLayer = 0;
									vk_blitInfo.dstSubresource.layerCount = 1;
									if (should_render_window_frame() && should_render_window_frame_bar()) {
										if (should_render_window_frame_edges()) {
											vk_blitInfo.dstOffsets[0].x = WINDOW_X_OFFSET;
											vk_blitInfo.dstOffsets[0].y = WINDOW_Y_OFFSET;
											vk_blitInfo.dstOffsets[0].z = 0;
											vk_blitInfo.dstOffsets[1].x = static_cast<int32_t>(vk_swapchainResolution.width - WINDOW_BORDER_TOTAL_SIZE - 1);
											vk_blitInfo.dstOffsets[1].y = static_cast<int32_t>(vk_swapchainResolution.height - WINDOW_BORDER_TOTAL_SIZE - 1);
											vk_blitInfo.dstOffsets[1].z = 1;
										} else {
											vk_blitInfo.dstOffsets[0].x = 0;
											vk_blitInfo.dstOffsets[0].y = WINDOW_BAR_SIZE;
											vk_blitInfo.dstOffsets[0].z = 0;
											vk_blitInfo.dstOffsets[1].x = static_cast<int32_t>(vk_swapchainResolution.width);
											vk_blitInfo.dstOffsets[1].y = static_cast<int32_t>(vk_swapchainResolution.height);
											vk_blitInfo.dstOffsets[1].z = 1;
										}
									} else {
										vk_blitInfo.dstOffsets[0].x = 0;
										vk_blitInfo.dstOffsets[0].y = 0;
										vk_blitInfo.dstOffsets[0].z = 0;
										vk_blitInfo.dstOffsets[1].x = static_cast<int32_t>(vk_swapchainResolution.width);
										vk_blitInfo.dstOffsets[1].y = static_cast<int32_t>(vk_swapchainResolution.height);
										vk_blitInfo.dstOffsets[1].z = 1;
									}
									vkCmdBlitImage(
											vk_hCommandBuffer,
											IS_MSAA_ENABLED() ? vk_hSinglesampledImage : vk_hRenderTargetImage,
											VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
											swapchainImages[u32CurrentSwapchainImageIndex],
											VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
											1,
											&vk_blitInfo,
											vk_eScreenFilter);
								} else if (IS_MSAA_ENABLED()) {
									VkImageResolve vk_resolveInfo;
									vk_resolveInfo.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
									vk_resolveInfo.srcSubresource.mipLevel = 0;
									vk_resolveInfo.srcSubresource.baseArrayLayer = uCurrentFrameInFlightIndex;
									vk_resolveInfo.srcSubresource.layerCount = 1;
									vk_resolveInfo.srcOffset.x = 0;
									vk_resolveInfo.srcOffset.y = 0;
									vk_resolveInfo.srcOffset.z = 0;
									vk_resolveInfo.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
									vk_resolveInfo.dstSubresource.mipLevel = 0;
									vk_resolveInfo.dstSubresource.baseArrayLayer = 0;
									vk_resolveInfo.dstSubresource.layerCount = 1;
									vk_resolveInfo.dstOffset.z = 0;
									vk_resolveInfo.extent.depth = 1;
									if (should_render_window_frame() && should_render_window_frame_bar()) {
										if (should_render_window_frame_edges()) {
											vk_resolveInfo.dstOffset.x = WINDOW_X_OFFSET;
											vk_resolveInfo.dstOffset.y = WINDOW_Y_OFFSET;
											vk_resolveInfo.extent.width = std::clamp<uint32_t>(renderImageSize[0], 1U, vk_swapchainResolution.width - WINDOW_EXTRA_WIDTH - 1U);
											vk_resolveInfo.extent.height = std::clamp<uint32_t>(renderImageSize[1], 1U, vk_swapchainResolution.height - WINDOW_EXTRA_HEIGHT - 1U);
										} else {
											vk_resolveInfo.dstOffset.x = 0;
											vk_resolveInfo.dstOffset.y = WINDOW_BAR_SIZE;
											vk_resolveInfo.extent.width = renderImageSize[0];
											vk_resolveInfo.extent.height = std::clamp<uint32_t>(renderImageSize[1], 1U, vk_swapchainResolution.height - WINDOW_BAR_SIZE - 1U);
										}
									} else {
										vk_resolveInfo.dstOffset.x = 0;
										vk_resolveInfo.dstOffset.y = 0;
										vk_resolveInfo.extent.width = renderImageSize[0];
										vk_resolveInfo.extent.height = renderImageSize[1];
									}
									vkCmdResolveImage(
											vk_hCommandBuffer,
											vk_hRenderTargetImage,
											VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
											swapchainImages[u32CurrentSwapchainImageIndex],
											VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
											1,
											&vk_resolveInfo);
								} else {
									VkImageCopy vk_copyInfo;
									vk_copyInfo.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
									vk_copyInfo.srcSubresource.mipLevel = 0;
									vk_copyInfo.srcSubresource.baseArrayLayer = uCurrentFrameInFlightIndex;
									vk_copyInfo.srcSubresource.layerCount = 1;
									vk_copyInfo.srcOffset.x = 0;
									vk_copyInfo.srcOffset.y = 0;
									vk_copyInfo.srcOffset.z = 0;
									vk_copyInfo.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
									vk_copyInfo.dstSubresource.mipLevel = 0;
									vk_copyInfo.dstSubresource.baseArrayLayer = 0;
									vk_copyInfo.dstSubresource.layerCount = 1;
									vk_copyInfo.dstOffset.z = 0;
									vk_copyInfo.extent.depth = 1;
									if (should_render_window_frame() && should_render_window_frame_bar()) {
										if (should_render_window_frame_edges()) {
											vk_copyInfo.dstOffset.x = WINDOW_X_OFFSET;
											vk_copyInfo.dstOffset.y = WINDOW_Y_OFFSET;
											vk_copyInfo.extent.width = std::clamp<uint32_t>(renderImageSize[0], 1U, vk_swapchainResolution.width - WINDOW_EXTRA_WIDTH - 1U);
											vk_copyInfo.extent.height = std::clamp<uint32_t>(renderImageSize[1], 1U, vk_swapchainResolution.height - WINDOW_EXTRA_HEIGHT - 1U);
										} else {
											vk_copyInfo.dstOffset.x = 0;
											vk_copyInfo.dstOffset.y = WINDOW_BAR_SIZE;
											vk_copyInfo.extent.width = renderImageSize[0];
											vk_copyInfo.extent.height = std::clamp<uint32_t>(renderImageSize[1], 1U, vk_swapchainResolution.height - WINDOW_BAR_SIZE - 1U);
										}
									} else {
										vk_copyInfo.dstOffset.x = 0;
										vk_copyInfo.dstOffset.y = 0;
										vk_copyInfo.extent.width = renderImageSize[0];
										vk_copyInfo.extent.height = renderImageSize[1];
									}
									vkCmdCopyImage(
											vk_hCommandBuffer,
											vk_hRenderTargetImage,
											VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
											swapchainImages[u32CurrentSwapchainImageIndex],
											VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
											1,
											&vk_copyInfo);
								}
								const VkImageMemoryBarrier vk_swapchainImageBarrier = {
									.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
									.pNext = nullptr,
									.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT,
									.dstAccessMask = VK_ACCESS_NONE,
									.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
									.newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
									.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
									.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
									.image = swapchainImages[u32CurrentSwapchainImageIndex],
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
								.deviceIndex = 1
							},
							vk_signalSwapchainSemaphoreInfo = {
								.sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO,
								.pNext = nullptr,
								.semaphore = swapchainSemaphores[u32CurrentSwapchainSemaphoreIndex * RE_VK_SEMAPHORES_PER_SWAPCHAIN_IMAGE + 1],
								.stageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
								.deviceIndex = 1
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
				break;
			case VK_TIMEOUT:
				RE_ABORT("Rendering timed out after ", u64RenderTimeoutSec, " seconds");
			default:
				RE_FATAL_ERROR("Failed to wait for the Vulkan fence to synchronize rendering");
				break;
		}
		return false;
	}

#ifdef RENDERER_INCLUDE_EMPTY_PRESENT
	bool present_empty() {
		if (!acquire_next_swapchain_image())
			return true;
		PRINT_DEBUG("Waiting for Vulkan fence ", vk_hEmptyPresentFence, " to synchronize empty presentation");
		constexpr uint64_t u64EmptyPresentTimeoutSec = 2;
		switch (vkWaitForFences(vk_hDevice, 1, &vk_hEmptyPresentFence, VK_TRUE, u64EmptyPresentTimeoutSec * 1000000000UL)) {
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
							.image = swapchainImages[u32CurrentSwapchainImageIndex],
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
										vk_pahQueues[aRenderTasks[0].logical_queue_index_for_function(RENDER_TASK_SUBINDEX_IMAGE_BLIT)],
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
				RE_ABORT("Empty presentation timed out after ", u64EmptyPresentTimeoutSec, " seconds");
			default:
				RE_FATAL_ERROR("Failed to wait for the signaling of Vulkan fence ", vk_hEmptyPresentFence, " used to synchronize empty presentation");
				break;
		}
		return false;
	}
#endif

}
