#ifndef __RE_RENDERER_H__
#define __RE_RENDERER_H__

#include "RE_Render System.hpp"

namespace RE {

#define RE_VK_RENDERABLE_RECTANGLES_COUNT 16384U
#define RE_VK_FRAMES_IN_FLIGHT 2U

#define RE_VK_GAME_OBJECT_SUPBASS 0U

	extern Camera *pActiveCamera;
	extern VkViewport vk_cameraViewportArea;
	extern VkRect2D vk_cameraScissorArea;

	extern VkBuffer vk_hRectIndexBuffer;
	extern VkDeviceMemory vk_hRectIndexBufferMemory;

	extern VkRenderPass vk_hWorldRenderPass;
	extern VkFramebuffer vk_ahWorldFramebuffers[RE_VK_FRAMES_IN_FLIGHT];
	
	bool init_renderer();
	void destroy_renderer();
	void render();
	bool swapchain_created_renderer();
	void swapchain_destroyed_renderer();
	void calculate_render_area();

}

#endif /* __RE_RENDERER_H__ */
