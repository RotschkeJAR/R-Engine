#ifndef __RE_RENDERER_FRAMEBUFFERS_H__
#define __RE_RENDERER_FRAMEBUFFERS_H__ 1

#include "RE_Renderer_Internal.hpp"

namespace RE {

	extern VkFramebuffer vk_ahFramebuffers[RE_VK_FRAMES_IN_FLIGHT];
	extern std::unique_ptr<VkFramebuffer[]> std_swapchainFramebuffers;

	bool create_renderer_framebuffers();
	void destroy_renderer_framebuffers();

}

#endif /* __RE_RENDERER_FRAMEBUFFERS_H__ */
