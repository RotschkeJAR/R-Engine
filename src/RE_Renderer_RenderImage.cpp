#include "RE_Renderer_Internal.hpp"
#include "RE_Main.hpp"

namespace RE {

	ScreenPercentageSettings screenPercentageSettings;
	VkImage vk_hRenderImages;
	static VkDeviceMemory vk_hRenderImageMemories;
	VkImageView vk_ahRenderImageViews[RE_VK_FRAMES_IN_FLIGHT];
	VkExtent2D vk_renderImageSize;
	
	static bool create_render_images(const std::vector<uint32_t> &rRenderQueuesFamilyIndices, const bool bResolvingRequired, const bool bBlittingRequired) {
		if (!bResolvingRequired && !bBlittingRequired)
			return true;
		PRINT_DEBUG("Creating Vulkan image array as target for rendering");
		if (create_vulkan_image(
				0,
				VK_IMAGE_TYPE_2D,
				vk_eSwapchainImageFormat,
				VkExtent3D{vk_renderImageSize.width, vk_renderImageSize.height, 1},
				1,
				RE_VK_FRAMES_IN_FLIGHT,
				vk_eMsaaCount,
				VK_IMAGE_TILING_OPTIMAL,
				VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT,
				rRenderQueuesFamilyIndices.size(),
				rRenderQueuesFamilyIndices.data(),
				VK_IMAGE_LAYOUT_UNDEFINED,
				VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
				&vk_hRenderImages,
				&vk_hRenderImageMemories)) {
			size_t renderImageCreateIndex = 0;
			while (renderImageCreateIndex < RE_VK_FRAMES_IN_FLIGHT) {
				PRINT_DEBUG("Creating Vulkan image view at index ", renderImageCreateIndex, " for render image array");
				if (create_vulkan_image_view(
						vk_hRenderImages,
						VK_IMAGE_VIEW_TYPE_2D,
						vk_eSwapchainImageFormat,
						VK_IMAGE_ASPECT_COLOR_BIT,
						0,
						1,
						renderImageCreateIndex,
						1,
						&vk_ahRenderImageViews[renderImageCreateIndex])) {
					renderImageCreateIndex++;
					continue;
				} else
					RE_FATAL_ERROR("Failed creating Vulkan image view for render image at index ", renderImageCreateIndex);
				break;
			}
			if (renderImageCreateIndex == RE_VK_FRAMES_IN_FLIGHT)
				return true;
			for (size_t renderImageDestroyIndex = 0; renderImageDestroyIndex < renderImageCreateIndex; renderImageDestroyIndex++) {
				PRINT_DEBUG("Destroying Vulkan image view at index ", renderImageDestroyIndex, " due to the failure creating all image views pointing at the render image array");
				vkDestroyImageView(vk_hDevice, vk_ahRenderImageViews[renderImageDestroyIndex], nullptr);
			}
			PRINT_DEBUG("Destroying Vulkan image ", vk_hRenderImages, "and its memory ", vk_hRenderImageMemories, " due to failure creating its image views");
			vkFreeMemory(vk_hDevice, vk_hRenderImageMemories, nullptr);
			vkDestroyImage(vk_hDevice, vk_hRenderImages, nullptr);
		} else
			RE_FATAL_ERROR("Failed creating render image");
		return false;
	}

	static void destroy_render_images() {
		if (screenPercentageSettings.eMode == RE_SCREEN_PERCENTAGE_MODE_NORMAL && vk_eMsaaCount == VK_SAMPLE_COUNT_1_BIT)
			return;
		for (size_t renderImageDestroyIndex = 0; renderImageDestroyIndex < RE_VK_FRAMES_IN_FLIGHT; renderImageDestroyIndex++) {
			PRINT_DEBUG("Destroying Vulkan image view at index ", renderImageDestroyIndex, " pointing at render image array");
			vkDestroyImageView(vk_hDevice, vk_ahRenderImageViews[renderImageDestroyIndex], nullptr);
		}
		PRINT_DEBUG("Destroying Vulkan image ", vk_hRenderImages, " and its memory ", vk_hRenderImageMemories);
		vkFreeMemory(vk_hDevice, vk_hRenderImageMemories, nullptr);
		vkDestroyImage(vk_hDevice, vk_hRenderImages, nullptr);
	}

