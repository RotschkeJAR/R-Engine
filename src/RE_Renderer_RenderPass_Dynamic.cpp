#include "RE_Renderer_RenderPass_Internal.hpp"

namespace RE {

	bool create_render_pass_dynamic() {
		return true;
	}

	void destroy_render_pass_dynamic() {}

	void begin_render_pass_dynamic(VkCommandBuffer vk_hCommandBuffer) {
		const VkImageMemoryBarrier2 vk_aImageAttachmentBarriers[] = {
			{
				.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2,
				.pNext = nullptr,
				.srcStageMask = VK_PIPELINE_STAGE_2_TOP_OF_PIPE_BIT,
				.srcAccessMask = VK_ACCESS_2_NONE,
				.dstStageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT,
				.dstAccessMask = VK_ACCESS_2_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT,
				.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED,
				.newLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
				.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
				.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
				.image = vk_hRenderTargetImage,
				.subresourceRange = {
					.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
					.baseMipLevel = 0,
					.levelCount = 1,
					.baseArrayLayer = uCurrentFrameInFlightIndex,
					.layerCount = 1
				}
			}, {
				.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2,
				.pNext = nullptr,
				.srcStageMask = VK_PIPELINE_STAGE_2_TOP_OF_PIPE_BIT,
				.srcAccessMask = VK_ACCESS_2_NONE,
				.dstStageMask = VK_PIPELINE_STAGE_2_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_2_LATE_FRAGMENT_TESTS_BIT,
				.dstAccessMask = VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
				.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED,
				.newLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
				.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
				.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
				.image = vk_hDepthStencilImage,
				.subresourceRange = {
					.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT | static_cast<VkImageAspectFlags>(are_stencils_enabled() ? VK_IMAGE_ASPECT_STENCIL_BIT : 0),
					.baseMipLevel = 0,
					.levelCount = 1,
					.baseArrayLayer = uCurrentFrameInFlightIndex,
					.layerCount = 1
				}
			}, {
				.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2,
				.pNext = nullptr,
				.srcStageMask = VK_PIPELINE_STAGE_2_TOP_OF_PIPE_BIT,
				.srcAccessMask = VK_ACCESS_2_NONE,
				.dstStageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT,
				.dstAccessMask = VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT,
				.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED,
				.newLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
				.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
				.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
				.image = vk_hSinglesampledImage,
				.subresourceRange = {
					.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
					.baseMipLevel = 0,
					.levelCount = 1,
					.baseArrayLayer = uCurrentFrameInFlightIndex,
					.layerCount = 1
				}
			}
		};
		uint32_t u32AttachmentCount = static_cast<uint32_t>(sizeof(vk_aImageAttachmentBarriers) / sizeof(vk_aImageAttachmentBarriers[0]) - 1);
		if (!RENDER_IMAGE_SIZE_EQUALS_SWAPCHAIN() && IS_MSAA_ENABLED())
			u32AttachmentCount ++;
		const VkDependencyInfo vk_dependencyInfo = {
			.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO,
			.pNext = nullptr,
			.dependencyFlags = 0,
			.memoryBarrierCount = 0,
			.pMemoryBarriers = nullptr,
			.bufferMemoryBarrierCount = 0,
			.pBufferMemoryBarriers = nullptr,
			.imageMemoryBarrierCount = u32AttachmentCount,
			.pImageMemoryBarriers = vk_aImageAttachmentBarriers
		};
		vkCmdPipelineBarrier2(vk_hCommandBuffer, &vk_dependencyInfo);
		const VkRenderingAttachmentInfo vk_aColorAttachments[] = {
			{
				.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO,
				.pNext = nullptr,
				.imageView = vk_ahRenderTargetImageViews[uCurrentFrameInFlightIndex],
				.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
				.resolveMode = (!RENDER_IMAGE_SIZE_EQUALS_SWAPCHAIN() && IS_MSAA_ENABLED()) ? VK_RESOLVE_MODE_AVERAGE_BIT : VK_RESOLVE_MODE_NONE,
				.resolveImageView = vk_ahSinglesampledImageViews[uCurrentFrameInFlightIndex],
				.resolveImageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
				.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
				.storeOp = VK_ATTACHMENT_STORE_OP_STORE,
				.clearValue = {
					.color = {
						.float32 = {
							backgroundClearColor[0],
							backgroundClearColor[1],
							backgroundClearColor[2],
							backgroundClearColor[3]
						}
					}
				}
			}
		},
		vk_aDepthStencilAttachments[] = {
			{
				.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO,
				.pNext = nullptr,
				.imageView = vk_ahDepthStencilImageViews[uCurrentFrameInFlightIndex],
				.imageLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
				.resolveMode = VK_RESOLVE_MODE_NONE,
				.resolveImageView = VK_NULL_HANDLE,
				.resolveImageLayout = VK_IMAGE_LAYOUT_UNDEFINED,
				.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
				.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
				.clearValue = {
					.depthStencil = {
						.depth = 1.0f,
						.stencil = 0
					}
				}
			}
		};
		const VkRenderingInfo vk_renderingInfo = {
			.sType = VK_STRUCTURE_TYPE_RENDERING_INFO,
			.pNext = nullptr,
			.flags = 0,
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
			.layerCount = 1,
			.viewMask = 0,
			.colorAttachmentCount = sizeof(vk_aColorAttachments) / sizeof(vk_aColorAttachments[0]),
			.pColorAttachments = vk_aColorAttachments,
			.pDepthAttachment = vk_aDepthStencilAttachments,
			.pStencilAttachment = are_stencils_enabled() ? vk_aDepthStencilAttachments : nullptr
		};
		vkCmdBeginRendering(vk_hCommandBuffer, &vk_renderingInfo);
	}

