#ifndef __RE_VULKAN_WRAPPER_FUNCS_H__
#define __RE_VULKAN_WRAPPER_FUNCS_H__

#include "RE_Vulkan_Instance.hpp"

namespace RE {
	
	bool create_vulkan_buffer(const VkDeviceSize vk_size, const VkBufferUsageFlags vk_eUsages, const uint32_t u32QueueCount, const uint32_t *pu32Queues, const VkMemoryPropertyFlags vk_eMemoryPropertyFlags, VkBuffer *vk_phBuffer, VkDeviceMemory *vk_phMemory);

	bool create_vulkan_image(const VkImageCreateFlags vk_eCreateFlags, const VkImageType vk_eType, const VkFormat vk_eFormat, const VkExtent3D vk_extent, const uint32_t u32MipLevels, const uint32_t u32ArrayLayerCount, const VkSampleCountFlagBits vk_eSamples, const VkImageTiling vk_eTiling, const VkImageUsageFlags vk_eUsages, const uint32_t u32QueueCount, const uint32_t *pau32Queues, const VkImageLayout vk_eLayout, const VkMemoryPropertyFlags vk_eMemoryPropertyFlags, VkImage *vk_phImage, VkDeviceMemory *vk_phMemory);

	bool create_vulkan_image_view(const VkImage vk_hImage, const VkImageViewType vk_eType, const VkFormat vk_eFormat, const VkImageAspectFlags vk_eImageAspects, const uint32_t u32BaseMipLevel, const uint32_t u32MipLevelCount, const uint32_t u32BaseArrayLayer, const uint32_t u32ArrayLayerCount, VkImageView *vk_phImageView);

	bool create_vulkan_framebuffer(const VkFramebufferCreateFlags vk_eCreateFlags, const VkRenderPass vk_hRenderPass, const uint32_t u32ImageViewAttachmentCount, const VkImageView *vk_pahImageViews, const uint32_t u32Width, const uint32_t u32Height, const uint32_t u32Layer, VkFramebuffer *vk_phFramebuffer);

}

#endif /* __RE_VULKAN_WRAPPER_FUNCS_H__ */