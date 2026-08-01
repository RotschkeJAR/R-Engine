#ifndef __RE_RENDERER_RENDER_PASS_H__
#define __RE_RENDERER_RENDER_PASS_H__ 1

#include "RE_Renderer_Internal.hpp"

namespace RE {

	extern VkRenderPass vk_hRenderPass,
		vk_hSwapchainRenderPass;
	extern uint32_t u32SubpassScenery,
		u32SwapchainSubpassWindowFrame;

	bool create_render_pass();
	void destroy_render_pass();

}

#endif /* __RE_RENDERER_RENDER_PASS_H__ */
