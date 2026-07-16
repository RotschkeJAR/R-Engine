#include "RE_Vulkan_Wrappers.hpp"
#include "RE_Console.hpp"

namespace RE {
	
	bool create_vulkan_framebuffer(
			VkFramebufferCreateFlags vk_mFlags, 
			VkRenderPass vk_hRenderPass, 
			uint32_t u32AttachmentCount, 
			const VkImageView *vk_pahAttachments, 
			uint32_t u32Width, 
			uint32_t u32Height, 
			uint32_t u32Layers, 
			VkFramebuffer *vk_phFramebuffer) {
		PRINT_DEBUG("Creating a Vulkan framebuffer");
		const VkFramebufferCreateInfo vk_createInfo = {
			.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
			.flags = vk_mFlags,
			.renderPass = vk_hRenderPass,
			.attachmentCount = u32AttachmentCount,
			.pAttachments = vk_pahAttachments,
			.width = u32Width,
			.height = u32Height,
			.layers = u32Layers
		};
		if (vkCreateFramebuffer(vk_hDevice, &vk_createInfo, nullptr, vk_phFramebuffer) == VK_SUCCESS)
			return true;
		RE_ERROR("Failed to create a Vulkan framebuffer");
		return false;
	}

	Vulkan_Framebuffer::Vulkan_Framebuffer() : vk_hFramebuffer(VK_NULL_HANDLE) {}
	
	Vulkan_Framebuffer::Vulkan_Framebuffer(
			VkFramebufferCreateFlags vk_mFlags, 
			VkRenderPass vk_hRenderPass, 
			uint32_t u32AttachmentCount, 
			const VkImageView *vk_pahAttachments, 
			uint32_t u32Width, 
			uint32_t u32Height, 
			uint32_t u32Layers) : Vulkan_Framebuffer() {
		PRINT_DEBUG_CLASS("Constructing Vulkan framebuffer wrapper");
		create(vk_mFlags, vk_hRenderPass, u32AttachmentCount, vk_pahAttachments, u32Width, u32Height, u32Layers);
	}
	
	Vulkan_Framebuffer::Vulkan_Framebuffer(Vulkan_Framebuffer &&rrCopy) : vk_hFramebuffer(rrCopy.vk_hFramebuffer) {
		PRINT_DEBUG_CLASS("Constructing Vulkan framebuffer wrapper by moving ownership from another wrapper");
		rrCopy.vk_hFramebuffer = VK_NULL_HANDLE;
	}

	Vulkan_Framebuffer::~Vulkan_Framebuffer() {
		PRINT_DEBUG_CLASS("Destructing Vulkan framebuffer wrapper");
		destroy();
	}

	bool Vulkan_Framebuffer::create(
			VkFramebufferCreateFlags vk_mFlags, 
			VkRenderPass vk_hRenderPass, 
			uint32_t u32AttachmentCount, 
			const VkImageView *vk_pahAttachments, 
			uint32_t u32Width, 
			uint32_t u32Height, 
			uint32_t u32Layers) {
	#ifndef NDEBUG
		if (valid())
			RE_ERROR("Creating another Vulkan framebuffer wrapper, when the old framebuffer ", vk_hFramebuffer, " hasn't been destroyed yet");
	#endif
		PRINT_DEBUG_CLASS("Creating Vulkan framebuffer wrapper");
		return create_vulkan_framebuffer(vk_mFlags, vk_hRenderPass, u32AttachmentCount, vk_pahAttachments, u32Width, u32Height, u32Layers, &vk_hFramebuffer);
	}
	
	void Vulkan_Framebuffer::destroy() {
		PRINT_DEBUG_CLASS("Destroying Vulkan framebuffer wrapper");
		vkDestroyFramebuffer(vk_hDevice, vk_hFramebuffer, nullptr);
		vk_hFramebuffer = VK_NULL_HANDLE;
	}

	VkFramebuffer Vulkan_Framebuffer::get() const {
		return vk_hFramebuffer;
	}

	const VkFramebuffer* Vulkan_Framebuffer::get_ptr() const {
		return &vk_hFramebuffer;
	}
	
	bool Vulkan_Framebuffer::valid() const {
		return vk_hFramebuffer != VK_NULL_HANDLE;
	}

	Vulkan_Framebuffer::operator VkFramebuffer() const {
		return get();
	}

	Vulkan_Framebuffer::operator const VkFramebuffer*() const {
		return get_ptr();
	}
	
	Vulkan_Framebuffer::operator bool() const {
		return valid();
	}

	VkFramebuffer Vulkan_Framebuffer::operator()() const {
		return get();
	}

}
