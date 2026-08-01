#ifndef __RE_RENDERER_H__
#define __RE_RENDERER_H__ 1

#include "RE_GPU.hpp"

#include "RE_Renderer_ShaderStructs.hpp"

namespace RE {

#define RE_VK_FRAMES_IN_FLIGHT   2

#define CHAR_TEXTURE_COUNT   0x5F
#define CHAR_TEXTURE_SIZE    8

	bool init_renderer();
	void destroy_renderer();
	void render();
	bool wait_for_rendering_finished();

	// Swapchain
	void mark_swapchain_dirty();

	// Presentation
	extern uint32_t u32IndexToSelectedSurfaceFormat;
	
	// Render Task
#ifdef RE_OS_LINUX
	bool prepare_render_tasks_for_dummy_presentation();
#endif

	// Depth-stencil images
	void find_suitable_depth_stencil_formats();

#ifdef RE_OS_LINUX
	extern WindowFrameUniformData *pWindowFrameUniformData;
	extern VkDrawIndirectCommand *pIndirectDrawWindowTitle;
#endif

	// Settings GUI
	bool init_settings_gui();
	void destroy_settings_gui();

}

#endif /* __RE_RENDERER_H__ */
