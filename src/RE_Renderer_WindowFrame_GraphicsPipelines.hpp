#ifndef __RE_RENDERER_WINDOW_FRAME_GRAPHICS_PIPELINES_H__
#define __RE_RENDERER_WINDOW_FRAME_GRAPHICS_PIPELINES_H__ 1

#include "RE_Renderer_WindowFrame_Internal.hpp"

namespace RE {

#ifdef RENDERER_INCLUDE_WINDOW_FRAME

	extern VkPipelineLayout vk_hWindowFramePipelineLayout;
	extern VkPipeline vk_hWindowFrameGraphicsPipeline;

	bool create_window_frame_graphics_pipelines();
	void destroy_window_frame_graphics_pipelines();

#endif

}

#endif /* __RE_RENDERER_WINDOW_FRAME_GRAPHICS_PIPELINES_H__ */
