#ifndef __RE_RENDERER_IMAGES_INTERNAL_H__
#define __RE_RENDERER_IMAGES_INTERNAL_H__

#include "RE_Renderer_Images.hpp"

namespace RE {

	// Render target
	constexpr VkImageUsageFlags vk_mRenderTargetImageUsages = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
	bool create_render_target_image();
	bool create_render_target_image_views();
	void destroy_render_target_image();
	void destroy_render_target_image_views();

	// Singlesampled images
	constexpr VkImageUsageFlags vk_mSinglesampledImageUsages = VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
	bool create_singlesampled_image();
	bool create_singlesampled_image_views();
	void destroy_singlesampled_image();
	void destroy_singlesampled_image_views();
	
	// Depth-stencil images
#define DEPTH_STENCIL_FEATURE_S8_SUPPORTED_BIT			0x01
#define DEPTH_STENCIL_FEATURE_D16_SUPPORTED_BIT			0x02
#define DEPTH_STENCIL_FEATURE_D24_SUPPORTED_BIT			0x04
#define DEPTH_STENCIL_FEATURE_D32_SUPPORTED_BIT			0x08
#define DEPTH_STENCIL_FEATURE_D16_S8_SUPPORTED_BIT		0x10
#define DEPTH_STENCIL_FEATURE_D24_S8_SUPPORTED_BIT		0x20
#define DEPTH_STENCIL_FEATURE_D32_S8_SUPPORTED_BIT		0x40
#define DEPTH_STENCIL_FEATURE_TRANSIENT_ATTACHMENT_BIT	0x80
	constexpr VkImageUsageFlags vk_mDepthStencilImageUsages = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
	extern uint8_t m8DepthStencilFeatures;
#define GET_COMBINED_USAGE_FLAGS_FOR_DEPTH_STENCIL_IMAGE() (vk_mDepthStencilImageUsages | ((m8DepthStencilFeatures & DEPTH_STENCIL_FEATURE_TRANSIENT_ATTACHMENT_BIT) ? VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT : 0))
	bool create_depth_stencil_image();
	bool create_depth_stencil_image_views();
	void destroy_depth_stencil_image();
	void destroy_depth_stencil_image_views();

	// Memory
	bool alloc_memory_for_images_renderer();
	void free_memory_for_images_renderer();

}

#endif /* __RE_RENDERER_IMAGES_INTERNAL_H__ */
