#ifndef __RE_VULKAN_WRAPPER_FUNCTIONS_H__
#define __RE_VULKAN_WRAPPER_FUNCTIONS_H__

#include "RE_Internal Header.hpp"

namespace RE {
	
	bool __vk_create_buffer(const VkBufferCreateFlags vk_eCreateFlags, const VkDeviceSize vk_bufferSizeInBytes, const VkBufferUsageFlags vk_bufferUsage, const uint32_t u32QueueTypeCount, const uint32_t *pu32BufferQueueTypeIndices, const VkMemoryPropertyFlags vk_memoryProperties, VkBuffer *vk_pBuffer, VkDeviceMemory *vk_pBufferMemory, const char *pFile, const char *pFunc, const uint32_t u32Line);
#define vk_create_buffer(SIZE, USAGE, QUEUES, QUEUE_COUNT, MEMORY_PROPERTY, BUFFER_OUT, MEMORY_OUT) CATCH_SIGNAL_AND_RETURN(__vk_create_buffer(0, SIZE, USAGE, QUEUES, QUEUE_COUNT, MEMORY_PROPERTY, BUFFER_OUT, MEMORY_OUT, __FILE__, __func__, __LINE__), bool)

	bool __vk_create_framebuffer(const VkFramebufferCreateFlags vk_eCreateFlags, const VkRenderPass vk_renderPass, const uint32_t u32ImageViewAttachmentCount, const VkImageView *vk_pImageViewAttachments, const uint32_t u32Width, const uint32_t u32Height, const uint32_t u32Layers, VkFramebuffer *vk_pFramebuffer, const char *pcFile, const char *pcFunc, const uint32_t u32Line);
#define vk_create_framebuffer(RENDERPASS, IMGVIEW_ATTACH_COUNT, IMGVIEW_ATTACHS, WIDTH, HEIGHT, LAYERS, FRAMEBUFFER_OUT) CATCH_SIGNAL_AND_RETURN(__vk_create_framebuffer(0, RENDERPASS, IMGVIEW_ATTACH_COUNT, IMGVIEW_ATTACHS, WIDTH, HEIGHT, LAYERS, FRAMEBUFFER_OUT, __FILE__, __func__, __LINE__), bool)

	bool __vk_create_semaphore(const VkSemaphoreCreateFlags vk_eCreateFlags, const VkSemaphore *vk_pSemaphore, const char *pcFile, const char *pcFunc, const uint32_t u32Line);
#define vk_create_semaphore(SEMAPHORE_OUT) CATCH_SIGNAL_AND_RETURN(__vk_create_semaphore(0, SEMAPHORE_OUT, __FILE__, __func__, __LINE__), bool)

	bool __vk_create_fence(const VkFenceCreateFlags vk_eCreateFlags, VkFence *vk_pFence, const char *pcFile, const char *pcFunc, const uint32_t u32Line);
#define vk_create_fence(FLAGS, FENCE_OUT) CATCH_SIGNAL_AND_RETURN(__vk_create_fence(FLAGS, FENCE_OUT, __FILE__, __func__, __LINE__), bool)

	bool __vk_create_shader_from_file(const char *pcPathToFile, const VkShaderModuleCreateFlags vk_eCreateFlags, VkShaderModule *vk_pShaderModule, const char *pcFile, const char *pcFunc, const uint32_t u32Line);
#define vk_create_shader_from_file(FILEPATH, SHADER_OUT) CATCH_SIGNAL_AND_RETURN(__vk_create_shader_from_file(FILEPATH, 0, SHADER_OUT, __FILE__, __func__, __LINE__), bool)

	bool __vk_create_render_pass(const uint32_t u32AttachmentDescriptionCount, const VkAttachmentDescription *vk_pAttachmentDescriptions, const uint32_t u32SubpassDescriptionCount, const VkSubpassDescription *vk_pSubpassDescriptions, const uint32_t u32SubpassDependencyCount, const VkSubpassDependency *vk_pSubpassDependencies, VkRenderPass *vk_pRenderPass, const char *pcFile, const char *pcFunc, const uint32_t u32Line);
#define vk_create_render_pass(ATTACH_DESC_COUNT, ATTACH_DESCS, SUBPASS_DESC_COUNT, SUBPASS_DESCS, SUBPASS_DEP_COUNT, SUBPASS_DEPS, RENDERPASS_OUT) CATCH_SIGNAL_AND_RETURN(__vk_create_render_pass(ATTACH_DESC_COUNT, ATTACH_DESCS, SUBPASS_DESC_COUNT, SUBPASS_DESCS, SUBPASS_DEP_COUNT, SUBPASS_DEPS, RENDERPASS_OUT, __FILE__, __func__, __LINE__), bool)


	void __vk_wait_for_fence(const VkFence vk_fence);
#define vk_wait_for_fence(FENCE) CATCH_SIGNAL(__vk_wait_for_fence(FENCE))

	void __vk_reset_fence(const VkFence vk_fence);
#define vk_reset_fence(FENCE) CATCH_SIGNAL(__vk_reset_fence(FENCE))

	void __vk_wait_for_and_reset_fence(const VkFence vk_fence);
#define vk_wait_for_and_reset_fence(FENCE) CATCH_SIGNAL(__vk_wait_for_and_reset_fence(FENCE))

}

#endif /* __RE_VULKAN_WRAPPER_FUNCTIONS_H__ */