	void next_render_subpass_dynamic(VkCommandBuffer vk_hCommandBuffer) {}

	void end_render_pass_dynamic(VkCommandBuffer vk_hCommandBuffer) {
		vkCmdEndRendering(vk_hCommandBuffer);
		VkImageMemoryBarrier2 vk_finalImageAttachmentBarrier;
		vk_finalImageAttachmentBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2;
		vk_finalImageAttachmentBarrier.pNext = nullptr;
		vk_finalImageAttachmentBarrier.srcStageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT;
		vk_finalImageAttachmentBarrier.srcAccessMask = VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT;
		vk_finalImageAttachmentBarrier.dstAccessMask = VK_ACCESS_2_TRANSFER_READ_BIT;
		vk_finalImageAttachmentBarrier.oldLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		vk_finalImageAttachmentBarrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
		vk_finalImageAttachmentBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		vk_finalImageAttachmentBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		vk_finalImageAttachmentBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		vk_finalImageAttachmentBarrier.subresourceRange.baseMipLevel = 0;
		vk_finalImageAttachmentBarrier.subresourceRange.levelCount = 1;
		vk_finalImageAttachmentBarrier.subresourceRange.baseArrayLayer = uCurrentFrameInFlightIndex;
		vk_finalImageAttachmentBarrier.subresourceRange.layerCount = 1;
		if (!RENDER_IMAGE_SIZE_EQUALS_SWAPCHAIN()) {
			vk_finalImageAttachmentBarrier.dstStageMask = VK_PIPELINE_STAGE_2_BLIT_BIT;
			vk_finalImageAttachmentBarrier.image = IS_MSAA_ENABLED() ? vk_hSinglesampledImage : vk_hRenderTargetImage;
		} else if (IS_MSAA_ENABLED()) {
			vk_finalImageAttachmentBarrier.dstStageMask = VK_PIPELINE_STAGE_2_RESOLVE_BIT;
			vk_finalImageAttachmentBarrier.image = vk_hRenderTargetImage;
		} else {
			vk_finalImageAttachmentBarrier.dstStageMask = VK_PIPELINE_STAGE_2_COPY_BIT;
			vk_finalImageAttachmentBarrier.image = vk_hRenderTargetImage;
		}
		const VkDependencyInfo vk_dependencyInfo = {
			.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO,
			.pNext = nullptr,
			.dependencyFlags = 0,
			.memoryBarrierCount = 0,
			.pMemoryBarriers = nullptr,
			.bufferMemoryBarrierCount = 0,
			.pBufferMemoryBarriers = nullptr,
			.imageMemoryBarrierCount = 1,
			.pImageMemoryBarriers = &vk_finalImageAttachmentBarrier
		};
		vkCmdPipelineBarrier2(vk_hCommandBuffer, &vk_dependencyInfo);
	}

