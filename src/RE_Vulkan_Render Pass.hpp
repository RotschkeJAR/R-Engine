#ifndef __RE_VULKAN_RENDERPASS_H__
#define __RE_VULKAN_RENDERPASS_H__

#include "RE_Vulkan.hpp"

namespace RE {
	
	class Vulkan_RenderPass final {
		private:
			VkRenderPass vk_hRenderPass;

		public:
			Vulkan_RenderPass();
			~Vulkan_RenderPass();
			VkRenderPass get_render_pass() const;
			bool is_valid() const;

			operator VkRenderPass() const;
	};

}

#endif /* __RE_VULKAN_RENDERPASS_H__ */
