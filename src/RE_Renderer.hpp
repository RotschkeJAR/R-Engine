#ifndef __RE_RENDERER_H__
#define __RE_RENDERER_H__

#include "RE_Vulkan.hpp"

namespace RE {

#define RE_VK_RENDERABLE_RECTANGLES_COUNT 16384U
#define RE_VK_FRAMES_IN_FLIGHT 2U

	extern Camera *pActiveCamera;
	extern VkViewport vk_cameraViewportArea;
	extern VkRect2D vk_cameraScissorArea;

	extern VkBuffer vk_rectIndexBuffer;

	extern VkRenderPass vk_hWorldRenderPass;
	
	bool init_renderer();
	void destroy_renderer();
	void render();
	void calculate_render_area();

}

#endif /* __RE_RENDERER_H__ */