	void get_queues_for_render_images(std::vector<uint32_t> &rRenderTaskQueueIndices) {
		constexpr uint32_t au32FunctionsToLookup[] = {
			RENDER_TASK_SUBINDEX_RENDERING,
			RENDER_TASK_SUBINDEX_IMAGE_BLIT
		};
		constexpr uint32_t u32FunctionsToLookupCount = sizeof(au32FunctionsToLookup) / sizeof(au32FunctionsToLookup[0]);
		rRenderTaskQueueIndices.reserve(u32FunctionsToLookupCount);
		for (uint32_t u32FunctionToLookupIndex = 0; u32FunctionToLookupIndex < u32FunctionsToLookupCount; u32FunctionToLookupIndex++) {
			const uint32_t u32QueueFamilyIndex = queueFamilyIndices[renderTasks[0].get_logical_queue_index_for_function(au32FunctionsToLookup[u32FunctionToLookupIndex])];
			if (std::find(rRenderTaskQueueIndices.begin(), rRenderTaskQueueIndices.end(), u32QueueFamilyIndex) == rRenderTaskQueueIndices.end())
				rRenderTaskQueueIndices.push_back(u32QueueFamilyIndex);
		}
	}

	bool create_render_image_resources() {
		PRINT_DEBUG("Calculating size of image being rendered to");
		switch (screenPercentageSettings.eMode) {
			case RE_SCREEN_PERCENTAGE_MODE_NORMAL:
				vk_renderImageSize.width = vk_swapchainResolution.width;
				vk_renderImageSize.height = vk_swapchainResolution.height;
				break;
			case RE_SCREEN_PERCENTAGE_MODE_SCALED:
				vk_renderImageSize.width = static_cast<uint32_t>(std::round(vk_swapchainResolution.width * screenPercentageSettings.fScale));
				vk_renderImageSize.height = static_cast<uint32_t>(std::round(vk_swapchainResolution.height * screenPercentageSettings.fScale));
				break;
			case RE_SCREEN_PERCENTAGE_MODE_CONST_SIZE:
				vk_renderImageSize.width = screenPercentageSettings.constSize[0];
				vk_renderImageSize.height = screenPercentageSettings.constSize[1];
				break;
		}
		const bool bBlittingRequired = screenPercentageSettings.eMode != RE_SCREEN_PERCENTAGE_MODE_NORMAL, bResolvingRequired = vk_eMsaaCount != VK_SAMPLE_COUNT_1_BIT;
		VulkanTask depthImageLayoutTransitionTask;
		Vulkan_Fence depthStencilImageLayoutTransitionFence;
		if (create_depth_stencil_images(depthImageLayoutTransitionTask, depthStencilImageLayoutTransitionFence.get_fence())) {
			std::vector<uint32_t> renderTaskQueueIndices;
			get_queues_for_render_images(renderTaskQueueIndices);
			if (create_singlesampled_images(renderTaskQueueIndices, bResolvingRequired, bBlittingRequired)) {
				if (create_render_images(renderTaskQueueIndices, bResolvingRequired, bBlittingRequired)) {
					PRINT_DEBUG("Waiting for Vulkan fence signaling completion of image layout transfer of all depth images");
					depthStencilImageLayoutTransitionFence.wait_for();
					return true;
				}
				destroy_singlesampled_images();
			}
			PRINT_DEBUG("Waiting for Vulkan fence signaling completion of image layout transfer of all depth images due to failure initializing all swapchain-related Vulkan objects");
			depthStencilImageLayoutTransitionFence.wait_for();
			destroy_depth_stencil_images();
		}
		return false;
	}

	void destroy_render_image_resources() {
		PRINT_DEBUG("Destroying render image resources");
		destroy_render_images();
		if (screenPercentageSettings.eMode != RE_SCREEN_PERCENTAGE_MODE_NORMAL && vk_eMsaaCount != VK_SAMPLE_COUNT_1_BIT)
			destroy_singlesampled_images();
		destroy_depth_stencil_images();
	}

