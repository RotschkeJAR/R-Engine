#ifndef __RE_VULKAN_WRAPPER_FUNCS_H__
#define __RE_VULKAN_WRAPPER_FUNCS_H__

#include "RE_Vulkan_Instance.hpp"

namespace RE {

	bool create_vulkan_shader_from_file(const char *pacPathToFile, VkShaderModule *vk_phShader);
	
	bool create_vulkan_buffer(VkBufferCreateFlags vk_eCreateFlags, VkDeviceSize vk_size, VkBufferUsageFlags vk_eUsages, uint32_t u32QueueCount, const uint32_t *pau32Queues, VkMemoryPropertyFlags vk_eMemoryPropertyFlags, VkBuffer *vk_phBuffer, VkDeviceMemory *vk_phMemory);

	bool create_vulkan_image(VkImageCreateFlags vk_eCreateFlags, VkImageType vk_eType, VkFormat vk_eFormat, VkExtent3D vk_extent, uint32_t u32MipLevels, uint32_t u32ArrayLayerCount, VkSampleCountFlagBits vk_eSamples, VkImageTiling vk_eTiling, VkImageUsageFlags vk_eUsages, uint32_t u32QueueCount, const uint32_t *pau32Queues, VkImageLayout vk_eLayout, VkMemoryPropertyFlags vk_eMemoryPropertyFlags, VkImage *vk_phImage, VkDeviceMemory *vk_phMemory);

	bool create_vulkan_image_view(VkImage vk_hImage, VkImageViewType vk_eType, VkFormat vk_eFormat, VkImageAspectFlags vk_eImageAspects, uint32_t u32BaseMipLevel, uint32_t u32MipLevelCount, uint32_t u32BaseArrayLayer, uint32_t u32ArrayLayerCount, VkImageView *vk_phImageView);

	bool create_vulkan_framebuffer(VkFramebufferCreateFlags vk_eCreateFlags, VkRenderPass vk_hRenderPass, uint32_t u32ImageViewAttachmentCount, const VkImageView *vk_pahImageViews, uint32_t u32Width, uint32_t u32Height, uint32_t u32Layer, VkFramebuffer *vk_phFramebuffer);

	bool create_pipeline_layout(uint32_t u32DescriptorSetLayoutCount, const VkDescriptorSetLayout *vk_pahDescriptorSetLayouts, uint32_t u32PushConstRangeCount, const VkPushConstantRange *vk_pahPushConstRanges, VkPipelineLayoutCreateFlags vk_eCreateFlags, VkPipelineLayout *vk_phPipelineLayout);

	bool create_descriptor_set_layout(uint32_t u32DescriptorSetLayoutBindingCount, const VkDescriptorSetLayoutBinding *vk_paDescriptorSetLayoutBindings, VkDescriptorSetLayoutCreateFlags vk_eCreateFlags, VkDescriptorSetLayout *vk_phDescriptorSetLayout);

	bool create_descriptor_pool(uint32_t u32PoolSizeCounts, const VkDescriptorPoolSize *vk_paDescriptorPoolSizes, uint32_t u32MaxSets, VkDescriptorPool *vk_phDescriptorPool);

	bool alloc_descriptor_set(VkDescriptorPool vk_hDescriptorPool, uint32_t u32SetCount, const VkDescriptorSetLayout *vk_pahDescriptorSetLayouts, VkDescriptorSet *vk_pahDescriptorSet);

	bool alloc_vulkan_command_buffers(VkCommandPool vk_hCommandPool, VkCommandBufferLevel vk_eLevel, uint32_t u32Count, VkCommandBuffer *vk_pahCommandBuffer);

	bool begin_recording_vulkan_command_buffer(VkCommandBuffer vk_hCommandBuffer, VkCommandBufferUsageFlags vk_eUsages, const VkCommandBufferInheritanceInfo *vk_pInheritanceInfo);

}

#endif /* __RE_VULKAN_WRAPPER_FUNCS_H__ */
