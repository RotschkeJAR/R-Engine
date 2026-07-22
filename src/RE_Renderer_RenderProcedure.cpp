#include "RE_Renderer_RenderProcedure.hpp"
#include "RE_Window.hpp"

namespace RE {
	
	bool render_procedure() {
		PRINT_DEBUG("Waiting for rendering finishing at fence index ", u8CurrentFrameInFlightIndex);
		if (vkWaitForFences(vk_hDevice, 1, &vk_ahRenderFences[u8CurrentFrameInFlightIndex], VK_TRUE, std::numeric_limits<uint64_t>::max()) == VK_SUCCESS) {
			if (!acquire_next_swapchain_image())
				return true;
			if (aRenderTasks[u8CurrentFrameInFlightIndex].record(
					RENDER_TASK_SUBINDEX_IMAGE_BLIT,
					VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
					[&](VkCommandBuffer vk_hCommandBuffer, uint8_t u8PreviousLogicalQueue, uint8_t u8CurrentLogicalQueue, uint8_t u8NextLogicalQueue) {
						constexpr VkImageSubresourceRange vk_imageRange = {
							.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
							.baseMipLevel = 0,
							.levelCount = 1,
							.baseArrayLayer = 0,
							.layerCount = 1
						};
					#ifdef RE_OS_LINUX
						const VkImageMemoryBarrier vk_swapchainImage1Info = {
							.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
							.pNext = nullptr,
							.srcAccessMask = VK_ACCESS_NONE,
							.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
							.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED,
							.newLayout = VK_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL,
							.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
							.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
							.image = swapchainImages[u32CurrentSwapchainImageIndex],
							.subresourceRange = vk_imageRange
						};
						vkCmdPipelineBarrier(
								vk_hCommandBuffer,
								VK_PIPELINE_STAGE_NONE,
								VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
								0,
								0,
								nullptr,
								0,
								nullptr,
								1,
								&vk_swapchainImage1Info);
						const VkRenderingAttachmentInfo vk_attachmentInfo = {
							.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO,
							.imageView = swapchainImageViews[u32CurrentSwapchainImageIndex],
							.imageLayout = VK_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL,
							.resolveMode = VK_RESOLVE_MODE_NONE,
							.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
							.storeOp = VK_ATTACHMENT_STORE_OP_STORE,
							.clearValue = {
								.color = {
									.float32 = {
										backgroundClearColor.get_red(),
										backgroundClearColor.get_green(),
										backgroundClearColor.get_blue(),
										1.0f
									}
								}
							}
						};
						const VkRenderingInfo vk_renderingInfo = {
							.sType = VK_STRUCTURE_TYPE_RENDERING_INFO,
							.pNext = nullptr,
							.flags = 0,
							.renderArea = {
								.offset = {},
								.extent = vk_swapchainResolution
							},
							.layerCount = 1,
							.viewMask = 0,
							.colorAttachmentCount = 1,
							.pColorAttachments = &vk_attachmentInfo,
							.pDepthAttachment = nullptr,
							.pStencilAttachment = nullptr
						};
						vkCmdBeginRendering(vk_hCommandBuffer, &vk_renderingInfo);
						if (eLinuxWindowType == LINUX_WINDOW_TYPE_WAYLAND && should_render_window_frame_bar()) {
							vkCmdBindPipeline(
									vk_hCommandBuffer,
									VK_PIPELINE_BIND_POINT_GRAPHICS,
									vk_hGraphicsPipelineWindowFrame);
							const VkViewport vk_viewport = {
								.x = 0.0f,
								.y = 0.0f,
								.width = static_cast<float>(vk_swapchainResolution.width),
								.height = static_cast<float>(vk_swapchainResolution.height),
								.minDepth = 0.0f,
								.maxDepth = 0.0f
							};
							vkCmdSetViewport(vk_hCommandBuffer, 0, 1, &vk_viewport);
							const VkRect2D vk_scissor = {
								.offset = {},
								.extent = vk_swapchainResolution
							};
							vkCmdSetScissor(vk_hCommandBuffer, 0, 1, &vk_scissor);
							const VkDescriptorSet vk_ahDescSets[] = {
								vk_hWindowFrameDescSet,
								vk_hCharacterDescSet
							};
							vkCmdBindDescriptorSets(
									vk_hCommandBuffer,
									VK_PIPELINE_BIND_POINT_GRAPHICS,
									vk_hWindowFramePipelineLayout,
									0,
									sizeof(vk_ahDescSets) / sizeof(vk_ahDescSets[0]),
									vk_ahDescSets,
									0,
									nullptr);
							WindowShaderData windowData;
							windowData.a2u32Size[0] = vk_swapchainResolution.width;
							windowData.a2u32Size[1] = vk_swapchainResolution.height;
							if (should_render_window_frame_edges()) {
								windowData.u32WindowFrameToRender = WINDOW_FRAME_RENDER_MODE_SHADOWS;
								windowData.b32RenderEdges = SHR_TRUE;
								vkCmdPushConstants(
										vk_hCommandBuffer,
										vk_hWindowFramePipelineLayout,
										VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
										0,
										sizeof(WindowShaderData),
										static_cast<const void*>(&windowData));
								vkCmdDraw(vk_hCommandBuffer, 10, 1, 0, 0);
								windowData.u32WindowFrameToRender = WINDOW_FRAME_RENDER_MODE_BAR;
								vkCmdPushConstants(
										vk_hCommandBuffer,
										vk_hWindowFramePipelineLayout,
										VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
										offsetof(WindowShaderData, u32WindowFrameToRender),
										sizeof(WindowShaderData::u32WindowFrameToRender),
										static_cast<const void*>(&windowData.u32WindowFrameToRender));
								vkCmdDraw(vk_hCommandBuffer, 10, 1, 0, 0);
							} else {
								windowData.u32WindowFrameToRender = WINDOW_FRAME_RENDER_MODE_BAR;
								windowData.b32RenderEdges = SHR_FALSE;
								vkCmdPushConstants(
										vk_hCommandBuffer,
										vk_hWindowFramePipelineLayout,
										VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
										0,
										sizeof(WindowShaderData),
										static_cast<const void*>(&windowData));
								vkCmdDraw(vk_hCommandBuffer, 4, 1, 0, 0);
							}
							windowData.u32WindowFrameToRender = WINDOW_FRAME_RENDER_MODE_BUTTONS;
							vkCmdPushConstants(
									vk_hCommandBuffer,
									vk_hWindowFramePipelineLayout,
									VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
									offsetof(WindowShaderData, u32WindowFrameToRender),
									sizeof(WindowShaderData::u32WindowFrameToRender),
									static_cast<const void*>(&windowData.u32WindowFrameToRender));
							vkCmdDraw(vk_hCommandBuffer, 4, 6, 0, 0);
							windowData.u32WindowFrameToRender = WINDOW_FRAME_RENDER_MODE_TITLE;
							vkCmdPushConstants(
									vk_hCommandBuffer,
									vk_hWindowFramePipelineLayout,
									VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
									offsetof(WindowShaderData, u32WindowFrameToRender),
									sizeof(WindowShaderData::u32WindowFrameToRender),
									static_cast<const void*>(&windowData.u32WindowFrameToRender));
							vkCmdDrawIndirect(
									vk_hCommandBuffer,
									vk_hWindowFrameBuffer,
									sizeof(WindowFrameUniformData),
									1,
									sizeof(VkDrawIndirectCommand));
						}
						vkCmdEndRendering(vk_hCommandBuffer);
						const VkImageMemoryBarrier vk_swapchainImage2Info = {
							.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
							.pNext = nullptr,
							.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
							.dstAccessMask = VK_ACCESS_MEMORY_READ_BIT,
							.oldLayout = VK_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL,
							.newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
							.srcQueueFamilyIndex = queueFamilyIndices[u8CurrentLogicalQueue],
							.dstQueueFamilyIndex = queueFamilyIndices[u8NextLogicalQueue],
							.image = swapchainImages[u32CurrentSwapchainImageIndex],
							.subresourceRange = vk_imageRange
						};
						vkCmdPipelineBarrier(
								vk_hCommandBuffer,
								VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
								VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
								0,
								0,
								nullptr,
								0,
								nullptr,
								1,
								&vk_swapchainImage2Info);
					#else
						const VkImageMemoryBarrier vk_swapchainImage1Info = {
							.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
							.pNext = nullptr,
							.srcAccessMask = VK_ACCESS_NONE,
							.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT,
							.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED,
							.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
							.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
							.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
							.image = swapchainImages[u32CurrentSwapchainImageIndex],
							.subresourceRange = vk_imageRange
						};
						vkCmdPipelineBarrier(
								vk_hCommandBuffer,
								VK_PIPELINE_STAGE_NONE,
								VK_PIPELINE_STAGE_TRANSFER_BIT,
								0,
								0,
								nullptr,
								0,
								nullptr,
								1,
								&vk_swapchainImage1Info);
						const VkClearColorValue vk_clearColor = {
							.float32 = {
								backgroundClearColor.get_red(),
								backgroundClearColor.get_green(),
								backgroundClearColor.get_blue(),
								1.0f
							}
						};
						vkCmdClearColorImage(
								vk_hCommandBuffer,
								swapchainImages[u32CurrentSwapchainImageIndex],
								VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
								&vk_clearColor,
								1,
								&vk_imageRange);
						const VkImageMemoryBarrier vk_swapchainImage2Info = {
							.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
							.pNext = nullptr,
							.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT,
							.dstAccessMask = VK_ACCESS_MEMORY_READ_BIT,
							.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
							.newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
							.srcQueueFamilyIndex = queueFamilyIndices[u8CurrentLogicalQueue],
							.dstQueueFamilyIndex = queueFamilyIndices[u8NextLogicalQueue],
							.image = swapchainImages[u32CurrentSwapchainImageIndex],
							.subresourceRange = vk_imageRange
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
								&vk_swapchainImage2Info);
					#endif
					})) {
				PRINT_DEBUG("Submitting rendering task at frame-in-flight index ", u8CurrentFrameInFlightIndex);
				if (vkResetFences(vk_hDevice, 1, &vk_ahRenderFences[u8CurrentFrameInFlightIndex]) == VK_SUCCESS) {
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
					if (aRenderTasks[u8CurrentFrameInFlightIndex].submit(
							1,
							&vk_waitSwapchainSemaphoreInfo,
							vk_aeInternalWaitStages,
							1,
							&vk_signalSwapchainSemaphoreInfo,
							vk_ahRenderFences[u8CurrentFrameInFlightIndex])) {
						present_swapchain_image();
						return true;
					} else
						RE_FATAL_ERROR("Failed submitting a rendering task");
				} else
					RE_FATAL_ERROR("Failed to reset a Vulkan fence used for synchronizing rendering");
			}
		}
		return false;
	}

	bool present_empty() {
		if (!acquire_next_swapchain_image())
			return true;
		PRINT_DEBUG("Waiting for Vulkan fence ", vk_hEmptyPresentFence, " to synchronize empty presentation");
		if (vkWaitForFences(
				vk_hDevice,
				1,
				&vk_hEmptyPresentFence,
				VK_TRUE,
				std::numeric_limits<uint64_t>::max()) == VK_SUCCESS) {
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
		} else
			RE_FATAL_ERROR("Failed to wait for the signaling of Vulkan fence ", vk_hEmptyPresentFence, " used to synchronize empty presentation");
		return false;
	}

}
