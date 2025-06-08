#include "RE_Renderer.hpp"
#include "RE_Internal Header.hpp"
#include "RE_Window.hpp"
#include "RE_Vulkan_Wrapper Classes.hpp"
#include "RE_Vulkan_Wrapper Functions.hpp"
#include "RE_Renderer_GameObject.hpp"

#include <thread>

namespace RE {

	typedef uint16_t REindex_t;
#define RE_VK_RECT_INDEX_BUFFER_SIZE (RE_VK_RENDERABLE_RECTANGLES_COUNT * 6U)
#define RE_VK_RECT_INDEX_BUFFER_SIZE_BYTES (RE_VK_RECT_INDEX_BUFFER_SIZE * sizeof(REindex_t))

	Camera *pActiveCamera = nullptr;
	VkViewport vk_cameraViewportArea;
	VkRect2D vk_cameraScissorArea;
	VkExtent2D vk_worldRenderImageExtent = {
		.width = 600,
		.height = 400
	};

	VkBuffer vk_hRectIndexBuffer = VK_NULL_HANDLE;
	VkDeviceMemory vk_hRectIndexBufferMemory = VK_NULL_HANDLE;

	VkRenderPass vk_hWorldRenderPass = VK_NULL_HANDLE;
	VkImage vk_ahWorldRenderImages[RE_VK_FRAMES_IN_FLIGHT] = {};
	VkDeviceMemory vk_ahWorldRenderImageMemories[RE_VK_FRAMES_IN_FLIGHT] = {};
	VkImageView vk_ahWorldRenderImageViews[RE_VK_FRAMES_IN_FLIGHT] = {};
	VkFramebuffer vk_ahWorldFramebuffers[RE_VK_FRAMES_IN_FLIGHT] = {};

	VkFence vk_ahRenderFences[RE_VK_FRAMES_IN_FLIGHT] = {};
	VkSemaphore vk_ahRenderSemaphores[RE_VK_RENDER_SEMAPHORE_COUNT] = {}, *vk_pahPresentSemaphores = nullptr;

	VkCommandBuffer vk_ahRenderCommandBuffers[RE_VK_FRAMES_IN_FLIGHT] = {};

	uint8_t u8CurrentFrameInFlightIndex = 0U;

