#ifndef __RE_VULKAN_WRAPPER_FUNCTIONS_H__
#define __RE_VULKAN_WRAPPER_FUNCTIONS_H__

#include "RE_Internal Header.hpp"

namespace RE {
	
	bool __vk_create_buffer(const VkBufferCreateFlags vk_eCreateFlags, const VkDeviceSize vk_bufferSizeInBytes, const VkBufferUsageFlags vk_bufferUsage, const uint32_t u32QueueTypeCount, const uint32_t *pu32BufferQueueTypeIndices, const VkMemoryPropertyFlags vk_memoryProperties, VkBuffer *vk_phBuffer, VkDeviceMemory *vk_phBufferMemory, const char *pFile, const char *pFunc, const uint32_t u32Line);
#define vk_create_buffer(SIZE, USAGE, QUEUE_COUNT, QUEUES, MEMORY_PROPERTY, BUFFER_OUT, MEMORY_OUT) CATCH_SIGNAL_AND_RETURN(__vk_create_buffer(0, SIZE, USAGE, QUEUE_COUNT, QUEUES, MEMORY_PROPERTY, BUFFER_OUT, MEMORY_OUT, __FILE__, __func__, __LINE__), bool)

	bool __vk_create_framebuffer(const VkFramebufferCreateFlags vk_eCreateFlags, const VkRenderPass vk_hRenderPass, const uint32_t u32ImageViewAttachmentCount, const VkImageView *vk_phImageViewAttachments, const uint32_t u32Width, const uint32_t u32Height, const uint32_t u32Layers, VkFramebuffer *vk_phFramebuffer, const char *pcFile, const char *pcFunc, const uint32_t u32Line);
#define vk_create_framebuffer(RENDERPASS, IMGVIEW_ATTACH_COUNT, IMGVIEW_ATTACHS, WIDTH, HEIGHT, LAYERS, FRAMEBUFFER_OUT) CATCH_SIGNAL_AND_RETURN(__vk_create_framebuffer(0, RENDERPASS, IMGVIEW_ATTACH_COUNT, IMGVIEW_ATTACHS, WIDTH, HEIGHT, LAYERS, FRAMEBUFFER_OUT, __FILE__, __func__, __LINE__), bool)

	bool __vk_create_semaphore(const VkSemaphoreCreateFlags vk_eCreateFlags, VkSemaphore *vk_phSemaphore, const char *pcFile, const char *pcFunc, const uint32_t u32Line);
#define vk_create_semaphore(SEMAPHORE_OUT) CATCH_SIGNAL_AND_RETURN(__vk_create_semaphore(0, SEMAPHORE_OUT, __FILE__, __func__, __LINE__), bool)

	bool __vk_create_fence(const VkFenceCreateFlags vk_eCreateFlags, VkFence *vk_phFence, const char *pcFile, const char *pcFunc, const uint32_t u32Line);
#define vk_create_fence(FLAGS, FENCE_OUT) CATCH_SIGNAL_AND_RETURN(__vk_create_fence(FLAGS, FENCE_OUT, __FILE__, __func__, __LINE__), bool)

	bool __vk_create_shader_from_file(const char *pcPathToFile, const VkShaderModuleCreateFlags vk_eCreateFlags, VkShaderModule *vk_phShaderModule, const char *pcFile, const char *pcFunc, const uint32_t u32Line);
#define vk_create_shader_from_file(FILEPATH, SHADER_OUT) CATCH_SIGNAL_AND_RETURN(__vk_create_shader_from_file(FILEPATH, 0, SHADER_OUT, __FILE__, __func__, __LINE__), bool)

	bool __vk_create_render_pass(const uint32_t u32AttachmentDescriptionCount, const VkAttachmentDescription *vk_pAttachmentDescriptions, const uint32_t u32SubpassDescriptionCount, const VkSubpassDescription *vk_pSubpassDescriptions, const uint32_t u32SubpassDependencyCount, const VkSubpassDependency *vk_pSubpassDependencies, VkRenderPass *vk_phRenderPass, const char *pcFile, const char *pcFunc, const uint32_t u32Line);
#define vk_create_render_pass(ATTACH_DESC_COUNT, ATTACH_DESCS, SUBPASS_DESC_COUNT, SUBPASS_DESCS, SUBPASS_DEP_COUNT, SUBPASS_DEPS, RENDERPASS_OUT) CATCH_SIGNAL_AND_RETURN(__vk_create_render_pass(ATTACH_DESC_COUNT, ATTACH_DESCS, SUBPASS_DESC_COUNT, SUBPASS_DESCS, SUBPASS_DEP_COUNT, SUBPASS_DEPS, RENDERPASS_OUT, __FILE__, __func__, __LINE__), bool)

