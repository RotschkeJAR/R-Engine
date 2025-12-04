#include "RE_Renderer_Internal.hpp"

namespace RE {
	
	bool create_renderpass() {
		PRINT_DEBUG("Creating Vulkan render pass");
		return true;
	}

	void destroy_renderpass() {
		PRINT_DEBUG("Destroying Vulkan render pass");
	}

	void record_cmd_begin_renderpass(const VkCommandBuffer vk_hCommandBuffer) {
		PRINT_DEBUG("Beginning dynamic renderpass in command buffer ", vk_hCommandBuffer);
		VkRenderingAttachmentInfo vk_colorAttachment = {
			.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO,
			.imageView = vk_ahRenderImageViews[u8CurrentFrameInFlightIndex],
			.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
			.resolveMode = VK_RESOLVE_MODE_AVERAGE_BIT,
			.resolveImageView = vk_pahSwapchainImageViews[u32SwapchainImageIndex],
			.resolveImageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
			.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
			.storeOp = VK_ATTACHMENT_STORE_OP_STORE,
			.clearValue = {
				.color = {
					.float32 = {backgroundClearColor[0], backgroundClearColor[1], backgroundClearColor[2], backgroundClearColor[3]}
				}
			}
		};
		const bool bBlittingRequired = screenPercentageSettings.eMode != RE_SCREEN_PERCENTAGE_MODE_NORMAL;
		if (vk_eMsaaCount != VK_SAMPLE_COUNT_1_BIT) {
			if (bBlittingRequired)
				vk_colorAttachment.resolveImageView = vk_ahSingleSampledWorldRenderImageViews[u8CurrentFrameInFlightIndex];
		} else {
			vk_colorAttachment.resolveMode = VK_RESOLVE_MODE_NONE;
			if (!bBlittingRequired)
				vk_colorAttachment.imageView = vk_pahSwapchainImageViews[u32SwapchainImageIndex];
		}
		const bool bDepthStencilImagesSeparated = are_depth_stencil_images_separated(availableDepthStencilFormats[u8IndexToSelectedDepthStencilFormat]);
		const VkRenderingAttachmentInfo vk_depthAttachment = {
			.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO,
			.imageView = vk_a4hDepthStencilImageViews[u8CurrentFrameInFlightIndex * 2 + DEPTH_IMAGE_INDEX],
			.imageLayout = bDepthStencilImagesSeparated ? VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL : VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
			.resolveMode = VK_RESOLVE_MODE_NONE,
			.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
			.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
			.clearValue = {
				.depthStencil = {
					.depth = 1.0f
				}
			}
		};
		const VkRenderingAttachmentInfo vk_stencilAttachment = {
			.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO,
			.imageView = vk_a4hDepthStencilImageViews[u8CurrentFrameInFlightIndex * 2 + STENCIL_IMAGE_INDEX],
			.imageLayout = bDepthStencilImagesSeparated ? VK_IMAGE_LAYOUT_STENCIL_ATTACHMENT_OPTIMAL : VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
			.resolveMode = VK_RESOLVE_MODE_NONE,
			.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
			.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
			.clearValue = {
				.depthStencil = {
					.stencil = 0
				}
			}
		};
		const VkRenderingInfo vk_renderInfo = {
			.sType = VK_STRUCTURE_TYPE_RENDERING_INFO,
			.flags = 0,
			.renderArea = vk_cameraProjectionOnscreen,
			.layerCount = 1,
			.viewMask = 0,
			.colorAttachmentCount = 1,
			.pColorAttachments = &vk_colorAttachment,
			.pDepthAttachment = &vk_depthAttachment,
			.pStencilAttachment = bStencilsEnabled ? &vk_stencilAttachment : nullptr
		};
		vkCmdBeginRendering(vk_hCommandBuffer, &vk_renderInfo);
	}

	void record_cmd_end_renderpass(const VkCommandBuffer vk_hCommandBuffer) {
		PRINT_DEBUG("Ending dynamic renderpass in command buffer ", vk_hCommandBuffer);
		vkCmdEndRendering(vk_hCommandBuffer);
	}

}
