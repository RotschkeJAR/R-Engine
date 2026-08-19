#include "RE_Renderer_RenderPass_Internal.hpp"

namespace RE {

	bool create_render_pass_static() {
		u32SubpassScenery = 0;
		unsigned uAttachmentCount = 2U + bool_to_int(IS_SINGLESAMPLED_IMAGE_REQUIRED());
		const VkAttachmentDescription2 vk_aAttachmentDescs[] = {
			{
				.sType = VK_STRUCTURE_TYPE_ATTACHMENT_DESCRIPTION_2,
				.pNext = nullptr,
				.flags = 0,
				.format = vk_eSwapchainImageFormat,
				.samples = vk_eMsaaMode,
				.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
				.storeOp = VK_ATTACHMENT_STORE_OP_STORE,
				.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
				.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
				.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
				.finalLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL
			}, {
				.sType = VK_STRUCTURE_TYPE_ATTACHMENT_DESCRIPTION_2,
				.pNext = nullptr,
				.flags = 0,
				.format = vk_eSelectedDepthStencilFormat,
				.samples = vk_eMsaaMode,
				.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
				.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
				.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
				.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
				.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
				.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL
			}, {
				.sType = VK_STRUCTURE_TYPE_ATTACHMENT_DESCRIPTION_2,
				.pNext = nullptr,
				.flags = 0,
				.format = vk_eSwapchainImageFormat,
				.samples = VK_SAMPLE_COUNT_1_BIT,
				.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
				.storeOp = VK_ATTACHMENT_STORE_OP_STORE,
				.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
				.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
				.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
				.finalLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL
			}
		};
		const VkAttachmentReference2 vk_colorRef = {
			.sType = VK_STRUCTURE_TYPE_ATTACHMENT_REFERENCE_2,
			.pNext = nullptr,
			.attachment = 0,
			.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
			.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT
		},
		vk_depthStencilRef = {
			.sType = VK_STRUCTURE_TYPE_ATTACHMENT_REFERENCE_2,
			.pNext = nullptr,
			.attachment = 1,
			.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
			.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT | static_cast<VkImageAspectFlags>(are_stencils_enabled() ? VK_IMAGE_ASPECT_STENCIL_BIT : 0)
		},
		vk_singlesampledRef = {
			.sType = VK_STRUCTURE_TYPE_ATTACHMENT_REFERENCE_2,
			.pNext = nullptr,
			.attachment = 2,
			.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
			.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT
		};
		const VkSubpassDescription2 vk_aSubpassDescs[] = {
			{
				.sType = VK_STRUCTURE_TYPE_SUBPASS_DESCRIPTION_2,
				.pNext = nullptr,
				.flags = 0,
				.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,
				.viewMask = 0,
				.inputAttachmentCount = 0,
				.pInputAttachments = nullptr,
				.colorAttachmentCount = 1,
				.pColorAttachments = &vk_colorRef,
				.pResolveAttachments = IS_SINGLESAMPLED_IMAGE_REQUIRED() ? &vk_singlesampledRef : nullptr,
				.pDepthStencilAttachment = &vk_depthStencilRef,
				.preserveAttachmentCount = 0,
				.pPreserveAttachments = nullptr
			}
		};
		const VkSubpassDependency2 vk_aDependencies[] = {
			{
				.sType = VK_STRUCTURE_TYPE_SUBPASS_DEPENDENCY_2,
				.pNext = nullptr,
				.srcSubpass = VK_SUBPASS_EXTERNAL,
				.dstSubpass = u32SubpassScenery,
				.srcStageMask = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
				.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT,
				.srcAccessMask = VK_ACCESS_NONE,
				.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
				.dependencyFlags = 0,
				.viewOffset = 0
			}, {
				.sType = VK_STRUCTURE_TYPE_SUBPASS_DEPENDENCY_2,
				.pNext = nullptr,
				.srcSubpass = u32SubpassScenery,
				.dstSubpass = VK_SUBPASS_EXTERNAL,
				.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT,
				.dstStageMask = VK_PIPELINE_STAGE_TRANSFER_BIT,
				.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
				.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT,
				.dependencyFlags = 0,
				.viewOffset = 0
			}
		};
		const VkRenderPassCreateInfo2 vk_renderPassCreateInfo = {
			.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO_2,
			.pNext = nullptr,
			.flags = 0,
			.attachmentCount = static_cast<uint32_t>(uAttachmentCount),
			.pAttachments = vk_aAttachmentDescs,
			.subpassCount = sizeof(vk_aSubpassDescs) / sizeof(vk_aSubpassDescs[0]),
			.pSubpasses = vk_aSubpassDescs,
			.dependencyCount = sizeof(vk_aDependencies) / sizeof(vk_aDependencies[0]),
			.pDependencies = vk_aDependencies,
			.correlatedViewMaskCount = 0,
			.pCorrelatedViewMasks = nullptr
		};
		if (vkCreateRenderPass2(vk_hDevice, &vk_renderPassCreateInfo, nullptr, &vk_hRenderPass) == VK_SUCCESS) {
			u32SwapchainSubpassWindowFrame = 0;
			const VkAttachmentDescription2 vk_aAttachmentDescs[] = {
				{
					.sType = VK_STRUCTURE_TYPE_ATTACHMENT_DESCRIPTION_2,
					.pNext = nullptr,
					.flags = 0,
					.format = vk_eSwapchainImageFormat,
					.samples = VK_SAMPLE_COUNT_1_BIT,
					.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
					.storeOp = VK_ATTACHMENT_STORE_OP_STORE,
					.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
					.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
					.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
					.finalLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL
				}
			};
			const VkAttachmentReference2 vk_colorRef = {
				.sType = VK_STRUCTURE_TYPE_ATTACHMENT_REFERENCE_2,
				.pNext = nullptr,
				.attachment = 0,
				.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
				.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT
			};
			const VkSubpassDescription2 vk_aSubpassDescs[] = {
				{
					.sType = VK_STRUCTURE_TYPE_SUBPASS_DESCRIPTION_2,
					.pNext = nullptr,
					.flags = 0,
					.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,
					.viewMask = 0,
					.inputAttachmentCount = 0,
					.pInputAttachments = nullptr,
					.colorAttachmentCount = 1,
					.pColorAttachments = &vk_colorRef,
					.pResolveAttachments = nullptr,
					.pDepthStencilAttachment = nullptr,
					.preserveAttachmentCount = 0,
					.pPreserveAttachments = nullptr
				}
			};
			const VkSubpassDependency2 vk_aDependencies[] = {
				{
					.sType = VK_STRUCTURE_TYPE_SUBPASS_DEPENDENCY_2,
					.pNext = nullptr,
					.srcSubpass = VK_SUBPASS_EXTERNAL,
					.dstSubpass = u32SwapchainSubpassWindowFrame,
					.srcStageMask = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
					.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
					.srcAccessMask = VK_ACCESS_NONE,
					.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
					.dependencyFlags = 0,
					.viewOffset = 0
				}, {
					.sType = VK_STRUCTURE_TYPE_SUBPASS_DEPENDENCY_2,
					.pNext = nullptr,
					.srcSubpass = u32SwapchainSubpassWindowFrame,
					.dstSubpass = VK_SUBPASS_EXTERNAL,
					.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
					.dstStageMask = VK_PIPELINE_STAGE_TRANSFER_BIT,
					.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
					.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT,
					.dependencyFlags = 0,
					.viewOffset = 0
				}
			};
			const VkRenderPassCreateInfo2 vk_swapchainRenderPassCreateInfo = {
				.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO_2,
				.pNext = nullptr,
				.flags = 0,
				.attachmentCount = sizeof(vk_aAttachmentDescs) / sizeof(vk_aAttachmentDescs[0]),
				.pAttachments = vk_aAttachmentDescs,
				.subpassCount = sizeof(vk_aSubpassDescs) / sizeof(vk_aSubpassDescs[0]),
				.pSubpasses = vk_aSubpassDescs,
				.dependencyCount = sizeof(vk_aDependencies) / sizeof(vk_aDependencies[0]),
				.pDependencies = vk_aDependencies,
				.correlatedViewMaskCount = 0,
				.pCorrelatedViewMasks = nullptr
			};
			if (vkCreateRenderPass2(vk_hDevice, &vk_swapchainRenderPassCreateInfo, nullptr, &vk_hSwapchainRenderPass) == VK_SUCCESS) {
				return true;
			} else
				RE_FATAL_ERROR("Failed to create the Vulkan render pass for rendering in the swapchain directly");
		} else
			RE_FATAL_ERROR("Failed to create the Vulkan render pass for rendering in render target image");
		destroy_render_pass_static();
		return false;
	}

