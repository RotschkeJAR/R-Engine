#include "RE_Renderer_Framebuffers.hpp"

namespace RE {

	std::unique_ptr<VkFramebuffer[]> std_framebuffers;

	bool create_renderer_framebuffers() {
		std_framebuffers = std::make_unique<VkFramebuffer[]>(u32SwapchainImageCount);
		VkImageView vk_aAttachments[] = {
			VK_NULL_HANDLE,
			vk_ahRenderTargetImageViews[0],
			vk_ahDepthStencilImageViews[0],
			vk_ahSinglesampledImageViews[0]
		};
		const VkFramebufferCreateInfo vk_framebufferCreateInfo = {
			.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
			.pNext = nullptr,
			.flags = 0,
			.renderPass = vk_hRenderPass,
			.attachmentCount = 3U + bool_to_int(vk_hSinglesampledImage != VK_NULL_HANDLE),
			.pAttachments = vk_aAttachments,
			.width = vk_swapchainResolution.width,
			.height = vk_swapchainResolution.height,
			.layers = 1
		};
		uint32_t u32FramebufferCreateIndex;
		for (u32FramebufferCreateIndex = 0; u32FramebufferCreateIndex < u32SwapchainImageCount; u32FramebufferCreateIndex++) {
			vk_aAttachments[0] = swapchainImageViews[u32FramebufferCreateIndex];
			if (vkCreateFramebuffer(vk_hDevice, &vk_framebufferCreateInfo, nullptr, &std_framebuffers[u32FramebufferCreateIndex]) == VK_SUCCESS) {
				continue;
			}
			break;
		}
		for (uint32_t u32FramebufferDestroyIndex = 0; u32FramebufferDestroyIndex < u32FramebufferCreateIndex; u32FramebufferDestroyIndex++)
			vkDestroyFramebuffer(vk_hDevice, std_framebuffers[u32FramebufferDestroyIndex], nullptr);
		std_framebuffers.reset();
		return false;
	}

	void destroy_renderer_framebuffers() {
		for (uint32_t u32FramebufferDestroyIndex = 0; u32FramebufferDestroyIndex < u32SwapchainImageCount; u32FramebufferDestroyIndex++)
			vkDestroyFramebuffer(vk_hDevice, std_framebuffers[u32FramebufferDestroyIndex], nullptr);
		std_framebuffers.reset();
	}

}
