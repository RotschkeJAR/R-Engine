#ifndef __RE_RENDERER_H__
#define __RE_RENDERER_H__

#include "RE_GPU.hpp"
#include "RE_Renderer_Shader Structs.hpp"

namespace RE {

#define RE_VK_FRAMES_IN_FLIGHT 2

	// Swapchain
	extern bool bSwapchainDirty;
	
	void mark_swapchain_dirty();

	// Presentation
	extern uint32_t u32IndexToSelectedSurfaceFormat;

	extern VkPipelineLayout vk_hWorldPipelineLayout;
	extern float f32SampleShadingRate;
	extern uint8_t u8CurrentFrameInFlightIndex;

	bool init_renderer();
	void destroy_renderer();
	void render();
	bool wait_for_rendering_finished();

	// Render Images
	void get_queues_for_swapchain_images(std::vector<uint32_t> &rRenderTaskQueueIndices);

	// Render buffer
	extern RawGameObjectShaderData *apaRawGameObjectBufferData[RE_VK_FRAMES_IN_FLIGHT];

	// Depth-stencil buffers
	void find_suitable_depth_stencil_formats();

	// Descriptor sets
#define RE_VK_UNIFORM_BINDING_CAMERA 0
#define RE_VK_UNIFORM_BINDING_TEXTURE 0

	// Textures
#define RE_VK_MAX_SAMPLED_IMAGES 0x7FFF
	extern VkDescriptorSet vk_hTextureDescSet;

	// MSAA
	extern VkSampleCountFlagBits vk_eMsaaCount;

}

#endif /* __RE_RENDERER_H__ */