	void destroy_render_pass_static() {
		PRINT_DEBUG("Destroying Vulkan render passes used for rendering");
		vkDestroyRenderPass(vk_hDevice, vk_hRenderPass, nullptr);
		vkDestroyRenderPass(vk_hDevice, vk_hSwapchainRenderPass, nullptr);
		vk_hRenderPass = VK_NULL_HANDLE;
		vk_hSwapchainRenderPass = VK_NULL_HANDLE;
		u32SubpassScenery = 0;
		u32SwapchainSubpassWindowFrame = 0;
	}

	void begin_render_pass_static(
			VkCommandBuffer vk_hCommandBuffer,
			VkClearValue (&vk_raClears)[RENDER_PASS_ATTACHMENT_COUNT],
			VkRenderPassBeginInfo &vk_rRenderPassBeginInfo,
			VkSubpassBeginInfo &vk_rSubpassBeginInfo,
			VkSubpassEndInfo &vk_rSubpassEndInfo) {
		for (unsigned uChannelIndex = 0; uChannelIndex < Color::uColorChannelCount; uChannelIndex++) {
			vk_raClears[RENDER_PASS_ATTACHMENT_COLOR].color.float32[uChannelIndex] = backgroundClearColor[uChannelIndex];
			vk_raClears[RENDER_PASS_ATTACHMENT_RESOLVE_DST].color.float32[uChannelIndex] = 0.0f;
		}
		vk_raClears[RENDER_PASS_ATTACHMENT_DEPTH_STENCIL].depthStencil.depth = 1.0f;
		vk_raClears[RENDER_PASS_ATTACHMENT_DEPTH_STENCIL].depthStencil.stencil = 0;
		vk_rRenderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		vk_rRenderPassBeginInfo.pNext = nullptr;
		vk_rRenderPassBeginInfo.renderPass = vk_hRenderPass;
		vk_rRenderPassBeginInfo.framebuffer = vk_ahFramebuffers[uCurrentFrameInFlightIndex];
		vk_rRenderPassBeginInfo.renderArea.offset.x = 0;
		vk_rRenderPassBeginInfo.renderArea.offset.y = 0;
		vk_rRenderPassBeginInfo.renderArea.extent.width = renderImageSize[0];
		vk_rRenderPassBeginInfo.renderArea.extent.height = renderImageSize[1];
		vk_rRenderPassBeginInfo.clearValueCount = RENDER_PASS_ATTACHMENT_COUNT;
		vk_rRenderPassBeginInfo.pClearValues = vk_raClears;
		vk_rSubpassBeginInfo.sType = VK_STRUCTURE_TYPE_SUBPASS_BEGIN_INFO;
		vk_rSubpassBeginInfo.pNext = nullptr;
		vk_rSubpassBeginInfo.contents = VK_SUBPASS_CONTENTS_INLINE;
		vk_rSubpassEndInfo.sType = VK_STRUCTURE_TYPE_SUBPASS_END_INFO;
		vk_rSubpassEndInfo.pNext = nullptr;
		vkCmdBeginRenderPass2(vk_hCommandBuffer, &vk_rRenderPassBeginInfo, &vk_rSubpassBeginInfo);
	}

