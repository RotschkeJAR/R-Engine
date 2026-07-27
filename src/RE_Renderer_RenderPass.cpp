#include "RE_Renderer_RenderPass.hpp"
#include "RE_Settings.hpp"

namespace RE {

	VkRenderPass vk_hRenderPass;

	bool create_render_pass() {
		PRINT_DEBUG("Creating Vulkan render pass for rendering");
		const bool bMultisamplingEnabled = vk_eMsaaMode != VK_SAMPLE_COUNT_1_BIT;
		unsigned uAttachmentCount = 2;
		VkAttachmentDescription2 vk_aAttachmentDescs[3];
		vk_aAttachmentDescs[0].sType = VK_STRUCTURE_TYPE_ATTACHMENT_DESCRIPTION_2;
		vk_aAttachmentDescs[0].pNext = nullptr;
		vk_aAttachmentDescs[0].format = vk_eSwapchainImageFormat;
		vk_aAttachmentDescs[0].samples = vk_eMsaaMode;
		vk_aAttachmentDescs[0].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		vk_aAttachmentDescs[0].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		vk_aAttachmentDescs[0].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		vk_aAttachmentDescs[0].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		vk_aAttachmentDescs[0].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		if (vk_hRenderTargetImage) {
			vk_aAttachmentDescs[0].flags = 0;
			vk_aAttachmentDescs[0].finalLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
		} else {
			vk_aAttachmentDescs[0].flags = VK_ATTACHMENT_DESCRIPTION_MAY_ALIAS_BIT;
			vk_aAttachmentDescs[0].finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
		}
		vk_aAttachmentDescs[1].sType = VK_STRUCTURE_TYPE_ATTACHMENT_DESCRIPTION_2;
		vk_aAttachmentDescs[1].pNext = nullptr;
		vk_aAttachmentDescs[1].flags = 0;
		vk_aAttachmentDescs[1].format = vk_eSelectedDepthStencilFormat;
		vk_aAttachmentDescs[1].samples = vk_eMsaaMode;
		vk_aAttachmentDescs[1].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		vk_aAttachmentDescs[1].storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		vk_aAttachmentDescs[1].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		vk_aAttachmentDescs[1].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		vk_aAttachmentDescs[1].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		vk_aAttachmentDescs[1].finalLayout = bStencilsEnabled ? VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL : VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL;
		if (bMultisamplingEnabled) {
			vk_aAttachmentDescs[2].sType = VK_STRUCTURE_TYPE_ATTACHMENT_DESCRIPTION_2;
			vk_aAttachmentDescs[2].pNext = nullptr;
			vk_aAttachmentDescs[2].flags = 0;
			vk_aAttachmentDescs[2].format = vk_eSwapchainImageFormat;
			vk_aAttachmentDescs[2].samples = VK_SAMPLE_COUNT_1_BIT;
			vk_aAttachmentDescs[2].loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
			vk_aAttachmentDescs[2].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
			vk_aAttachmentDescs[2].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
			vk_aAttachmentDescs[2].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
			vk_aAttachmentDescs[2].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			vk_aAttachmentDescs[2].finalLayout = vk_hSinglesampledImage ? VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL : VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
			uAttachmentCount++;
		}
		const VkAttachmentReference2 vk_aWindowFrameAttachmentRefs[] = {
			{
				.sType = VK_STRUCTURE_TYPE_ATTACHMENT_REFERENCE_2,
				.pNext = nullptr,
				.attachment = 0,
				.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
				.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT
			}
		},
		vk_aColorAttachmentRefs[] = {
			{
				.sType = VK_STRUCTURE_TYPE_ATTACHMENT_REFERENCE_2,
				.pNext = nullptr,
				.attachment = 1,
				.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
				.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT
			}
		},
		vk_aDepthStencilAttachmentRefs[] = {
			{
				.sType = VK_STRUCTURE_TYPE_ATTACHMENT_REFERENCE_2,
				.pNext = nullptr,
				.attachment = 2,
				.layout = bStencilsEnabled ? VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL : VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL,
				.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT | static_cast<VkImageAspectFlags>(bStencilsEnabled ? VK_IMAGE_ASPECT_STENCIL_BIT : 0)
			}
		},
		vk_aSinglesampledAttachmentRefs[] = {
			{
				.sType = VK_STRUCTURE_TYPE_ATTACHMENT_REFERENCE_2,
				.pNext = nullptr,
				.attachment = 3,
				.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
				.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT
			}
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
				.colorAttachmentCount = sizeof(vk_aWindowFrameAttachmentRefs) / sizeof(vk_aWindowFrameAttachmentRefs[0]),
				.pColorAttachments = vk_aWindowFrameAttachmentRefs,
				.pResolveAttachments = nullptr,
				.pDepthStencilAttachment = nullptr,
				.preserveAttachmentCount = 0,
				.pPreserveAttachments = nullptr
			}, {
				.sType = VK_STRUCTURE_TYPE_SUBPASS_DESCRIPTION_2,
				.pNext = nullptr,
				.flags = 0,
				.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,
				.viewMask = 0,
				.inputAttachmentCount = 0,
				.pInputAttachments = nullptr,
				.colorAttachmentCount = sizeof(vk_aColorAttachmentRefs) / sizeof(vk_aColorAttachmentRefs[0]),
				.pColorAttachments = vk_aColorAttachmentRefs,
				.pResolveAttachments = bMultisamplingEnabled ? vk_aSinglesampledAttachmentRefs : nullptr,
				.pDepthStencilAttachment = vk_aDepthStencilAttachmentRefs,
				.preserveAttachmentCount = 0,
				.pPreserveAttachments = nullptr
			}
		};
		PRINT_LN(vk_aSubpassDescs[0].colorAttachmentCount, ", ", vk_aSubpassDescs[1].colorAttachmentCount);
		const VkSubpassDependency2 vk_aSubpassDependencies[] = {
			{
				.sType = VK_STRUCTURE_TYPE_SUBPASS_DEPENDENCY_2,
				.pNext = nullptr,
				.srcSubpass = VK_SUBPASS_EXTERNAL,
				.dstSubpass = RENDER_SUBPASS_WINDOW_FRAME,
				.srcStageMask = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
				.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
				.srcAccessMask = VK_ACCESS_NONE,
				.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
				.dependencyFlags = 0,
				.viewOffset = 0
			}, {
				.sType = VK_STRUCTURE_TYPE_SUBPASS_DEPENDENCY_2,
				.pNext = nullptr,
				.srcSubpass = RENDER_SUBPASS_WINDOW_FRAME,
				.dstSubpass = RENDER_SUBPASS_SCENERY,
				.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
				.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT,
				.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
				.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
				.dependencyFlags = 0,
				.viewOffset = 0
			}, {
				.sType = VK_STRUCTURE_TYPE_SUBPASS_DEPENDENCY_2,
				.pNext = nullptr,
				.srcSubpass = RENDER_SUBPASS_SCENERY,
				.dstSubpass = VK_SUBPASS_EXTERNAL,
				.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT,
				.dstStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
				.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
				.dstAccessMask = VK_ACCESS_NONE,
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
			.dependencyCount = sizeof(vk_aSubpassDependencies) / sizeof(vk_aSubpassDependencies[0]),
			.pDependencies = vk_aSubpassDependencies,
			.correlatedViewMaskCount = 0,
			.pCorrelatedViewMasks = nullptr
		};
		if (vkCreateRenderPass2(vk_hDevice, &vk_renderPassCreateInfo, nullptr, &vk_hRenderPass) == VK_SUCCESS) {
			return true;
		} else
			RE_FATAL_ERROR("Failed to create Vulkan render pass");
		return false;
	}

	void destroy_render_pass() {
		PRINT_DEBUG("Destroying Vulkan render pass used for rendering");
		vkDestroyRenderPass(vk_hDevice, vk_hRenderPass, nullptr);
	}

}
