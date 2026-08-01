#include "RE_Renderer_RenderPass.hpp"

namespace RE {

	VkRenderPass vk_hRenderPass = VK_NULL_HANDLE,
		vk_hSwapchainRenderPass = VK_NULL_HANDLE;
	uint32_t u32SubpassScenery,
		u32SwapchainSubpassWindowFrame;

	bool create_render_pass() {
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
					.dstSubpass = u32SubpassScenery,
					.srcStageMask = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
					.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
					.srcAccessMask = VK_ACCESS_NONE,
					.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
					.dependencyFlags = 0,
					.viewOffset = 0
				}, {
					.sType = VK_STRUCTURE_TYPE_SUBPASS_DEPENDENCY_2,
					.pNext = nullptr,
					.srcSubpass = u32SubpassScenery,
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
			vkDestroyRenderPass(vk_hDevice, vk_hRenderPass, nullptr);
		} else
			RE_FATAL_ERROR("Failed to create the Vulkan render pass for rendering in render target image");
		vk_hRenderPass = VK_NULL_HANDLE;
		vk_hSwapchainRenderPass = VK_NULL_HANDLE;
		return false;
	}

	void destroy_render_pass() {
		PRINT_DEBUG("Destroying Vulkan render passes used for rendering");
		vkDestroyRenderPass(vk_hDevice, vk_hRenderPass, nullptr);
		vkDestroyRenderPass(vk_hDevice, vk_hSwapchainRenderPass, nullptr);
		vk_hRenderPass = VK_NULL_HANDLE;
		vk_hSwapchainRenderPass = VK_NULL_HANDLE;
	}

}