	void next_render_subpass_static(
			VkCommandBuffer vk_hCommandBuffer,
			VkSubpassBeginInfo &vk_rSubpassBeginInfo,
			VkSubpassEndInfo &vk_rSubpassEndInfo) {
		vk_rSubpassBeginInfo.contents = VK_SUBPASS_CONTENTS_INLINE;
		vkCmdNextSubpass2(vk_hCommandBuffer, &vk_rSubpassBeginInfo, &vk_rSubpassEndInfo);
	}

	void end_render_pass_static(
			VkCommandBuffer vk_hCommandBuffer,
			VkSubpassEndInfo &vk_rSubpassEndInfo) {
		vkCmdEndRenderPass2(vk_hCommandBuffer, &vk_rSubpassEndInfo);
	}

	void begin_swapchain_render_pass_static(
			VkCommandBuffer vk_hCommandBuffer,
			VkClearValue (&vk_raClears)[RENDER_PASS_ATTACHMENT_COUNT],
			VkRenderPassBeginInfo &vk_rRenderPassBeginInfo,
			VkSubpassBeginInfo &vk_rSubpassBeginInfo) {
		vk_rRenderPassBeginInfo.renderPass = vk_hSwapchainRenderPass;
		vk_rRenderPassBeginInfo.framebuffer = std_swapchainFramebuffers[uCurrentFrameInFlightIndex];
		vk_rRenderPassBeginInfo.renderArea.extent = vk_swapchainResolution;
		vk_rRenderPassBeginInfo.clearValueCount = 1;
		vk_raClears[0].color.float32[0] = 0.0f;
		vk_raClears[0].color.float32[1] = 0.0f;
		vk_raClears[0].color.float32[2] = 0.0f;
		vk_raClears[0].color.float32[3] = 1.0f;
		vkCmdBeginRenderPass2(vk_hCommandBuffer, &vk_rRenderPassBeginInfo, &vk_rSubpassBeginInfo);
	}

