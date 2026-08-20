#include "RE_Renderer_Framebuffers.hpp"
#include "RE_Settings.hpp"

namespace RE {

	VkFramebuffer vk_ahFramebuffers[RE_VK_FRAMES_IN_FLIGHT];
	std::unique_ptr<VkFramebuffer[]> std_swapchainFramebuffers;

	bool create_renderer_framebuffers() {
		if (bUseDynamicRenderPass)
			return true;
		PRINT_DEBUG("Creating Vulkan framebuffers");
		VkImageView vk_ahAttachments[3];
		VkFramebufferCreateInfo vk_framebufferCreateInfo = {
			.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
			.pNext = nullptr,
			.flags = 0,
			.renderPass = vk_hRenderPass,
			.attachmentCount = 2U + bool_to_int(IS_SINGLESAMPLED_IMAGE_REQUIRED()),
			.pAttachments = vk_ahAttachments,
			.width = renderImageSize[0],
			.height = renderImageSize[1],
			.layers = 1
		};
		unsigned uFramebufferCreateIndex;
		for (uFramebufferCreateIndex = 0; uFramebufferCreateIndex < RE_VK_FRAMES_IN_FLIGHT; uFramebufferCreateIndex++) {
			PRINT_DEBUG("Creating Vulkan framebuffer for renderable image at frame-in-flight index ", uFramebufferCreateIndex);
			vk_ahAttachments[0] = vk_ahRenderTargetImageViews[uFramebufferCreateIndex];
			vk_ahAttachments[1] = vk_ahDepthStencilImageViews[uFramebufferCreateIndex];
			vk_ahAttachments[2] = vk_ahSinglesampledImageViews[uFramebufferCreateIndex];
			if (vkCreateFramebuffer(vk_hDevice, &vk_framebufferCreateInfo, nullptr, &vk_ahFramebuffers[uFramebufferCreateIndex]) == VK_SUCCESS) {
				continue;
			} else
				RE_FATAL_ERROR("Failed creating Vulkan framebuffer for renderable images at frame-in-flight index ", uFramebufferCreateIndex, " and render pass ", vk_hRenderPass);
			break;
		}
		if (uFramebufferCreateIndex == RE_VK_FRAMES_IN_FLIGHT) {
			std_swapchainFramebuffers = std::make_unique<VkFramebuffer[]>(u32SwapchainImageCount);
			vk_framebufferCreateInfo.renderPass = vk_hSwapchainRenderPass;
			vk_framebufferCreateInfo.attachmentCount = 1;
			vk_framebufferCreateInfo.width = vk_swapchainResolution.width;
			vk_framebufferCreateInfo.height = vk_swapchainResolution.height;
			uint32_t u32FramebufferCreateIndex;
			for (u32FramebufferCreateIndex = 0; u32FramebufferCreateIndex < u32SwapchainImageCount; u32FramebufferCreateIndex++) {
				PRINT_DEBUG("Creating Vulkan framebuffer for swapchain image at index ", u32FramebufferCreateIndex);
				vk_ahAttachments[0] = std_swapchainImageViews[u32FramebufferCreateIndex];
				if (vkCreateFramebuffer(vk_hDevice, &vk_framebufferCreateInfo, nullptr, &std_swapchainFramebuffers[u32FramebufferCreateIndex]) == VK_SUCCESS) {
					continue;
				} else
					RE_FATAL_ERROR("Failed creating Vulkan framebuffer for swapchain image at index ", u32FramebufferCreateIndex, " and render pass ", vk_hSwapchainRenderPass);
				break;
			}
			if (u32FramebufferCreateIndex == u32SwapchainImageCount) {
				return true;
			}
			PRINT_DEBUG("Destroying Vulkan framebuffers used for swapchain images due to failure creating all");
			for (uint32_t u32FramebufferDestroyIndex = 0; u32FramebufferDestroyIndex < u32FramebufferCreateIndex; u32FramebufferDestroyIndex++)
				vkDestroyFramebuffer(vk_hDevice, std_swapchainFramebuffers[u32FramebufferDestroyIndex], nullptr);
			std_swapchainFramebuffers.reset();
		}
		PRINT_DEBUG("Destroying Vulkan framebuffers used for renderable images due to failure creating them all");
		for (unsigned uFramebufferDestroyIndex = 0; uFramebufferDestroyIndex < uFramebufferCreateIndex; uFramebufferDestroyIndex++) {
			vkDestroyFramebuffer(vk_hDevice, vk_ahFramebuffers[uFramebufferDestroyIndex], nullptr);
			vk_ahFramebuffers[uFramebufferDestroyIndex] = VK_NULL_HANDLE;
		}
		return false;
	}

	void destroy_renderer_framebuffers() {
		if (bUseDynamicRenderPass)
			return;
		PRINT_DEBUG("Destroying all Vulkan framebuffers");
		for (uint32_t u32FramebufferDestroyIndex = 0; u32FramebufferDestroyIndex < u32SwapchainImageCount; u32FramebufferDestroyIndex++)
			vkDestroyFramebuffer(vk_hDevice, std_swapchainFramebuffers[u32FramebufferDestroyIndex], nullptr);
		std_swapchainFramebuffers.reset();
		for (VkFramebuffer &vk_rhFramebuffer : vk_ahFramebuffers)
			vkDestroyFramebuffer(vk_hDevice, vk_rhFramebuffer, nullptr);
	}

}
