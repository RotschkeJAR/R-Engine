#ifndef __RE_VULKAN_FRAMEBUFFER_H__
#define __RE_VULKAN_FRAMEBUFFER_H__

#include "RE_Vulkan.hpp"
#include "RE_Vulkan_Render Pass.hpp"

namespace RE {
	
	class Vulkan_Framebuffer final {
		private:
			VkFramebuffer vk_hFramebuffer;

		public:
			Vulkan_Framebuffer() = delete;
			Vulkan_Framebuffer(const Vulkan_RenderPass *pRenderPass, const uint32_t u32ImageViewAttachmentCount, const VkImageView *vk_pImageViewAttachments, const uint32_t u32Width, const uint32_t u32Height);
			~Vulkan_Framebuffer();
			VkFramebuffer get_frmebuffer() const;
			bool is_valid() const;

			operator VkFramebuffer() const;
	};

}

#endif /* __RE_VULKAN_FRAMEBUFFER_H__ */