	bool __vk_create_descriptor_set_layout(const VkDescriptorSetLayoutCreateFlags vk_eCreateFlags, const uint32_t u32BindingCount, const VkDescriptorSetLayoutBinding *vk_pBindings, VkDescriptorSetLayout *vk_pDescriptorSetLayout, const char *pcFile, const char *pcFunc, const uint32_t u32Line);
#define vk_create_descriptor_set_layout(FLAGS, BINDING_COUNT, BINDINGS, DESC_SET_LAYOUT_OUT) CATCH_SIGNAL_AND_RETURN(__vk_create_descriptor_set_layout(FLAGS, BINDING_COUNT, BINDINGS, DESC_SET_LAYOUT_OUT, __FILE__, __func__, __LINE__), bool)

	bool __vk_create_pipeline_layout(const VkPipelineLayoutCreateFlags vk_eCreateFlags, const uint32_t u32DescriptorSetLayoutCount, const VkDescriptorSetLayout *vk_phDescriptorSetLayouts, const uint32_t u32PushConstantRangeCount, const VkPushConstantRange *vk_pPushConstantRanges, VkPipelineLayout *vk_phPipelineLayout, const char *pcFile, const char *pcFunc, const uint32_t u32Line);
#define vk_create_pipeline_layout(FLAGS, DESC_SET_LAYOUT_COUNT, DESC_SET_LAYOUTS, PUSH_CONST_RANGE_COUNT, PUSH_CONST_RANGES, PIPELINELAYOUT_OUT) CATCH_SIGNAL_AND_RETURN(__vk_create_pipeline_layout(FLAGS, DESC_SET_LAYOUT_COUNT, DESC_SET_LAYOUTS, PUSH_CONST_RANGE_COUNT, PUSH_CONST_RANGES, PIPELINELAYOUT_OUT, __FILE__, __func__, __LINE__), bool)

	bool __vk_create_descriptor_pool(const VkDescriptorPoolCreateFlags vk_eCreateFlags, const uint32_t u32MaxSets, const uint32_t u32PoolSizeCount, const VkDescriptorPoolSize *vk_pPoolSizes, VkDescriptorPool *vk_phDescriptorPool, const char *pcFile, const char *pcFunc, const uint32_t u32Line);
#define vk_create_descriptor_pool(FLAGS, MAX_SETS, POOL_SIZE_COUNT, POOL_SIZES, DESC_POOL_OUT) CATCH_SIGNAL_AND_RETURN(__vk_create_descriptor_pool(FLAGS, MAX_SETS, POOL_SIZE_COUNT, POOL_SIZES, DESC_POOL_OUT, __FILE__, __func__, __LINE__), bool)

	bool __vk_alloc_command_buffers(const VkCommandPool vk_hCommandPool, const VkCommandBufferLevel vk_eCommandBufferLevel, const uint32_t u32CommandBufferCount, VkCommandBuffer *vk_phCommandBuffers, const char *pcFile, const char *pcFunc, const uint32_t u32Line);
#define vk_alloc_command_buffers(CMDPOOL, CMDBUFFER_LEVEL, CMDBUFFER_COUNT, CMDBUFFERS_OUT) CATCH_SIGNAL_AND_RETURN(__vk_alloc_command_buffers(CMDPOOL, CMDBUFFER_LEVEL, CMDBUFFER_COUNT, CMDBUFFERS_OUT, __FILE__, __func__, __LINE__), bool)

	bool __vk_alloc_descriptor_sets(const VkDescriptorPool vk_hDescriptorPool, const uint32_t u32DescriptorSetCount, const VkDescriptorSetLayout *vk_phDescriptorSetLayouts, VkDescriptorSet *vk_phDescriptorSets, const char *pcFile, const char *pcFunc, const uint32_t u32Line);
#define vk_alloc_descriptor_sets(DESC_POOL, DESC_SET_COUNT, DESC_SET_LAYOUTS, DESC_SETS_OUT) CATCH_SIGNAL_AND_RETURN(__vk_alloc_descriptor_sets(DESC_POOL, DESC_SET_COUNT, DESC_SET_LAYOUTS, DESC_SETS_OUT, __FILE__, __func__, __LINE__), bool)


	void __vk_wait_for_fence(const VkFence *vk_phFence);
#define vk_wait_for_fence(FENCE) CATCH_SIGNAL(__vk_wait_for_fence(FENCE))

	void __vk_reset_fence(const VkFence *vk_phFence);
#define vk_reset_fence(FENCE) CATCH_SIGNAL(__vk_reset_fence(FENCE))

	void __vk_wait_for_and_reset_fence(const VkFence *vk_phFence);
#define vk_wait_for_and_reset_fence(FENCE) CATCH_SIGNAL(__vk_wait_for_and_reset_fence(FENCE))

