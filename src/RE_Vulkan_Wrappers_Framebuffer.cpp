#include "RE_Vulkan_Wrappers.hpp"
#include "RE_Console.hpp"

namespace RE {
	
	bool create_vulkan_framebuffer(const VkFramebufferCreateFlags vk_eFlags, 
			const VkRenderPass vk_hRenderPass, 
			const uint32_t u32AttachmentCount, 
			const VkImageView *const vk_pahAttachments, 
			const uint32_t u32Width, 
			const uint32_t u32Height, 
			const uint32_t u32Layers, 
			VkFramebuffer *const vk_phFramebuffer) {
		PRINT_DEBUG("Creating a Vulkan framebuffer");
		const VkFramebufferCreateInfo vk_createInfo = {
			.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
			.flags = vk_eFlags,
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
	
	Vulkan_Framebuffer::Vulkan_Framebuffer(const VkFramebufferCreateFlags vk_eFlags, 
			const VkRenderPass vk_hRenderPass, 
			const uint32_t u32AttachmentCount, 
			const VkImageView *const vk_pahAttachments, 
			const uint32_t u32Width, 
			const uint32_t u32Height, 
			const uint32_t u32Layers) : Vulkan_Framebuffer() {
		PRINT_DEBUG_CLASS("Constructing Vulkan framebuffer wrapper");
		create(vk_eFlags, vk_hRenderPass, u32AttachmentCount, vk_pahAttachments, u32Width, u32Height, u32Layers);
	}
	
	Vulkan_Framebuffer::Vulkan_Framebuffer(Vulkan_Framebuffer &&rrCopy) : vk_hFramebuffer(rrCopy.vk_hFramebuffer) {
		PRINT_DEBUG_CLASS("Constructing Vulkan framebuffer wrapper by moving ownership from another wrapper");
		rrCopy.vk_hFramebuffer = VK_NULL_HANDLE;
	}

	Vulkan_Framebuffer::~Vulkan_Framebuffer() {
		PRINT_DEBUG_CLASS("Destructing Vulkan framebuffer wrapper");
		if (valid())
			destroy();
	}

	bool Vulkan_Framebuffer::create(const VkFramebufferCreateFlags vk_eFlags, 
			const VkRenderPass vk_hRenderPass, 
			const uint32_t u32AttachmentCount, 
			const VkImageView *const vk_pahAttachments, 
			const uint32_t u32Width, 
			const uint32_t u32Height, 
			const uint32_t u32Layers) {
#ifndef RE_DISABLE_PRINT_DEBUGS
		if (valid())
			RE_ERROR("Creating another Vulkan framebuffer wrapper, when the old one hasn't been destroyed yet");
#endif
		PRINT_DEBUG_CLASS("Creating Vulkan framebuffer wrapper");
		return create_vulkan_framebuffer(vk_eFlags, vk_hRenderPass, u32AttachmentCount, vk_pahAttachments, u32Width, u32Height, u32Layers, &vk_hFramebuffer);
	}
	
	void Vulkan_Framebuffer::destroy() {
		PRINT_DEBUG_CLASS("Destroying Vulkan framebuffer wrapper");
		vkDestroyFramebuffer(vk_hDevice, vk_hFramebuffer, nullptr);
		vk_hFramebuffer = VK_NULL_HANDLE;
	}

	[[nodiscard]]
	VkFramebuffer Vulkan_Framebuffer::get() const noexcept {
		return vk_hFramebuffer;
	}

	[[nodiscard]]
	const VkFramebuffer* Vulkan_Framebuffer::get_ptr() const noexcept {
		return &vk_hFramebuffer;
	}
	
	[[nodiscard]]
	bool Vulkan_Framebuffer::valid() const noexcept {
		return vk_hFramebuffer != VK_NULL_HANDLE;
	}

	[[nodiscard]]
	Vulkan_Framebuffer::operator VkFramebuffer() const {
		return get();
	}

	[[nodiscard]]
	Vulkan_Framebuffer::operator const VkFramebuffer*() const {
		return get_ptr();
	}
	
	[[nodiscard]]
	Vulkan_Framebuffer::operator bool() const {
		return valid();
	}

}
