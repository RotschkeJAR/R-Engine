#ifndef __RE_RENDERER_BUFFERS_H__
#define __RE_RENDERER_BUFFERS_H__

#include "RE_Renderer_Internal.hpp"

namespace RE {

	extern RawGameObjectShaderData *apaGameObjectBufferInstanceData[RE_VK_FRAMES_IN_FLIGHT];
	extern VkBuffer vk_ahRawGameObjectBuffers[RE_VK_FRAMES_IN_FLIGHT];

	extern VkBuffer vk_ahCameraBuffers[RE_VK_FRAMES_IN_FLIGHT];
	extern VkDeviceSize vk_aCameraBufferMemoryOffsets[RE_VK_FRAMES_IN_FLIGHT];
	extern VulkanMemory *apCameraBufferMemories[RE_VK_FRAMES_IN_FLIGHT];

	extern VkBuffer vk_ahSortableDepthBuffers[RE_VK_FRAMES_IN_FLIGHT];

	extern VkBuffer vk_ahGameObjectBuffers[RE_VK_FRAMES_IN_FLIGHT];

	bool create_renderer_buffers();
	void destroy_renderer_buffers();

	bool record_cmd_transfer_buffer();

}

#endif /* __RE_RENDERER_BUFFERS_H__ */
