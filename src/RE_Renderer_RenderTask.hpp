#ifndef __RE_RENDERER_RENDER_TASK_H__
#define __RE_RENDERER_RENDER_TASK_H__

#include "RE_Renderer_Internal.hpp"

namespace RE {
	
#define RENDER_TASK_SUBINDEX_BUFFER_TRANSFER    0
#define RENDER_TASK_SUBINDEX_PROCESSING         1
#define RENDER_TASK_SUBINDEX_RENDERING          2
#define RENDER_TASK_SUBINDEX_IMAGE_BLIT         3

	extern VulkanTask aRenderTasks[RE_VK_FRAMES_IN_FLIGHT];
	extern VkFence vk_ahRenderFences[RE_VK_FRAMES_IN_FLIGHT];
	extern uint8_t u8CurrentFrameInFlightIndex;

	bool create_render_tasks();
	void destroy_render_tasks();

}

#endif /* __RE_RENDERER_RENDER_TASK_H__ */
