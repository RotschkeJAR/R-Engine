#include "RE_Renderer_Internal.hpp"

namespace RE {

	void record_cmd_begin_dynamic_rendering(const VkCommandBuffer vk_hCommandBuffer) {
		PRINT_DEBUG("Beginning Vulkan dynamic rendering in command buffer ", vk_hCommandBuffer);
		VkRenderingAttachmentInfo vk_colorAttachment = {
			.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO,
			.imageView = renderTargetImageViews[u8CurrentFrameInFlightIndex],
			.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
			.resolveMode = VK_RESOLVE_MODE_AVERAGE_BIT,
			.resolveImageView = swapchainImageViews[u32SwapchainImageIndex],
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
				vk_colorAttachment.resolveImageView = singlesampledImageViews[u8CurrentFrameInFlightIndex];
		} else {
			vk_colorAttachment.resolveMode = VK_RESOLVE_MODE_NONE;
			if (!bBlittingRequired)
				vk_colorAttachment.imageView = swapchainImageViews[u32SwapchainImageIndex];
		}
		const VkRenderingAttachmentInfo vk_depthAttachment = {
			.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO,
			.imageView = depthImageViews[u8CurrentFrameInFlightIndex],
			.imageLayout = vk_eDepthLayout,
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
			.imageView = stencilImageViews[u8CurrentFrameInFlightIndex],
			.imageLayout = vk_eStencilLayout,
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
			.renderArea = {
				.extent = {
					.width = renderImageSize[0],
					.height = renderImageSize[1]
				}
			},
			.layerCount = 1,
			.viewMask = 0,
			.colorAttachmentCount = 1,
			.pColorAttachments = &vk_colorAttachment,
			.pDepthAttachment = &vk_depthAttachment,
			.pStencilAttachment = vk_hStencilImage != VK_NULL_HANDLE ? &vk_stencilAttachment : nullptr
		};
		vkCmdBeginRendering(vk_hCommandBuffer, &vk_renderInfo);
	}

	void record_cmd_end_dynamic_rendering(const VkCommandBuffer vk_hCommandBuffer) {
		PRINT_DEBUG("Ending dynamic rendering in command buffer ", vk_hCommandBuffer);
		vkCmdEndRendering(vk_hCommandBuffer);
	}

}
