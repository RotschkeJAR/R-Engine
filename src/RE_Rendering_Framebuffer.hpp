#ifndef __RE_RENDERING_FRAMEBUFFER_H__
#define __RE_RENDERING_FRAMEBUFFER_H__

#include "RE_Vulkan.hpp"
#include "RE_Rendering_Render Pass.hpp"

namespace RE {
	
	class Rendering_Framebuffer {
		private:
			VkFramebuffer vk_hFramebuffer;

		public:
			Rendering_Framebuffer() = delete;
			Rendering_Framebuffer(const Rendering_RenderPass *pRenderPass, const uint32_t u32ImageViewAttachmentCount, const VkImageView *vk_pImageViewAttachments, const uint32_t u32Width, const uint32_t u32Height);
			~Rendering_Framebuffer();
			VkFramebuffer get_frmebuffer() const;
			bool is_valid() const;

			operator VkFramebuffer() const;
	};

}

#endif /* __RE_RENDERING_FRAMEBUFFER_H__ */
