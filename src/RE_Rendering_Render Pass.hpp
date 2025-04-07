#ifndef __RE_RENDERING_RENDERPASS_H__
#define __RE_RENDERING_RENDERPASS_H__

#include "RE_Vulkan.hpp"

namespace RE {
	
	class Rendering_RenderPass {
		private:
			VkRenderPass vk_hRenderPass;

		public:
			Rendering_RenderPass();
			~Rendering_RenderPass();
			VkRenderPass get_render_pass() const;
			bool is_valid() const;

			operator VkRenderPass() const;
	};

}

#endif /* __RE_RENDERING_RENDERPASS_H__ */