	bool init_renderer() {
		constexpr uint32_t u32StagingIndexBufferQueueCount = 1U, u32StagingIndexBufferQueues[u32StagingIndexBufferQueueCount] = {RE_VK_QUEUE_TRANSFER_INDEX};
		Vulkan_Buffer stagingIndexBuffer(RE_VK_RECT_INDEX_BUFFER_SIZE_BYTES, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, u32StagingIndexBufferQueueCount, u32StagingIndexBufferQueues, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
		if (stagingIndexBuffer.is_valid()) {
			std::thread stagingIndexBufferDataInit([&]() {
				REindex_t *pu16RectIndices;
				stagingIndexBuffer.map_memory((void**) &pu16RectIndices);
				for (uint16_t u16RectNumber = 0U; u16RectNumber < RE_VK_RENDERABLE_RECTANGLES_COUNT; u16RectNumber++) {
					pu16RectIndices[u16RectNumber * 6U + 0U] = u16RectNumber * 4U + 0U;
					pu16RectIndices[u16RectNumber * 6U + 1U] = u16RectNumber * 4U + 1U;
					pu16RectIndices[u16RectNumber * 6U + 2U] = u16RectNumber * 4U + 2U;
					pu16RectIndices[u16RectNumber * 6U + 3U] = u16RectNumber * 4U + 2U;
					pu16RectIndices[u16RectNumber * 6U + 4U] = u16RectNumber * 4U + 3U;
					pu16RectIndices[u16RectNumber * 6U + 5U] = u16RectNumber * 4U + 0U;
				}
				stagingIndexBuffer.unmap_memory();
			});
			constexpr uint32_t u32IndexBufferQueueCount = 2U, u32IndexBufferQueues[u32IndexBufferQueueCount] = {RE_VK_QUEUE_TRANSFER_INDEX, RE_VK_QUEUE_GRAPHICS_INDEX};
			if (CATCH_SIGNAL_AND_RETURN(create_vulkan_buffer(RE_VK_RECT_INDEX_BUFFER_SIZE_BYTES, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, u32IndexBufferQueueCount, u32IndexBufferQueues, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &vk_hRectIndexBuffer, &vk_hRectIndexBufferMemory), bool)) {
				Vulkan_CommandBuffer indexBufferDataTransferCommandBuffer(vk_hCommandPools[RE_VK_COMMAND_POOL_TRANSFER_TRANSIENT_INDEX], VK_COMMAND_BUFFER_LEVEL_PRIMARY);
				if (indexBufferDataTransferCommandBuffer.is_valid()) {
					if (indexBufferDataTransferCommandBuffer.begin_recording(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT, nullptr)) {
						const VkBufferCopy vk_rectIndexBufferCopy = {
							.size = RE_VK_RECT_INDEX_BUFFER_SIZE_BYTES
						};
						vkCmdCopyBuffer(indexBufferDataTransferCommandBuffer, stagingIndexBuffer, vk_hRectIndexBuffer, 1U, &vk_rectIndexBufferCopy);
						if (indexBufferDataTransferCommandBuffer.end_recording()) {
							Vulkan_Fence rectIndexBufferDataTransferFence;
							if (rectIndexBufferDataTransferFence.is_valid()) {
								const VkSubmitInfo vk_rectIndexBufferTransferSubmitInfo = {
									.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
									.commandBufferCount = 1U,
									.pCommandBuffers = indexBufferDataTransferCommandBuffer.get_command_buffer_ptr()
								};
								stagingIndexBufferDataInit.join();
								if (vkQueueSubmit(vk_hDeviceQueueFamilies[RE_VK_QUEUE_TRANSFER_INDEX], 1U, &vk_rectIndexBufferTransferSubmitInfo, rectIndexBufferDataTransferFence)) {
									constexpr uint32_t u32WorldRenderPassAttachmentCount = 1U, u32WorldRenderPassSubpassCount = 1U, u32WorldRenderPassDependencyCount = 1U;
									const VkAttachmentDescription vk_worldRenderPassAttachments[u32WorldRenderPassAttachmentCount] = {
										{
											.format = vk_eSwapchainImageFormat,
											.samples = VK_SAMPLE_COUNT_1_BIT,
											.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
											.storeOp = VK_ATTACHMENT_STORE_OP_STORE,
											.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
											.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
											.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
											.finalLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL
										}
									};
									const VkAttachmentReference vk_worldRenderPassColorAttachments[] = {
										{
											.attachment = 0U,
											.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
										}
									};
									const VkSubpassDescription vk_worldRenderSubpasses[u32WorldRenderPassSubpassCount] = {
										{
											.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,
											.inputAttachmentCount = 0U,
											.pInputAttachments = nullptr,
											.colorAttachmentCount = 1U,
											.pColorAttachments = &vk_worldRenderPassColorAttachments[0]
										}
									};
									const VkSubpassDependency vk_worldRenderDependencies[u32WorldRenderPassDependencyCount] = {
										{
											.srcSubpass = VK_SUBPASS_EXTERNAL,
											.dstSubpass = RE_VK_GAME_OBJECT_SUPBASS,
											.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
											.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
											.srcAccessMask = VK_ACCESS_NONE,
											.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
											.dependencyFlags = 0
										}
									};
									const VkRenderPassCreateInfo vk_renderPassCreateInfo = {
										.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
										.attachmentCount = u32WorldRenderPassAttachmentCount,
										.pAttachments = vk_worldRenderPassAttachments,
										.subpassCount = u32WorldRenderPassSubpassCount,
										.pSubpasses = vk_worldRenderSubpasses,
										.dependencyCount = u32WorldRenderPassDependencyCount,
										.pDependencies = vk_worldRenderDependencies
									};
									if (vkCreateRenderPass(vk_hDevice, &vk_renderPassCreateInfo, nullptr, &vk_hWorldRenderPass)) {
										constexpr uint32_t u32WorldRenderImageQueueCount = 2U, au32WorldRenderImageQueues[u32WorldRenderImageQueueCount] = {RE_VK_QUEUE_GRAPHICS_INDEX, RE_VK_QUEUE_TRANSFER_INDEX};
										const VkExtent3D vk_worldRenderImageExtent3D = {
											.width = vk_worldRenderImageExtent.width,
											.height = vk_worldRenderImageExtent.height,
											.depth = 1U
										};
										uint8_t u8WorldRenderImageCollectionCreateIndex = 0U;
										while (u8WorldRenderImageCollectionCreateIndex < RE_VK_FRAMES_IN_FLIGHT) {
											if (CATCH_SIGNAL_AND_RETURN(create_vulkan_image(0, VK_IMAGE_TYPE_2D, vk_eSwapchainImageFormat, vk_worldRenderImageExtent3D, 1U, 1U, VK_SAMPLE_COUNT_1_BIT, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, u32WorldRenderImageQueueCount, au32WorldRenderImageQueues, VK_IMAGE_LAYOUT_UNDEFINED, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &vk_ahWorldRenderImages[u8WorldRenderImageCollectionCreateIndex], &vk_ahWorldRenderImageMemories[u8WorldRenderImageCollectionCreateIndex]), bool)) {
												if (CATCH_SIGNAL_AND_RETURN(create_vulkan_image_view(vk_ahWorldRenderImages[u8WorldRenderImageCollectionCreateIndex], VK_IMAGE_VIEW_TYPE_2D, vk_eSwapchainImageFormat, VK_IMAGE_ASPECT_COLOR_BIT, 0U, 1U, 0U, 1U, &vk_ahWorldRenderImageViews[u8WorldRenderImageCollectionCreateIndex]), bool)) {
													if (CATCH_SIGNAL_AND_RETURN(create_vulkan_framebuffer(0, vk_hWorldRenderPass, 1U, &vk_ahWorldRenderImageViews[u8WorldRenderImageCollectionCreateIndex], vk_worldRenderImageExtent.width, vk_worldRenderImageExtent.height, 1U, &vk_ahWorldFramebuffers[u8WorldRenderImageCollectionCreateIndex]), bool)) {
														u8WorldRenderImageCollectionCreateIndex++;
														continue;
													} else
														RE_FATAL_ERROR(append_to_string("Failed creating Vulkan framebuffer at index ", u8WorldRenderImageCollectionCreateIndex, " for world rendering"));
													vkDestroyImageView(vk_hDevice, vk_ahWorldRenderImageViews[u8WorldRenderImageCollectionCreateIndex], nullptr);
													vk_ahWorldRenderImageViews[u8WorldRenderImageCollectionCreateIndex] = VK_NULL_HANDLE;
												} else
													RE_FATAL_ERROR(append_to_string("Failed creating Vulkan GPU-side image view at index ", u8WorldRenderImageCollectionCreateIndex, " for world rendering"));
												vkFreeMemory(vk_hDevice, vk_ahWorldRenderImageMemories[u8WorldRenderImageCollectionCreateIndex], nullptr);
												vk_ahWorldRenderImageMemories[u8WorldRenderImageCollectionCreateIndex] = VK_NULL_HANDLE;
												vkDestroyImage(vk_hDevice, vk_ahWorldRenderImages[u8WorldRenderImageCollectionCreateIndex], nullptr);
												vk_ahWorldRenderImages[u8WorldRenderImageCollectionCreateIndex] = VK_NULL_HANDLE;
											} else
												RE_FATAL_ERROR(append_to_string("Failed creating Vulkan GPU-side image at index ", u8WorldRenderImageCollectionCreateIndex, " for world rendering"));
											break;
										}
										if (u8WorldRenderImageCollectionCreateIndex == RE_VK_FRAMES_IN_FLIGHT) {
											const VkFenceCreateInfo vk_renderFenceCreateInfo = {
												.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
												.flags = VK_FENCE_CREATE_SIGNALED_BIT
											};
											uint8_t u8FrameInFlightCreateIndex = 0U;
											while (u8FrameInFlightCreateIndex < RE_VK_FRAMES_IN_FLIGHT) {
												if (!vkCreateFence(vk_hDevice, &vk_renderFenceCreateInfo, nullptr, &vk_ahRenderFences[u8FrameInFlightCreateIndex])) {
													RE_FATAL_ERROR(append_to_string("Failed to create Vulkan render fence at index ", u8FrameInFlightCreateIndex));
													break;
												}
												u8FrameInFlightCreateIndex++;
											}
											if (u8FrameInFlightCreateIndex == RE_VK_FRAMES_IN_FLIGHT) {
												const VkSemaphoreCreateInfo vk_renderSemaphoreCreateInfo = {
													.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO
												};
												uint16_t u16RenderSemaphoreCreateIndex = 0U;
												while (u16RenderSemaphoreCreateIndex < RE_VK_RENDER_SEMAPHORE_COUNT) {
													if (!vkCreateSemaphore(vk_hDevice, &vk_renderSemaphoreCreateInfo, nullptr, &vk_ahRenderSemaphores[u16RenderSemaphoreCreateIndex])) {
														RE_FATAL_ERROR(append_to_string("Failed creating Vulkan semaphore at index ", u16RenderSemaphoreCreateIndex));
														break;
													}
													u16RenderSemaphoreCreateIndex++;
												}
												if (u16RenderSemaphoreCreateIndex == RE_VK_RENDER_SEMAPHORE_COUNT && CATCH_SIGNAL_AND_RETURN(alloc_vulkan_command_buffers(vk_hCommandPools[RE_VK_COMMAND_POOL_GRAPHICS_PERSISTENT_INDEX], VK_COMMAND_BUFFER_LEVEL_PRIMARY, RE_VK_FRAMES_IN_FLIGHT, vk_ahRenderCommandBuffers), bool)) {
													if (CATCH_SIGNAL_AND_RETURN(swapchain_created_renderer() && init_gameobject_renderer(), bool)) {
														rectIndexBufferDataTransferFence.wait_for();
														return true;
													}
													vkFreeCommandBuffers(vk_hDevice, vk_hCommandPools[RE_VK_COMMAND_POOL_GRAPHICS_PERSISTENT_INDEX], RE_VK_FRAMES_IN_FLIGHT, vk_ahRenderCommandBuffers);
												}
												for (uint16_t u16RenderSemaphoreDeleteIndex = 0U; u16RenderSemaphoreDeleteIndex < u16RenderSemaphoreCreateIndex; u16RenderSemaphoreDeleteIndex++) {
													vkDestroySemaphore(vk_hDevice, vk_ahRenderSemaphores[u16RenderSemaphoreDeleteIndex], nullptr);
													vk_ahRenderSemaphores[u16RenderSemaphoreDeleteIndex] = VK_NULL_HANDLE;
												}
											}
											for (uint8_t u8RenderFenceDeleteIndex = 0U; u8RenderFenceDeleteIndex < u8FrameInFlightCreateIndex; u8RenderFenceDeleteIndex++) {
												vkDestroyFence(vk_hDevice, vk_ahRenderFences[u8RenderFenceDeleteIndex], nullptr);
												vk_ahRenderFences[u8RenderFenceDeleteIndex] = VK_NULL_HANDLE;
											}
										}
										for (uint8_t u8WorldRenderImageCollectionDeleteIndex = 0U; u8WorldRenderImageCollectionDeleteIndex < u8WorldRenderImageCollectionCreateIndex; u8WorldRenderImageCollectionDeleteIndex++) {
											vkDestroyFramebuffer(vk_hDevice, vk_ahWorldFramebuffers[u8WorldRenderImageCollectionDeleteIndex], nullptr);
											vk_ahWorldFramebuffers[u8WorldRenderImageCollectionDeleteIndex] = VK_NULL_HANDLE;
											vkDestroyImageView(vk_hDevice, vk_ahWorldRenderImageViews[u8WorldRenderImageCollectionDeleteIndex], nullptr);
											vk_ahWorldRenderImageViews[u8WorldRenderImageCollectionDeleteIndex] = VK_NULL_HANDLE;
											vkFreeMemory(vk_hDevice, vk_ahWorldRenderImageMemories[u8WorldRenderImageCollectionDeleteIndex], nullptr);
											vk_ahWorldRenderImageMemories[u8WorldRenderImageCollectionDeleteIndex] = VK_NULL_HANDLE;
											vkDestroyImage(vk_hDevice, vk_ahWorldRenderImages[u8WorldRenderImageCollectionDeleteIndex], nullptr);
											vk_ahWorldRenderImages[u8WorldRenderImageCollectionDeleteIndex] = VK_NULL_HANDLE;
										}
										vkDestroyRenderPass(vk_hDevice, vk_hWorldRenderPass, nullptr);
									}
									rectIndexBufferDataTransferFence.wait_for();
								} else
									RE_FATAL_ERROR("Failed to submit task to the transfer queue for copying data to the Vulkan rectangle index buffer on the GPU");
							} else
								RE_FATAL_ERROR("Failed to create Vulkan fence to wait on finishing transfer task for rectangle index buffer data");
						} else
							RE_FATAL_ERROR("Failed to finish recording Vulkan rectangle index buffer");
					} else
						RE_FATAL_ERROR("Failed to begin recording Vulkan rectangle index buffer");
				} else
					RE_FATAL_ERROR("Failed to allocate Vulkan command buffer to transfer data for the rectangle index buffer to the GPU");
				vkFreeMemory(vk_hDevice, vk_hRectIndexBufferMemory, nullptr);
				vk_hRectIndexBufferMemory = VK_NULL_HANDLE;
				vkDestroyBuffer(vk_hDevice, vk_hRectIndexBuffer, nullptr);
				vk_hRectIndexBuffer = VK_NULL_HANDLE;
			} else
				RE_FATAL_ERROR("Failed to create Vulkan rectangle index buffer on the GPU");
			stagingIndexBufferDataInit.join();
		} else
			RE_FATAL_ERROR("Failed to create Vulkan staging rectangle index buffer");
		return false;
	}
	
	void destroy_renderer() {
		CATCH_SIGNAL(destroy_gameobject_renderer());
		vkFreeCommandBuffers(vk_hDevice, vk_hCommandPools[RE_VK_COMMAND_POOL_GRAPHICS_PERSISTENT_INDEX], RE_VK_FRAMES_IN_FLIGHT, vk_ahRenderCommandBuffers);
		for (uint16_t u16RenderSemaphoreDeleteIndex = 0U; u16RenderSemaphoreDeleteIndex < RE_VK_RENDER_SEMAPHORE_COUNT; u16RenderSemaphoreDeleteIndex++) {
			vkDestroySemaphore(vk_hDevice, vk_ahRenderSemaphores[u16RenderSemaphoreDeleteIndex], nullptr);
			vk_ahRenderSemaphores[u16RenderSemaphoreDeleteIndex] = VK_NULL_HANDLE;
		}
		for (uint8_t u8FrameInFlightDeleteIndex = 0U; u8FrameInFlightDeleteIndex < RE_VK_FRAMES_IN_FLIGHT; u8FrameInFlightDeleteIndex++) {
			vkDestroyFence(vk_hDevice, vk_ahRenderFences[u8FrameInFlightDeleteIndex], nullptr);
			vk_ahRenderFences[u8FrameInFlightDeleteIndex] = VK_NULL_HANDLE;
			vkDestroyFramebuffer(vk_hDevice, vk_ahWorldFramebuffers[u8FrameInFlightDeleteIndex], nullptr);
			vk_ahWorldFramebuffers[u8FrameInFlightDeleteIndex] = VK_NULL_HANDLE;
			vkDestroyImageView(vk_hDevice, vk_ahWorldRenderImageViews[u8FrameInFlightDeleteIndex], nullptr);
			vk_ahWorldRenderImageViews[u8FrameInFlightDeleteIndex] = VK_NULL_HANDLE;
			vkFreeMemory(vk_hDevice, vk_ahWorldRenderImageMemories[u8FrameInFlightDeleteIndex], nullptr);
			vk_ahWorldRenderImageMemories[u8FrameInFlightDeleteIndex] = VK_NULL_HANDLE;
			vkDestroyImage(vk_hDevice, vk_ahWorldRenderImages[u8FrameInFlightDeleteIndex], nullptr);
			vk_ahWorldRenderImages[u8FrameInFlightDeleteIndex] = VK_NULL_HANDLE;
		}
		vkDestroyRenderPass(vk_hDevice, vk_hWorldRenderPass, nullptr);
		vkFreeMemory(vk_hDevice, vk_hRectIndexBufferMemory, nullptr);
		vk_hRectIndexBufferMemory = VK_NULL_HANDLE;
		vkDestroyBuffer(vk_hDevice, vk_hRectIndexBuffer, nullptr);
		vk_hRectIndexBuffer = VK_NULL_HANDLE;
	}

	void render() {
		if (pActiveCamera)
			CATCH_SIGNAL(pActiveCamera->update());
		vkWaitForFences(vk_hDevice, 1U, &vk_ahRenderFences[u8CurrentFrameInFlightIndex], VK_TRUE, std::numeric_limits<uint64_t>::max());
		uint32_t u32NextSwapchainImageIndex;
		vkAcquireNextImageKHR(vk_hDevice, vk_hSwapchain, std::numeric_limits<uint64_t>::max(), vk_ahRenderSemaphores[u8CurrentFrameInFlightIndex * RE_VK_SEMAPHORES_PER_FRAME_COUNT], VK_NULL_HANDLE, &u32NextSwapchainImageIndex);
		CATCH_SIGNAL(render_gameobjects());
		if (!begin_recording_vulkan_command_buffer(vk_ahRenderCommandBuffers[u8CurrentFrameInFlightIndex], VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT, nullptr)) {
			RE_FATAL_ERROR("Failed beginning to record Vulkan command buffer for rendering everything");
			return;
		}
		const VkClearValue vk_clearValues[] = {
			{
				.color = {
					.float32 = {
						0.0f,
						0.0f,
						0.0f,
						1.0f
					}
				}
			}
		};
		const VkRenderPassBeginInfo vk_renderPassInfo = {
			.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
			.renderPass = vk_hWorldRenderPass,
			.framebuffer = vk_ahWorldFramebuffers[u8CurrentFrameInFlightIndex],
			.renderArea = VkRect2D{{0, 0}, {8U, 8U}},
			.clearValueCount = 1U,
			.pClearValues = vk_clearValues
		};
		vkCmdBeginRenderPass(vk_ahRenderCommandBuffers[u8CurrentFrameInFlightIndex], &vk_renderPassInfo, VK_SUBPASS_CONTENTS_SECONDARY_COMMAND_BUFFERS);
		vkCmdExecuteCommands(vk_ahRenderCommandBuffers[u8CurrentFrameInFlightIndex], 1U, &vk_ahGameObjectSecondaryCommandBuffers[u8CurrentFrameInFlightIndex]);
		vkCmdEndRenderPass(vk_ahRenderCommandBuffers[u8CurrentFrameInFlightIndex]);
		const VkImageMemoryBarrier vk_swapchainImageLayoutPrepare = {
			.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
			.srcAccessMask = VK_ACCESS_NONE,
			.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT,
			.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED,
			.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
			.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
			.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
			.image = vk_phSwapchainImages[u32NextSwapchainImageIndex],
			.subresourceRange = {
				.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
				.baseMipLevel = 0U,
				.levelCount = 1U,
				.baseArrayLayer = 0U,
				.layerCount = 1U
			}
		};
		vkCmdPipelineBarrier(vk_ahRenderCommandBuffers[u8CurrentFrameInFlightIndex], VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0U, nullptr, 0U, nullptr, 1U, &vk_swapchainImageLayoutPrepare);
		const VkImageBlit vk_worldRenderImageBlit = {
			.srcSubresource = {
				.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
				.mipLevel = 0U,
				.baseArrayLayer = 0U,
				.layerCount = 1U
			},
			.srcOffsets = {
				{
					.x = 0,
					.y = static_cast<int32_t>(vk_worldRenderImageExtent.height),
					.z = 0
				}, {
					.x = static_cast<int32_t>(vk_worldRenderImageExtent.width),
					.y = 0,
					.z = 1
				}
			},
			.dstSubresource = {
				.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
				.mipLevel = 0U,
				.baseArrayLayer = 0U,
				.layerCount = 1U
			},
			.dstOffsets = {
				{
					.x = 0,
					.y = 0,
					.z = 0
				}, {
					.x = static_cast<int32_t>(vk_swapchainResolution.width),
					.y = static_cast<int32_t>(vk_swapchainResolution.height),
					.z = 1
				}
			}
		};
		vkCmdBlitImage(vk_ahRenderCommandBuffers[u8CurrentFrameInFlightIndex], vk_ahWorldRenderImages[u8CurrentFrameInFlightIndex], VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, vk_phSwapchainImages[u32NextSwapchainImageIndex], VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1U, &vk_worldRenderImageBlit, VK_FILTER_LINEAR);
		const VkImageMemoryBarrier vk_swapchainImageLayoutTransfer = {
			.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
			.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT,
			.dstAccessMask = VK_ACCESS_NONE,
			.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
			.newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
			.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
			.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
			.image = vk_phSwapchainImages[u32NextSwapchainImageIndex],
			.subresourceRange = {
				.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
				.baseMipLevel = 0U,
				.levelCount = 1U,
				.baseArrayLayer = 0U,
				.layerCount = 1U
			}
		};
		vkCmdPipelineBarrier(vk_ahRenderCommandBuffers[u8CurrentFrameInFlightIndex], VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, 0, 0U, nullptr, 0U, nullptr, 1U, &vk_swapchainImageLayoutTransfer);
		if (!vkEndCommandBuffer(vk_ahRenderCommandBuffers[u8CurrentFrameInFlightIndex])) {
			RE_FATAL_ERROR("Failed finishing to record Vulkan command buffer for rendering everything");
			return;
		}
		vkResetFences(vk_hDevice, 1U, &vk_ahRenderFences[u8CurrentFrameInFlightIndex]);
		const VkPipelineStageFlags vk_aePipelinesToWaitForBeforeRendering[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT};
		if (!submit_to_vulkan_queue(vk_hDeviceQueueFamilies[RE_VK_QUEUE_GRAPHICS_INDEX], 2U, &vk_ahRenderSemaphores[u8CurrentFrameInFlightIndex * RE_VK_SEMAPHORES_PER_FRAME_COUNT], vk_aePipelinesToWaitForBeforeRendering, 1U, &vk_ahRenderCommandBuffers[u8CurrentFrameInFlightIndex], 1U, &vk_ahRenderSemaphores[u8CurrentFrameInFlightIndex * RE_VK_SEMAPHORES_PER_FRAME_COUNT + 2U], vk_ahRenderFences[u8CurrentFrameInFlightIndex])) {
			RE_FATAL_ERROR("Failed submitting the task for rendering everything to the Vulkan GPU");
			return;
		}
		const VkPresentInfoKHR vk_presentInfo = {
			.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
			.waitSemaphoreCount = 1U,
			.pWaitSemaphores = &vk_ahRenderSemaphores[u8CurrentFrameInFlightIndex * RE_VK_SEMAPHORES_PER_FRAME_COUNT + 2U],
			.swapchainCount = 1U,
			.pSwapchains = &vk_hSwapchain,
			.pImageIndices = &u32NextSwapchainImageIndex
		};
		vkQueuePresentKHR(vk_hDeviceQueueFamilies[RE_VK_QUEUE_PRESENT_INDEX], &vk_presentInfo);
		u8CurrentFrameInFlightIndex = (u8CurrentFrameInFlightIndex + 1U) % RE_VK_FRAMES_IN_FLIGHT;
	}

	bool swapchain_created_renderer() {
		vk_pahPresentSemaphores = new VkSemaphore[u32SwapchainImageCount];
		const VkSemaphoreCreateInfo vk_presentSemaphoreCreateInfo = {
			.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO
		};
		for (uint32_t u32PresentSemaphoreCreateIndex = 0U; u32PresentSemaphoreCreateIndex < u32SwapchainImageCount; u32PresentSemaphoreCreateIndex++)
			if (!vkCreateSemaphore(vk_hDevice, &vk_presentSemaphoreCreateInfo, nullptr, &vk_pahPresentSemaphores[u32PresentSemaphoreCreateIndex])) {
				RE_FATAL_ERROR(append_to_string("Failed creating Vulkan semaphore at index ", u32PresentSemaphoreCreateIndex, " for synchronizing swapchain presentations"));
				for (uint32_t u32PresentSemaphoreDeleteIndex = 0U; u32PresentSemaphoreDeleteIndex < u32PresentSemaphoreCreateIndex; u32PresentSemaphoreDeleteIndex++)
					vkDestroySemaphore(vk_hDevice, vk_pahPresentSemaphores[u32PresentSemaphoreDeleteIndex], nullptr);
				return false;
			}
		return true;
	}

	void swapchain_destroyed_renderer() {
		if (!vk_pahPresentSemaphores)
			return;
		for (uint32_t u32PresentSemaphoreIndex = 0U; u32PresentSemaphoreIndex < u32SwapchainImageCount; u32PresentSemaphoreIndex++)
			vkDestroySemaphore(vk_hDevice, vk_pahPresentSemaphores[u32PresentSemaphoreIndex], nullptr);
		delete[] vk_pahPresentSemaphores;
		vk_pahPresentSemaphores = nullptr;
	}

	void calculate_render_area() {
		if (pActiveCamera) {
			const float fCameraScale = std::min(vk_swapchainResolution.width / pActiveCamera->scale[0], vk_swapchainResolution.height / pActiveCamera->scale[1]);
			vk_cameraViewportArea.width = std::round(pActiveCamera->scale[0] * fCameraScale);
			vk_cameraViewportArea.height = std::round(pActiveCamera->scale[1] * fCameraScale);
			vk_cameraViewportArea.x = std::round((vk_swapchainResolution.width - vk_cameraViewportArea.width) / 2.0f);
			vk_cameraViewportArea.y = std::round((vk_swapchainResolution.height - vk_cameraViewportArea.height) / 2.0f);
		} else {
			vk_cameraViewportArea.width = vk_swapchainResolution.width;
			vk_cameraViewportArea.height = vk_swapchainResolution.height;
			vk_cameraViewportArea.x = 0.0f;
			vk_cameraViewportArea.y = 0.0f;
		}
		vk_cameraScissorArea.extent.width = vk_cameraViewportArea.width;
		vk_cameraScissorArea.extent.height = vk_cameraViewportArea.height;
		vk_cameraScissorArea.offset.x = vk_cameraViewportArea.x;
		vk_cameraScissorArea.offset.y = vk_cameraViewportArea.y;
	}

}
