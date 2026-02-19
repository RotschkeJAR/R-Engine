#ifndef __RE_RENDERER_BUFFERS_H__
#define __RE_RENDERER_BUFFERS_H__

#include "RE_Renderer_Internal.hpp"

namespace RE {

	extern Vulkan_Buffer aRawGameObjectBuffers[RE_VK_FRAMES_IN_FLIGHT];

	extern VkBuffer vk_ahCameraBuffers[RE_VK_FRAMES_IN_FLIGHT];
	extern VulkanMemory cameraBuffersMemory;
	extern VkDeviceSize vk_aCameraBufferMemoryOffsets[RE_VK_FRAMES_IN_FLIGHT];

	extern Vulkan_Buffer aSortableDepthBuffers[RE_VK_FRAMES_IN_FLIGHT];

	extern Vulkan_Buffer aGameObjectBuffers[RE_VK_FRAMES_IN_FLIGHT];

	bool create_renderer_buffers();
	void destroy_renderer_buffers();

	bool record_cmd_transfer_buffer();

}

#endif /* __RE_RENDERER_BUFFERS_H__ */
