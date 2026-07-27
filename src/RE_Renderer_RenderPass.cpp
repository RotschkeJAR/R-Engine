#include "RE_Renderer_RenderPass.hpp"
#include "RE_Settings.hpp"

namespace RE {

	VkRenderPass vk_hRenderPass = VK_NULL_HANDLE,
		vk_hSwapchainRenderPass = VK_NULL_HANDLE;

	bool create_render_pass() {
		if (vk_hRenderTargetImage) {

		} else {
			const VkAttachmentDescription2 vk_aSubpassDescs[] = {
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
					.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR
				}, {
					.sType = VK_STRUCTURE_TYPE_ATTACHMENT_DESCRIPTION_2,
					.pNext = nullptr,
					.flags = 0,
					.format = vk_eSelectedDepthStencilFormat,
					.samples = VK_SAMPLE_COUNT_1_BIT,
					.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
					.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
					.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
					.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
					.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
					.finalLayout = bStencilsEnabled ? VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL : VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL
				}
			};
			VkRenderPassCreateInfo2 vk_renderPassCreateInfo = {
				.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
				.pNext = nullptr,
				.flags = 0,
				.attachmentCount = sizeof(vk_aSubpassDescs) / sizeof(vk_aSubpassDescs[0]),
				.pAttachments = vk_aSubpassDescs,
				.subpassCount = ,
				.pSubpasses = ,
				.dependencyCount = ,
				.pDependencies = ,
				.correlatedViewMaskCount = 0,
				.pCorrelatedViewMasks = nullptr
			};
			if (vkCreateRenderPass2(vk_hDevice, &vk_renderPassCreateInfo, nullptr, &vk_hSwapchainRenderPass)) {
				return true; 
			} else
				RE_FATAL_ERROR("");
		}
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