	bool __vk_signal_semaphores(const uint32_t u32SignalSemaphoreCount, const VkSemaphore *vk_phSignalSemaphores, const char *pcFile, const char *pcFunc, const uint32_t u32Line);
#define vk_signal_semaphores(SIGNAL_SEMA_COUNT, SIGNAL_SEMAS) CATCH_SIGNAL_AND_RETURN(__vk_signal_semaphores(SIGNAL_SEMA_COUNT, SIGNAL_SEMAS, __FILE__, __func__, __LINE__), bool)

	bool __vk_signal_fence(const VkFence vk_hSignalFence, const char *pcFile, const char *pcFunc, const uint32_t u32Line);
#define vk_signal_fence(FENCE) CATCH_SIGNAL_AND_RETURN(__vk_signal_fence(FENCE, __FILE__, __func__, __LINE__), bool)


	bool __vk_submit_to_graphics_queue(const uint32_t u32WaitSemaphoreCount, const VkSemaphore *vk_phWaitSemaphores, const VkPipelineStageFlags *vk_peWaitPipelineStage, const uint32_t u32CommandBufferCount, const VkCommandBuffer *vk_phCommandBuffers, const uint32_t u32SignalSemaphoreCount, const VkSemaphore *vk_phSignalSemaphores, const VkFence vk_hFenceToSignal, const char *pcFile, const char *pcFunc, const uint32_t u32Line);
#define vk_submit_to_graphics_queue(WAIT_SEMA_COUNT, WAIT_SEMAS, WAIT_PIPES, CMDBUFFER_COUNT, CMDBUFFERS, SIGNAL_SEMA_COUNT, SIGNAL_SEMAS, SIGNAL_FENCE) CATCH_SIGNAL_AND_RETURN(__vk_submit_to_graphics_queue(WAIT_SEMA_COUNT, WAIT_SEMAS, WAIT_PIPES, CMDBUFFER_COUNT, CMDBUFFERS, SIGNAL_SEMA_COUNT, SIGNAL_SEMAS, SIGNAL_FENCE, __FILE__, __func__, __LINE__), bool)

	bool __vk_submit_to_transfer_queue(const uint32_t u32WaitSemaphoreCount, const VkSemaphore *vk_phWaitSemaphores, const VkPipelineStageFlags *vk_peWaitPipelineStage, const uint32_t u32CommandBufferCount, const VkCommandBuffer *vk_phCommandBuffers, const uint32_t u32SignalSemaphoreCount, const VkSemaphore *vk_phSignalSemaphores, const VkFence vk_hFenceToSignal, const char *pcFile, const char *pcFunc, const uint32_t u32Line);
#define vk_submit_to_transfer_queue(WAIT_SEMA_COUNT, WAIT_SEMAS, WAIT_PIPES, CMDBUFFER_COUNT, CMDBUFFERS, SIGNAL_SEMA_COUNT, SIGNAL_SEMAS, SIGNAL_FENCE) CATCH_SIGNAL_AND_RETURN(__vk_submit_to_transfer_queue(WAIT_SEMA_COUNT, WAIT_SEMAS, WAIT_PIPES, CMDBUFFER_COUNT, CMDBUFFERS, SIGNAL_SEMA_COUNT, SIGNAL_SEMAS, SIGNAL_FENCE, __FILE__, __func__, __LINE__), bool)

	bool __vk_submit_to_present_queue(const uint32_t u32WaitSemaphoreCount, const VkSemaphore *vk_phWaitSemaphores, const uint32_t *pu32SwapchainImageIndex, const char *pcFile, const char *pcFunc, const uint32_t u32Line);
#define vk_submit_to_present_queue(WAIT_SEMA_COUNT, WAIT_SEMAS, SWAPIMG_INDEX) CATCH_SIGNAL_AND_RETURN(__vk_submit_to_present_queue(WAIT_SEMA_COUNT, WAIT_SEMAS, SWAPIMG_INDEX, __FILE__, __func__, __LINE__), bool)

	bool __vk_begin_recording_command_buffer(const VkCommandBuffer vk_hCommandBuffer, const VkCommandBufferUsageFlags vk_eUsageFlags, const VkCommandBufferInheritanceInfo *vk_pInheritanceInfo, const char *pcFile, const char *pcFunc, const uint32_t u32Line);
#define vk_begin_recording_command_buffer(CMDBUFFER, USAGE, INHERITANCE_INFO) __vk_begin_recording_command_buffer(CMDBUFFER, USAGE, INHERITANCE_INFO, __FILE__, __func__, __LINE__)

}

#endif /* __RE_VULKAN_WRAPPER_FUNCTIONS_H__ */
