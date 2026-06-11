#ifndef __RE_RENDERER_H__
#define __RE_RENDERER_H__

#include "RE_GPU.hpp"

#include "RE_Renderer_Shader Structs.hpp"

namespace RE {

#define RE_VK_FRAMES_IN_FLIGHT 2

	extern float f32SampleShadingRate;
	extern uint8_t u8CurrentFrameInFlightIndex;

	bool init_renderer();
	void destroy_renderer();
	void render();
	bool wait_for_rendering_finished();

	// Swapchain
	void mark_swapchain_dirty();

	// Presentation
	extern uint32_t u32IndexToSelectedSurfaceFormat;

	// Depth-stencil buffers
	void find_suitable_depth_stencil_formats();

	// Textures
	extern VkDescriptorSet vk_hTextureDescSet;

	// MSAA
	extern VkSampleCountFlagBits vk_eMsaaCount;

#ifdef RE_OS_LINUX
	extern WindowFrameUniformData *pWindowFrameUniformData;
	extern VkDrawIndirectCommand *pIndirectDrawWindowTitle;
#endif

}

#endif /* __RE_RENDERER_H__ */
