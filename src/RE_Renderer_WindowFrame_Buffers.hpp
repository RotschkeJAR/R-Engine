#ifndef __RE_RENDERER_WINDOW_FRAME_BUFFERS_H__
#define __RE_RENDERER_WINDOW_FRAME_BUFFERS_H__ 1

#include "RE_Renderer_WindowFrame_Internal.hpp"

namespace RE {

#ifdef RENDERER_INCLUDE_WINDOW_FRAME

	extern VkBuffer vk_hWindowFrameBuffer;
	extern VulkanMemory *pWindowFrameBufferMemory;

	bool create_window_frame_buffers();
	void destroy_window_frame_buffers();

#endif

}

#endif /* __RE_RENDERER_WINDOW_FRAME_BUFFERS_H__ */
