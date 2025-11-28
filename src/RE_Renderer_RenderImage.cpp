#include "RE_Renderer_Internal.hpp"
#include "RE_Main.hpp"

namespace RE {

	ScreenPercentageSettings screenPercentageSettings;
	VkImage vk_hRenderImages;
	VkDeviceMemory vk_hRenderImageMemories;
	VkImageView vk_ahRenderImageViews[RE_VK_FRAMES_IN_FLIGHT];
	VkExtent2D vk_renderImageSize;
	static VkImageResolve2 vk_resolveRegion;
	static VkResolveImageInfo2 vk_resolveInfo;
	static VkImageBlit2 vk_blitRegion;
	static VkBlitImageInfo2 vk_blitInfo;
	
	static bool create_render_images(const bool bResolvingRequired, const bool bBlittingRequired) {
		if (!bResolvingRequired && !bBlittingRequired)
			return true;
		PRINT_DEBUG("Creating Vulkan image array as target for rendering");
		if (create_vulkan_image(0, VK_IMAGE_TYPE_2D, vk_eSwapchainImageFormat, VkExtent3D{vk_renderImageSize.width, vk_renderImageSize.height, 1}, 1, RE_VK_FRAMES_IN_FLIGHT, vk_eMsaaCount, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT, 1, nullptr, VK_IMAGE_LAYOUT_UNDEFINED, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &vk_hRenderImages, &vk_hRenderImageMemories)) {
			size_t renderImageCreateIndex = 0;
			while (renderImageCreateIndex < RE_VK_FRAMES_IN_FLIGHT) {
				PRINT_DEBUG("Creating Vulkan image view at index ", renderImageCreateIndex, " for render image array");
				if (create_vulkan_image_view(vk_hRenderImages, VK_IMAGE_VIEW_TYPE_2D, vk_eSwapchainImageFormat, VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, renderImageCreateIndex, 1, &vk_ahRenderImageViews[renderImageCreateIndex])) {
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
			if (create_singlesampled_images(bResolvingRequired, bBlittingRequired)) {
				if (create_render_images(bResolvingRequired, bBlittingRequired)) {
					vk_resolveRegion.sType = VK_STRUCTURE_TYPE_IMAGE_RESOLVE_2;
					vk_resolveRegion.pNext = nullptr;
					vk_resolveRegion.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
					vk_resolveRegion.srcSubresource.mipLevel = 0;
					vk_resolveRegion.srcSubresource.layerCount = 1;
					vk_resolveRegion.srcOffset.x = 0;
					vk_resolveRegion.srcOffset.y = 0;
					vk_resolveRegion.srcOffset.z = 0;
					vk_resolveRegion.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
					vk_resolveRegion.dstSubresource.mipLevel = 0;
					vk_resolveRegion.dstSubresource.layerCount = 1;
					vk_resolveRegion.dstOffset.x = 0;
					vk_resolveRegion.dstOffset.y = 0;
					vk_resolveRegion.dstOffset.z = 0;
					vk_resolveRegion.extent.width = vk_renderImageSize.width;
					vk_resolveRegion.extent.height = vk_renderImageSize.height;
					vk_resolveRegion.extent.depth = 1;
					vk_resolveInfo.sType = VK_STRUCTURE_TYPE_RESOLVE_IMAGE_INFO_2;
					vk_resolveInfo.pNext = nullptr;
					vk_resolveInfo.srcImage = vk_hRenderImages;
					vk_resolveInfo.srcImageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
					vk_resolveInfo.dstImageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
					vk_resolveInfo.regionCount = 1;
					vk_resolveInfo.pRegions = &vk_resolveRegion;
					vk_blitRegion.sType = VK_STRUCTURE_TYPE_IMAGE_BLIT_2;
					vk_blitRegion.pNext = nullptr;
					vk_blitRegion.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
					vk_blitRegion.srcSubresource.mipLevel = 0;
					vk_blitRegion.srcSubresource.layerCount = 1;
					vk_blitRegion.srcOffsets[0].x = 0;
					vk_blitRegion.srcOffsets[0].y = 0;
					vk_blitRegion.srcOffsets[0].z = 0;
					vk_blitRegion.srcOffsets[1].x = vk_renderImageSize.width;
					vk_blitRegion.srcOffsets[1].y = vk_renderImageSize.height;
					vk_blitRegion.srcOffsets[1].z = 1;
					vk_blitRegion.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
					vk_blitRegion.dstSubresource.mipLevel = 0;
					vk_blitRegion.dstSubresource.layerCount = 1;
					vk_blitRegion.dstOffsets[0].x = 0;
					vk_blitRegion.dstOffsets[0].y = 0;
					vk_blitRegion.dstOffsets[0].z = 0;
					vk_blitRegion.dstOffsets[1].x = vk_swapchainResolution.width;
					vk_blitRegion.dstOffsets[1].y = vk_swapchainResolution.height;
					vk_blitRegion.dstOffsets[1].z = 1;
					vk_blitInfo.sType = VK_STRUCTURE_TYPE_BLIT_IMAGE_INFO_2;
					vk_blitInfo.pNext = nullptr;
					vk_blitInfo.srcImageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
					vk_blitInfo.dstImageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
					vk_blitInfo.regionCount = 1;
					vk_blitInfo.pRegions = &vk_blitRegion;
					switch (screenPercentageSettings.eScalingFilter) {
						case RE_TEXTURE_FILTER_NEAREST:
							vk_blitInfo.filter = VK_FILTER_NEAREST;
							break;
						case RE_TEXTURE_FILTER_LINEAR:
							vk_blitInfo.filter = VK_FILTER_LINEAR;
							break;
					}
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

	bool record_command_buffer_transfering_render_image() {
		const bool bResolvingRequired = vk_eMsaaCount != VK_SAMPLE_COUNT_1_BIT, bBlittingRequired = screenPercentageSettings.eMode != RE_SCREEN_PERCENTAGE_MODE_NORMAL;
		if (!bResolvingRequired && !bBlittingRequired)
			return true;
		if (!renderTasks[u8CurrentFrameInFlightIndex].record(RENDER_TASK_SUBINDEX_IMAGE_TRANSFER, VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT, [&](const VkCommandBuffer vk_hCommandBuffer, const uint8_t u8PreviousLogicalQueue, const uint8_t u8CurrentLogicalQueue, const uint8_t u8NextLogicalQueue) {
				if (bResolvingRequired) {
					vkCmdResolveImage2(vk_hCommandBuffer, &vk_resolveInfo);
					if (bBlittingRequired)
						vkCmdBlitImage2(vk_hCommandBuffer, &vk_blitInfo);
				} else if (bBlittingRequired)
					vkCmdBlitImage2(vk_hCommandBuffer, &vk_blitInfo);
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