	bool record_cmd_blitting_render_image() {
		if (renderTasks[u8CurrentFrameInFlightIndex].record(RENDER_TASK_SUBINDEX_IMAGE_BLIT, VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT, [&](const VkCommandBuffer vk_hCommandBuffer, const uint8_t u8PreviousLogicalQueue, const uint8_t u8CurrentLogicalQueue, const uint8_t u8NextLogicalQueue) {
				VkImageMemoryBarrier2 vk_aImageLayoutTransferRegion[2];
				vk_aImageLayoutTransferRegion[1].sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2;
				vk_aImageLayoutTransferRegion[1].pNext = nullptr;
				vk_aImageLayoutTransferRegion[1].srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
				vk_aImageLayoutTransferRegion[1].dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
				vk_aImageLayoutTransferRegion[1].image = vk_pahSwapchainImages[u32SwapchainImageIndex];
				vk_aImageLayoutTransferRegion[1].subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
				vk_aImageLayoutTransferRegion[1].subresourceRange.baseMipLevel = 0;
				vk_aImageLayoutTransferRegion[1].subresourceRange.levelCount = 1;
				vk_aImageLayoutTransferRegion[1].subresourceRange.baseArrayLayer = 0;
				vk_aImageLayoutTransferRegion[1].subresourceRange.layerCount = 1;
				VkDependencyInfo vk_imageLayoutTransferInfo;
				vk_imageLayoutTransferInfo.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO;
				vk_imageLayoutTransferInfo.pNext = nullptr;
				vk_imageLayoutTransferInfo.dependencyFlags = 0;
				vk_imageLayoutTransferInfo.memoryBarrierCount = 0;
				vk_imageLayoutTransferInfo.pMemoryBarriers = nullptr;
				vk_imageLayoutTransferInfo.bufferMemoryBarrierCount = 0;
				vk_imageLayoutTransferInfo.pBufferMemoryBarriers = nullptr;
				if (screenPercentageSettings.eMode != RE_SCREEN_PERCENTAGE_MODE_NORMAL) {
					PRINT_DEBUG("Transferring render image's layout from optimal for rendering to transfer source and swapchain image's to transfer destination");
					vk_aImageLayoutTransferRegion[0].sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2;
					vk_aImageLayoutTransferRegion[0].pNext = nullptr;
					vk_aImageLayoutTransferRegion[0].srcStageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT;
					vk_aImageLayoutTransferRegion[0].srcAccessMask = VK_ACCESS_2_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT;
					vk_aImageLayoutTransferRegion[0].dstStageMask = VK_PIPELINE_STAGE_2_TRANSFER_BIT;
					vk_aImageLayoutTransferRegion[0].dstAccessMask = VK_ACCESS_2_TRANSFER_READ_BIT;
					vk_aImageLayoutTransferRegion[0].oldLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
					vk_aImageLayoutTransferRegion[0].newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
					vk_aImageLayoutTransferRegion[0].srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
					vk_aImageLayoutTransferRegion[0].dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
					vk_aImageLayoutTransferRegion[0].image = vk_eMsaaCount != VK_SAMPLE_COUNT_1_BIT ? vk_hSingleSampledWorldRenderImages : vk_hRenderImages;
					vk_aImageLayoutTransferRegion[0].subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
					vk_aImageLayoutTransferRegion[0].subresourceRange.baseMipLevel = 0;
					vk_aImageLayoutTransferRegion[0].subresourceRange.levelCount = 1;
					vk_aImageLayoutTransferRegion[0].subresourceRange.baseArrayLayer = u8CurrentFrameInFlightIndex;
					vk_aImageLayoutTransferRegion[0].subresourceRange.layerCount = 1;
					vk_aImageLayoutTransferRegion[1].srcStageMask = VK_PIPELINE_STAGE_2_NONE;
					vk_aImageLayoutTransferRegion[1].srcAccessMask = VK_ACCESS_2_NONE;
					vk_aImageLayoutTransferRegion[1].dstStageMask = VK_PIPELINE_STAGE_2_TRANSFER_BIT;
					vk_aImageLayoutTransferRegion[1].dstAccessMask = VK_ACCESS_2_TRANSFER_WRITE_BIT;
					vk_aImageLayoutTransferRegion[1].oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
					vk_aImageLayoutTransferRegion[1].newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
					vk_imageLayoutTransferInfo.imageMemoryBarrierCount = 2;
					vk_imageLayoutTransferInfo.pImageMemoryBarriers = vk_aImageLayoutTransferRegion;
					vkCmdPipelineBarrier2(vk_hCommandBuffer, &vk_imageLayoutTransferInfo);
					PRINT_DEBUG("Blitting rendered image");
					const VkImageBlit2 vk_blitRegion = {
						.sType = VK_STRUCTURE_TYPE_IMAGE_BLIT_2,
						.srcSubresource = {
							.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
							.mipLevel = 0,
							.baseArrayLayer = u8CurrentFrameInFlightIndex,
							.layerCount = 1
						},
						.srcOffsets = {
							{}, 
							{
								.x = static_cast<int32_t>(vk_renderImageSize.width),
								.y = static_cast<int32_t>(vk_renderImageSize.height),
								.z = 1
							}
						},
						.dstSubresource = {
							.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
							.mipLevel = 0,
							.baseArrayLayer = 0,
							.layerCount = 1
						},
						.dstOffsets = {
							{},
							{
								.x = static_cast<int32_t>(vk_swapchainResolution.width),
								.y = static_cast<int32_t>(vk_swapchainResolution.height),
								.z = 1
							}
						}
					};
					VkBlitImageInfo2 vk_blitInfo = {
						.sType = VK_STRUCTURE_TYPE_BLIT_IMAGE_INFO_2,
						.srcImage = vk_aImageLayoutTransferRegion[0].image,
						.srcImageLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
						.dstImage = vk_pahSwapchainImages[u32SwapchainImageIndex],
						.dstImageLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
						.regionCount = 1,
						.pRegions = &vk_blitRegion
					};
					switch (screenPercentageSettings.eScalingFilter) {
						case RE_TEXTURE_FILTER_NEAREST:
							vk_blitInfo.filter = VK_FILTER_NEAREST;
							break;
						case RE_TEXTURE_FILTER_LINEAR:
							vk_blitInfo.filter = VK_FILTER_LINEAR;
							break;
					}
					vkCmdBlitImage2(vk_hCommandBuffer, &vk_blitInfo);
					vk_aImageLayoutTransferRegion[1].srcStageMask = VK_PIPELINE_STAGE_2_TRANSFER_BIT;
					vk_aImageLayoutTransferRegion[1].srcAccessMask = VK_ACCESS_2_TRANSFER_WRITE_BIT;
					vk_aImageLayoutTransferRegion[1].oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
				} else {
					vk_aImageLayoutTransferRegion[1].srcStageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT;
					vk_aImageLayoutTransferRegion[1].srcAccessMask = VK_ACCESS_2_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT;
					vk_aImageLayoutTransferRegion[1].oldLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
				}
				PRINT_DEBUG("Transferring swapchain image's layout to presentable source");
				vk_aImageLayoutTransferRegion[1].dstStageMask = VK_PIPELINE_STAGE_2_NONE;
				vk_aImageLayoutTransferRegion[1].dstAccessMask = VK_ACCESS_2_NONE;
				vk_aImageLayoutTransferRegion[1].newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
				vk_imageLayoutTransferInfo.imageMemoryBarrierCount = 1;
				vk_imageLayoutTransferInfo.pImageMemoryBarriers = &vk_aImageLayoutTransferRegion[1];
				vkCmdPipelineBarrier2(vk_hCommandBuffer, &vk_imageLayoutTransferInfo);
		})) {
			return true;
		} else
			RE_FATAL_ERROR("Failed recording command buffer for transfering rendered image to swapchain image");
		return false;
	}

