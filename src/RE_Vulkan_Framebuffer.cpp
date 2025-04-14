#include "RE_Vulkan_Framebuffer.hpp"
#include "RE_Render System.hpp"

namespace RE {
	
	Vulkan_Framebuffer::Vulkan_Framebuffer(const Vulkan_RenderPass *pRenderPass, const uint32_t u32ImageViewAttachmentCount, const VkImageView *vk_pImageViewAttachments, const uint32_t u32Width, const uint32_t u32Height) : vk_hFramebuffer(VK_NULL_HANDLE) {
		VkFramebufferCreateInfo vk_framebufferCreateInfo = {};
		vk_framebufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		vk_framebufferCreateInfo.renderPass = *pRenderPass;
		vk_framebufferCreateInfo.attachmentCount = u32ImageViewAttachmentCount;
		vk_framebufferCreateInfo.pAttachments = vk_pImageViewAttachments;
		vk_framebufferCreateInfo.width = u32Width;
		vk_framebufferCreateInfo.height = u32Height;
		vk_framebufferCreateInfo.layers = 1U;
		if (!CHECK_VK_RESULT(vkCreateFramebuffer(vk_hDevice, &vk_framebufferCreateInfo, nullptr, &vk_hFramebuffer))) {
			RE_ERROR("Failed creating Vulkan framebuffer");
			vk_hFramebuffer = VK_NULL_HANDLE;
		}
	}
	
	Vulkan_Framebuffer::~Vulkan_Framebuffer() {
		if (!is_valid())
			return;
		CATCH_SIGNAL(vkDestroyFramebuffer(vk_hDevice, vk_hFramebuffer, nullptr));
	}
	
	VkFramebuffer Vulkan_Framebuffer::get_frmebuffer() const {
		return vk_hFramebuffer;
	}
	
	bool Vulkan_Framebuffer::is_valid() const {
		return vk_hFramebuffer != VK_NULL_HANDLE;
	}

	Vulkan_Framebuffer::operator VkFramebuffer() const {
		return this->vk_hFramebuffer;
	}

}
