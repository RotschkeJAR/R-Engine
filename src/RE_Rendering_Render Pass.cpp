#include "RE_Rendering_Render Pass.hpp"
#include "RE_Render System.hpp"

namespace RE {
	
	Rendering_RenderPass::Rendering_RenderPass() : vk_hRenderPass(VK_NULL_HANDLE) {
		VkAttachmentDescription vk_colorAttachment = {};
		vk_colorAttachment.format = vk_eSwapchainImageFormat;
		vk_colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
		vk_colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		vk_colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		vk_colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		vk_colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		vk_colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		vk_colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

		VkAttachmentReference vk_attachmentReference = {};
		vk_attachmentReference.attachment = 0U;
		vk_attachmentReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		VkSubpassDescription vk_subpassDescription = {};
		vk_subpassDescription.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		vk_subpassDescription.colorAttachmentCount = 1U;
		vk_subpassDescription.pColorAttachments = &vk_attachmentReference;

		VkSubpassDependency vk_subpassDependency = {};
		vk_subpassDependency.srcSubpass = VK_SUBPASS_EXTERNAL;
		vk_subpassDependency.dstSubpass = 0;
		vk_subpassDependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		vk_subpassDependency.srcAccessMask = 0U;
		vk_subpassDependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		vk_subpassDependency.dstAccessMask = 0U;

		VkRenderPassCreateInfo vk_renderPassCreateInfo = {};
		vk_renderPassCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		vk_renderPassCreateInfo.attachmentCount = 1U;
		vk_renderPassCreateInfo.pAttachments = &vk_colorAttachment;
		vk_renderPassCreateInfo.subpassCount = 1U;
		vk_renderPassCreateInfo.pSubpasses = &vk_subpassDescription;
		vk_renderPassCreateInfo.dependencyCount = 1U;
		vk_renderPassCreateInfo.pDependencies = &vk_subpassDependency;
		if (!CHECK_VK_RESULT(vkCreateRenderPass(vk_hDevice, &vk_renderPassCreateInfo, nullptr, &vk_hRenderPass)))
			vk_hRenderPass = VK_NULL_HANDLE;
	}

	Rendering_RenderPass::~Rendering_RenderPass() {
		if (!is_valid())
			return;
		CATCH_SIGNAL(vkDestroyRenderPass(vk_hDevice, vk_hRenderPass, nullptr));
	}

	VkRenderPass Rendering_RenderPass::get_render_pass() const {
		return vk_hRenderPass;
	}

	bool Rendering_RenderPass::is_valid() const {
		return vk_hRenderPass != VK_NULL_HANDLE;
	}

	Rendering_RenderPass::operator VkRenderPass() const {
		return this->get_render_pass();
	}

}
