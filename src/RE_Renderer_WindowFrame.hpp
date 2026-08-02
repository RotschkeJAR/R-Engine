#ifndef __RE_RENDERER_WINDOW_FRAME_H__
#define __RE_RENDERER_WINDOW_FRAME_H__ 1

#include "RE_Renderer_Internal.hpp"
#include "RE_Window.hpp"

namespace RE {

#ifdef RENDERER_INCLUDE_WINDOW_FRAME

	bool init_window_frame();
	void destroy_window_frame();
	void render_window_frame(VkCommandBuffer vk_hCommandBuffer);

#else

	consteval bool init_window_frame() {
		return true;
	}
	consteval void destroy_window_frame() {}
	inline void render_window_frame(VkCommandBuffer vk_hCommandBuffer) {}

#endif

}

#endif /* __RE_RENDERER_WINDOW_FRAME_H__ */
