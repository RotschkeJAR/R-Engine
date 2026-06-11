#ifndef __RE_RENDERER_BUFFERS_H__
#define __RE_RENDERER_BUFFERS_H__

#include "RE_Renderer_Internal.hpp"

namespace RE {

	extern VkBuffer vk_hStagingGameObjectsBuffer;
	extern VulkanMemory *pStagingGameObjectsBufferMemory;
	extern GameObjectShaderData *paStagingGameObjectsBufferData;

	extern VkBuffer vk_ahGameObjectsBuffers[RE_VK_FRAMES_IN_FLIGHT];

	extern VkBuffer vk_hCameraBuffer;
	extern VulkanMemory *pCameraBufferMemory;
	extern std::unique_ptr<CameraShaderData*[]> camerasShaderData;

	extern VkBuffer vk_ahSortableDepthBuffers[RE_VK_FRAMES_IN_FLIGHT];

	extern VkBuffer vk_ahGameObjectsModelMatrixBuffers[RE_VK_FRAMES_IN_FLIGHT];

#ifdef RE_OS_LINUX
	extern VkBuffer vk_hWindowFrameBuffer;
	extern VulkanMemory *pWindowFrameBufferMemory;
#endif

	bool create_renderer_buffers();
	void destroy_renderer_buffers();

}

#endif /* __RE_RENDERER_BUFFERS_H__ */