	void set_screen_percentage_settings(const ScreenPercentageSettings &rNewSettings) {
		bool bNormalInstead = false;
		if (screenPercentageSettings.eMode == rNewSettings.eMode)
			switch (rNewSettings.eMode) {
				case RE_SCREEN_PERCENTAGE_MODE_NORMAL:
					return;
				case RE_SCREEN_PERCENTAGE_MODE_SCALED:
					if (rNewSettings.fScale == 1.0f) {
						RE_WARNING("Screen percentage had to be changed to 100%, however this has the same effect like the normal mode. Therefore the normal mode will be enabled instead");
						bNormalInstead = true;
					} else if (screenPercentageSettings.fScale == rNewSettings.fScale && screenPercentageSettings.eScalingFilter == rNewSettings.eScalingFilter)
						return;
					break;
				case RE_SCREEN_PERCENTAGE_MODE_CONST_SIZE:
					if (screenPercentageSettings.constSize == rNewSettings.constSize && screenPercentageSettings.eScalingFilter == rNewSettings.eScalingFilter)
						return;
					break;
			}
		PRINT_DEBUG("Updating screen percentage settings");
		screenPercentageSettings = rNewSettings;
		if (bNormalInstead)
			screenPercentageSettings.eMode = RE_SCREEN_PERCENTAGE_MODE_NORMAL;
		if (!bSwapchainDirty && bRunning) {
			PRINT_DEBUG("Immediatly applying new screen percentage settings");
			WAIT_FOR_IDLE_VULKAN_DEVICE();
			destroy_render_image_resources();
			create_render_image_resources();
		}
	}

	[[nodiscard]]
	ScreenPercentageSettings get_screen_percentage_settings() {
		return screenPercentageSettings;
	}

}
