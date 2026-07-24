#ifndef __RE_RENDERER_WINDOW_FRAME_BUFFERS_H__
#define __RE_RENDERER_WINDOW_FRAME_BUFFERS_H__

#include "RE_Renderer_WindowFrame_Internal.hpp"

namespace RE {

	extern VkBuffer vk_hWindowFrameBuffer;
	extern VulkanMemory *pWindowFrameBufferMemory;

	bool create_window_frame_buffers();
	void destroy_window_frame_buffers();

}

#endif /* __RE_RENDERER_WINDOW_FRAME_BUFFERS_H__ */
