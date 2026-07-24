#include "RE_Renderer_SettingsGUI_RenderPass.hpp"

namespace RE {

	VkRenderPass vk_hSettingsRenderPass;

	bool create_settings_gui_renderpass() {
		PRINT_DEBUG("Creating Vulkan render pass ", vk_hSettingsRenderPass, " for rendering settings GUI");
		const VkAttachmentDescription vk_aAttachmentDescs[] = {
			{
				.flags = 0,
				.format = vk_eSwapchainImageFormat,
				.samples = VK_SAMPLE_COUNT_1_BIT,
				.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
				.storeOp = VK_ATTACHMENT_STORE_OP_STORE,
				.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
				.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
				.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
				.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR
			}
		};
		const VkAttachmentReference vk_aaAttachmentRefs[1][1] = {
			{
				{
					.attachment = 0,
					.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
				}
			}
		};
		const VkSubpassDescription vk_aSubpassDescs[] = {
			{
				.flags = 0,
				.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,
				.inputAttachmentCount = 0,
				.pInputAttachments = nullptr,
				.colorAttachmentCount = sizeof(vk_aaAttachmentRefs[0]) / sizeof(vk_aaAttachmentRefs[0][0]),
				.pColorAttachments = vk_aaAttachmentRefs[0],
				.pResolveAttachments = nullptr,
				.pDepthStencilAttachment = nullptr,
				.preserveAttachmentCount = 0,
				.pPreserveAttachments = nullptr
			}
		};
		const VkSubpassDependency vk_aDependencies[] = {
			{
				.srcSubpass = VK_SUBPASS_EXTERNAL,
				.dstSubpass = 0,
				.srcStageMask = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
				.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
				.srcAccessMask = VK_ACCESS_NONE,
				.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
				.dependencyFlags = 0
			}, {
				.srcSubpass = 0,
				.dstSubpass = VK_SUBPASS_EXTERNAL,
				.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
				.dstStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
				.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
				.dstAccessMask = VK_ACCESS_NONE,
				.dependencyFlags = 0
			}
		};
		const VkRenderPassCreateInfo vk_renderPassCreateInfo = {
			.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
			.pNext = nullptr,
			.flags = 0,
			.attachmentCount = sizeof(vk_aAttachmentDescs) / sizeof(vk_aAttachmentDescs[0]),
			.pAttachments = vk_aAttachmentDescs,
			.subpassCount = sizeof(vk_aSubpassDescs) / sizeof(vk_aSubpassDescs[0]),
			.pSubpasses = vk_aSubpassDescs,
			.dependencyCount = sizeof(vk_aDependencies) / sizeof(vk_aDependencies[0]),
			.pDependencies = vk_aDependencies
		};
		if (vkCreateRenderPass(vk_hDevice, &vk_renderPassCreateInfo, nullptr, &vk_hSettingsRenderPass) == VK_SUCCESS) {
			return true;
		}
		return false;
	}

	void destroy_settings_gui_renderpass() {
		PRINT_DEBUG("Destroying Vulkan render pass ", vk_hSettingsRenderPass, " used for rendering settings GUI");
		vkDestroyRenderPass(vk_hDevice, vk_hSettingsRenderPass, nullptr);
	}

}