	void next_swapchain_render_pass_static(
			VkCommandBuffer vk_hCommandBuffer,
			VkSubpassBeginInfo &vk_rSubpassBeginInfo,
			VkSubpassEndInfo &vk_rSubpassEndInfo) {
		vk_rSubpassBeginInfo.contents = VK_SUBPASS_CONTENTS_INLINE;
		vkCmdNextSubpass2(vk_hCommandBuffer, &vk_rSubpassBeginInfo, &vk_rSubpassEndInfo);
	}

	void end_swapchain_render_pass_static(
			VkCommandBuffer vk_hCommandBuffer,
			VkSubpassEndInfo &vk_rSubpassEndInfo) {
		vkCmdEndRenderPass2(vk_hCommandBuffer, &vk_rSubpassEndInfo);
	}

	void setup_graphics_pipelines_render_pass_static(VkGraphicsPipelineCreateInfo (&vk_arGraphicsPipelineCreateInfos)[GRAPHICS_PIPELINE_COUNT]) {
		for (VkGraphicsPipelineCreateInfo &vk_rGraphicsPipelineCreateInfo : vk_arGraphicsPipelineCreateInfos) {
			vk_rGraphicsPipelineCreateInfo.renderPass = vk_hRenderPass;
			vk_rGraphicsPipelineCreateInfo.subpass = u32SubpassScenery;
		}
	}

	void setup_swapchain_pipelines_render_pass_static(VkGraphicsPipelineCreateInfo &vk_rGraphicsPipelineCreateInfo) {
		vk_rGraphicsPipelineCreateInfo.renderPass = vk_hSwapchainRenderPass;
		vk_rGraphicsPipelineCreateInfo.subpass = u32SwapchainSubpassWindowFrame;
	}

}
