#ifndef __RE_RENDERER_RENDER_PASS_H__
#define __RE_RENDERER_RENDER_PASS_H__ 1

#include "RE_Renderer_Internal.hpp"

namespace RE {

#define RENDER_SUBPASS_WINDOW_FRAME  0
#define RENDER_SUBPASS_SCENERY       1

	extern VkRenderPass vk_hRenderPass;

	bool create_render_pass();
	void destroy_render_pass();

}

#endif /* __RE_RENDERER_RENDER_PASS_H__ */
