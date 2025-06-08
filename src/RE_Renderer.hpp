#ifndef __RE_RENDERER_H__
#define __RE_RENDERER_H__

#include "RE_Render System.hpp"

namespace RE {

#define RE_VK_RENDERABLE_RECTANGLES_COUNT 16384U
#define RE_VK_FRAMES_IN_FLIGHT 2U

#define RE_VK_GAME_OBJECT_SUPBASS 0U

#define RE_VK_SEMAPHORES_PER_FRAME_COUNT 3U
#define RE_VK_RENDER_SEMAPHORE_COUNT (RE_VK_FRAMES_IN_FLIGHT * RE_VK_SEMAPHORES_PER_FRAME_COUNT)

	extern Camera *pActiveCamera;
	extern VkViewport vk_cameraViewportArea;
	extern VkRect2D vk_cameraScissorArea;

	extern VkBuffer vk_hRectIndexBuffer;
	extern VkDeviceMemory vk_hRectIndexBufferMemory;

	extern VkRenderPass vk_hWorldRenderPass;
	extern VkFramebuffer vk_ahWorldFramebuffers[RE_VK_FRAMES_IN_FLIGHT];

	extern VkSemaphore vk_ahRenderSemaphores[RE_VK_RENDER_SEMAPHORE_COUNT];

	extern uint8_t u8CurrentFrameInFlightIndex;
	
	bool init_renderer();
	void destroy_renderer();
	void render();
	bool swapchain_created_renderer();
	void swapchain_destroyed_renderer();
	void calculate_render_area();

}

#endif /* __RE_RENDERER_H__ */
