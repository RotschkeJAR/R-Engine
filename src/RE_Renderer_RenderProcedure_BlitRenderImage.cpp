#include "RE_Renderer_RenderProcedure_Internal.hpp"

namespace RE {

	void blit_render_image(VkCommandBuffer vk_hCommandBuffer) {
		if (!RENDER_IMAGE_SIZE_EQUALS_SWAPCHAIN()) {
			const VkImage vk_hSrcImage = IS_MSAA_ENABLED() ? vk_hSinglesampledImage : vk_hRenderTargetImage;
			PRINT_DEBUG("Recording command to blit data from Vulkan image ", vk_hSrcImage, " at layer ", uCurrentFrameInFlightIndex, " to swapchain image ", std_swapchainImages[u32CurrentSwapchainImageIndex]);
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
					vk_hSrcImage,
					VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
					std_swapchainImages[u32CurrentSwapchainImageIndex],
					VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
					1,
					&vk_blitInfo,
					vk_eScreenFilter);
		} else if (IS_MSAA_ENABLED()) {
			PRINT_DEBUG("Recording command to resolve data from Vulkan image ", vk_hRenderTargetImage, " at layer ", uCurrentFrameInFlightIndex, " to swapchain image ", std_swapchainImages[u32CurrentSwapchainImageIndex]);
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
					std_swapchainImages[u32CurrentSwapchainImageIndex],
					VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
					1,
					&vk_resolveInfo);
		} else {
			PRINT_DEBUG("Recording command to copy data from Vulkan image ", vk_hRenderTargetImage, " at layer ", uCurrentFrameInFlightIndex, " to swapchain image ", std_swapchainImages[u32CurrentSwapchainImageIndex]);
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
					std_swapchainImages[u32CurrentSwapchainImageIndex],
					VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
					1,
					&vk_copyInfo);
		}
	}

}
