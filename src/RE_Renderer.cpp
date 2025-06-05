#include "RE_Renderer.hpp"
#include "RE_Internal Header.hpp"
#include "RE_Window.hpp"
#include "RE_Vulkan_Wrapper Classes.hpp"
#include "RE_Vulkan_Wrapper Functions.hpp"

#include <thread>

namespace RE {

	typedef uint16_t REindex_t;
#define RE_VK_INDEX_BUFFER_SIZE RE_VK_RENDERABLE_RECTANGLES_COUNT * 6U
#define RE_VK_INDEX_BUFFER_BYTES RE_VK_INDEX_BUFFER_SIZE * sizeof(REindex_t)

	Camera *pActiveCamera = nullptr;
	VkViewport vk_cameraViewportArea;
	VkRect2D vk_cameraScissorArea;

	VkBuffer vk_hRectIndexBuffer = VK_NULL_HANDLE;
	VkDeviceMemory vk_hRectIndexBufferMemory = VK_NULL_HANDLE;

	VkRenderPass vk_hWorldRenderPass = VK_NULL_HANDLE;
	VkImage vk_ahWorldRenderImages[RE_VK_FRAMES_IN_FLIGHT] = {};
	VkDeviceMemory vk_ahWorldRenderImageMemories[RE_VK_FRAMES_IN_FLIGHT] = {};
	VkImageView vk_ahWorldRenderImageViews[RE_VK_FRAMES_IN_FLIGHT] = {};
	VkFramebuffer vk_ahWorldFramebuffers[RE_VK_FRAMES_IN_FLIGHT] = {};

	VkFence vk_ahRenderFence[RE_VK_FRAMES_IN_FLIGHT] = {};

	bool init_renderer() {
		constexpr uint32_t u32StagingIndexBufferQueueCount = 1U, u32StagingIndexBufferQueues[u32StagingIndexBufferQueueCount] = {RE_VK_QUEUE_TRANSFER_INDEX};
		Vulkan_Buffer stagingIndexBuffer(RE_VK_INDEX_BUFFER_BYTES, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, u32StagingIndexBufferQueueCount, u32StagingIndexBufferQueues, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
		if (stagingIndexBuffer.is_valid()) {
			std::thread stagingIndexBufferDataInit([&]() {
				uint16_t *pu16RectIndices;
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
			if (create_vulkan_buffer(RE_VK_INDEX_BUFFER_BYTES, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, u32IndexBufferQueueCount, u32IndexBufferQueues, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &vk_hRectIndexBuffer, &vk_hRectIndexBufferMemory)) {
				Vulkan_CommandBuffer indexBufferDataTransferCommandBuffer(vk_hCommandPools[RE_VK_COMMAND_POOL_TRANSFER_TRANSIENT_INDEX], VK_COMMAND_BUFFER_LEVEL_PRIMARY);
				if (indexBufferDataTransferCommandBuffer.is_valid()) {
					if (indexBufferDataTransferCommandBuffer.begin_recording(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT, nullptr)) {
						const VkBufferCopy vk_rectIndexBufferCopy = {
							.size = RE_VK_INDEX_BUFFER_BYTES
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
											.dstSubpass = 0,
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
										const VkExtent3D vk_worldRenderImageExtent = {
											.width = vk_swapchainResolution.width,
											.height = vk_swapchainResolution.height,
											.depth = 1U
										};
										uint8_t u8WorldRenderImageCollectionCreateIndex = 0U;
										while (u8WorldRenderImageCollectionCreateIndex < RE_VK_FRAMES_IN_FLIGHT) {
											if (create_vulkan_image(0, VK_IMAGE_TYPE_2D, vk_eSwapchainImageFormat, vk_worldRenderImageExtent, 1U, 1U, VK_SAMPLE_COUNT_1_BIT, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, u32WorldRenderImageQueueCount, au32WorldRenderImageQueues, VK_IMAGE_LAYOUT_UNDEFINED, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &vk_ahWorldRenderImages[u8WorldRenderImageCollectionCreateIndex], &vk_ahWorldRenderImageMemories[u8WorldRenderImageCollectionCreateIndex])) {
												if (create_vulkan_image_view(vk_ahWorldRenderImages[u8WorldRenderImageCollectionCreateIndex], VK_IMAGE_VIEW_TYPE_2D, vk_eSwapchainImageFormat, VK_IMAGE_ASPECT_COLOR_BIT, 0U, 1U, 0U, 1U, &vk_ahWorldRenderImageViews[u8WorldRenderImageCollectionCreateIndex])) {
													if (create_vulkan_framebuffer(0, vk_hWorldRenderPass, 1U, &vk_ahWorldRenderImageViews[u8WorldRenderImageCollectionCreateIndex], vk_worldRenderImageExtent.width, vk_worldRenderImageExtent.height, 1U, &vk_ahWorldFramebuffers[u8WorldRenderImageCollectionCreateIndex])) {
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
											rectIndexBufferDataTransferFence.wait_for();
											return true;
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
									RE_FATAL_ERROR("Failed to submit task to the transfer queue for copying data to the rectangle index buffer on the GPU");
							} else
								RE_FATAL_ERROR("Failed to create fence to wait on finishing tranfer task for rectangle index buffer");
						} else
							RE_FATAL_ERROR("Failed to finish recording rectangle index buffer");
					} else
						RE_FATAL_ERROR("Failed to begin recording rectangle index buffer");
				} else
					RE_FATAL_ERROR("Failed to allocate command buffer to tranfer data for the rectangle index buffer to the GPU");
				vkFreeMemory(vk_hDevice, vk_hRectIndexBufferMemory, nullptr);
				vk_hRectIndexBufferMemory = VK_NULL_HANDLE;
				vkDestroyBuffer(vk_hDevice, vk_hRectIndexBuffer, nullptr);
				vk_hRectIndexBuffer = VK_NULL_HANDLE;
			} else
				RE_FATAL_ERROR("Failed to create rectangle index buffer on the GPU");
			stagingIndexBufferDataInit.join();
		} else
			RE_FATAL_ERROR("Failed to create staging rectangle index buffer");
		return false;
	}
	
	void destroy_renderer() {
		for (uint8_t u8WorldRenderImageCollectionDeleteIndex = 0U; u8WorldRenderImageCollectionDeleteIndex < RE_VK_FRAMES_IN_FLIGHT; u8WorldRenderImageCollectionDeleteIndex++) {
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
		vkFreeMemory(vk_hDevice, vk_hRectIndexBufferMemory, nullptr);
		vk_hRectIndexBufferMemory = VK_NULL_HANDLE;
		vkDestroyBuffer(vk_hDevice, vk_hRectIndexBuffer, nullptr);
		vk_hRectIndexBuffer = VK_NULL_HANDLE;
	}

	void render() {
		if (pActiveCamera)
			CATCH_SIGNAL(pActiveCamera->update());
		// TODO: Implement render process
	}

	bool swapchain_created_renderer() {
		return true;
	}

	void swapchain_destroyed_renderer() {
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