	void begin_swapchain_render_pass_dynamic(VkCommandBuffer vk_hCommandBuffer) {
		const VkImageMemoryBarrier2 vk_swapchainImageAttachmentBarrier = {
			.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2,
			.pNext = nullptr,
			.srcStageMask = VK_PIPELINE_STAGE_2_TOP_OF_PIPE_BIT,
			.srcAccessMask = VK_ACCESS_2_NONE,
			.dstStageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT,
			.dstAccessMask = VK_ACCESS_2_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT,
			.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED,
			.newLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
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
		const VkDependencyInfo vk_dependencyInfo = {
			.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO,
			.pNext = nullptr,
			.dependencyFlags = 0,
			.memoryBarrierCount = 0,
			.pMemoryBarriers = nullptr,
			.bufferMemoryBarrierCount = 0,
			.pBufferMemoryBarriers = nullptr,
			.imageMemoryBarrierCount = 1,
			.pImageMemoryBarriers = &vk_swapchainImageAttachmentBarrier
		};
		vkCmdPipelineBarrier2(vk_hCommandBuffer, &vk_dependencyInfo);
		const VkRenderingAttachmentInfo vk_renderingAttachment = {
			.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO,
			.pNext = nullptr,
			.imageView = std_swapchainImageViews[u32CurrentSwapchainImageIndex],
			.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
			.resolveMode = VK_RESOLVE_MODE_NONE,
			.resolveImageView = VK_NULL_HANDLE,
			.resolveImageLayout = VK_IMAGE_LAYOUT_UNDEFINED,
			.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
			.storeOp = VK_ATTACHMENT_STORE_OP_STORE,
			.clearValue = {
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
		const VkRenderingInfo vk_renderingInfo = {
			.sType = VK_STRUCTURE_TYPE_RENDERING_INFO,
			.pNext = nullptr,
			.flags = 0,
			.renderArea = {
				.offset = {
					.x = 0,
					.y = 0
				},
				.extent = vk_swapchainResolution
			},
			.layerCount = 1,
			.viewMask = 0,
			.colorAttachmentCount = 1,
			.pColorAttachments = &vk_renderingAttachment,
			.pDepthAttachment = nullptr,
			.pStencilAttachment = nullptr
		};
		vkCmdBeginRendering(vk_hCommandBuffer, &vk_renderingInfo);
	}

	void next_swapchain_render_pass_dynamic(VkCommandBuffer vk_hCommandBuffer) {}

	void end_swapchain_render_pass_dynamic(VkCommandBuffer vk_hCommandBuffer) {
		vkCmdEndRendering(vk_hCommandBuffer);
		VkImageMemoryBarrier2 vk_finalSwapchainImageAttachmentBarrier = {
			.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2,
			.pNext = nullptr,
			.srcStageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT,
			.srcAccessMask = VK_ACCESS_2_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT,
			.dstAccessMask = VK_ACCESS_2_TRANSFER_WRITE_BIT,
			.oldLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
			.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
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
		if (!RENDER_IMAGE_SIZE_EQUALS_SWAPCHAIN())
			vk_finalSwapchainImageAttachmentBarrier.dstStageMask = VK_PIPELINE_STAGE_2_BLIT_BIT;
		else if (IS_MSAA_ENABLED())
			vk_finalSwapchainImageAttachmentBarrier.dstStageMask = VK_PIPELINE_STAGE_2_RESOLVE_BIT;
		else
			vk_finalSwapchainImageAttachmentBarrier.dstStageMask = VK_PIPELINE_STAGE_2_COPY_BIT;
		const VkDependencyInfo vk_dependencyInfo = {
			.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO,
			.pNext = nullptr,
			.dependencyFlags = 0,
			.memoryBarrierCount = 0,
			.pMemoryBarriers = nullptr,
			.bufferMemoryBarrierCount = 0,
			.pBufferMemoryBarriers = nullptr,
			.imageMemoryBarrierCount = 1,
			.pImageMemoryBarriers = &vk_finalSwapchainImageAttachmentBarrier
		};
		vkCmdPipelineBarrier2(vk_hCommandBuffer, &vk_dependencyInfo);
	}

	void setup_graphics_pipelines_render_pass_dynamic(
			VkGraphicsPipelineCreateInfo (&vk_arGraphicsPipelineCreateInfos)[GRAPHICS_PIPELINE_COUNT],
			VkPipelineRenderingCreateInfo &vk_rDynamicRenderPassInfo) {
		vk_rDynamicRenderPassInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO;
		vk_rDynamicRenderPassInfo.pNext = nullptr;
		vk_rDynamicRenderPassInfo.viewMask = 0;
		vk_rDynamicRenderPassInfo.colorAttachmentCount = 1;
		vk_rDynamicRenderPassInfo.pColorAttachmentFormats = &vk_eSwapchainImageFormat;
		vk_rDynamicRenderPassInfo.depthAttachmentFormat = vk_eSelectedDepthStencilFormat;
		vk_rDynamicRenderPassInfo.stencilAttachmentFormat = are_stencils_enabled() ? vk_eSelectedDepthStencilFormat : VK_FORMAT_UNDEFINED;
		for (VkGraphicsPipelineCreateInfo &vk_rGraphicsPipelineCreateInfo : vk_arGraphicsPipelineCreateInfos) {
			VkBaseInStructure *vk_pGraphicsPipelineExtension = reinterpret_cast<VkBaseInStructure*>(std::addressof(vk_rGraphicsPipelineCreateInfo));
			while (vk_pGraphicsPipelineExtension->pNext)
				vk_pGraphicsPipelineExtension = const_cast<VkBaseInStructure*>(vk_pGraphicsPipelineExtension->pNext);
			vk_pGraphicsPipelineExtension->pNext = reinterpret_cast<VkBaseInStructure*>(std::addressof(vk_rDynamicRenderPassInfo));
			vk_rGraphicsPipelineCreateInfo.renderPass = VK_NULL_HANDLE;
			vk_rGraphicsPipelineCreateInfo.subpass = 0;
		}
	}

	void setup_swapchain_pipelines_render_pass_dynamic(
			VkGraphicsPipelineCreateInfo &vk_rGraphicsPipelineCreateInfo,
			VkPipelineRenderingCreateInfo &vk_rDynamicRenderPassInfo) {
		vk_rDynamicRenderPassInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO;
		vk_rDynamicRenderPassInfo.pNext = nullptr;
		vk_rDynamicRenderPassInfo.viewMask = 0;
		vk_rDynamicRenderPassInfo.colorAttachmentCount = 1;
		vk_rDynamicRenderPassInfo.pColorAttachmentFormats = &vk_eSwapchainImageFormat;
		vk_rDynamicRenderPassInfo.depthAttachmentFormat = VK_FORMAT_UNDEFINED;
		vk_rDynamicRenderPassInfo.stencilAttachmentFormat = VK_FORMAT_UNDEFINED;
		VkBaseInStructure *vk_pGraphicsPipelineExtension = reinterpret_cast<VkBaseInStructure*>(std::addressof(vk_rGraphicsPipelineCreateInfo));
		while (vk_pGraphicsPipelineExtension->pNext)
			vk_pGraphicsPipelineExtension = const_cast<VkBaseInStructure*>(vk_pGraphicsPipelineExtension->pNext);
		vk_pGraphicsPipelineExtension->pNext = reinterpret_cast<VkBaseInStructure*>(std::addressof(vk_rDynamicRenderPassInfo));
		vk_rGraphicsPipelineCreateInfo.renderPass = VK_NULL_HANDLE;
		vk_rGraphicsPipelineCreateInfo.subpass = 0;
	}

}
