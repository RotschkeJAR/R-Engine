#ifndef __RE_RENDERER_IMAGES_INTERNAL_H__
#define __RE_RENDERER_IMAGES_INTERNAL_H__ 1

#include "RE_Renderer_Images.hpp"

namespace RE {

	// Render target
	constexpr VkImageUsageFlags vk_mRenderTargetImageUsages = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
	bool create_render_target_image();
	bool create_render_target_image_views();
	void destroy_render_target_image();
	void destroy_render_target_image_views();

	// Singlesampled images
	constexpr VkImageUsageFlags vk_mSinglesampledImageUsages = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
	bool create_singlesampled_image();
	bool create_singlesampled_image_views();
	void destroy_singlesampled_image();
	void destroy_singlesampled_image_views();
	
	// Depth-stencil images
	typedef unsigned DepthStencilFeatureFlags;
	enum DepthStencilFeatureBits : DepthStencilFeatureFlags {
		DEPTH_STENCIL_FEATURE_D16_SUPPORTED_BIT = 0x1,
		DEPTH_STENCIL_FEATURE_D24_SUPPORTED_BIT = 0x2,
		DEPTH_STENCIL_FEATURE_D32_SUPPORTED_BIT = 0x4,
		DEPTH_STENCIL_FEATURE_D16_S8_SUPPORTED_BIT = 0x8,
		DEPTH_STENCIL_FEATURE_D24_S8_SUPPORTED_BIT = 0x10,
		DEPTH_STENCIL_FEATURE_D32_S8_SUPPORTED_BIT = 0x20,
		DEPTH_STENCIL_FEATURE_TRANSIENT_ATTACHMENT_BIT = 0x40
	};
	constexpr VkImageUsageFlags vk_mDepthStencilImageUsages = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
	extern DepthStencilFeatureFlags mDepthStencilFeatures;
	bool create_depth_stencil_image();
	bool create_depth_stencil_image_views();
	void destroy_depth_stencil_image();
	void destroy_depth_stencil_image_views();

	// Characters
	bool create_character_image();
	bool create_character_image_view();
	void destroy_character_image();
	void destroy_character_image_view();

	// Memory
	bool alloc_memory_for_renderer_images();
	bool alloc_memory_for_swapchain_related_images();
	void free_memory_for_renderer_images();
	void free_memory_for_swapchain_related_images();

}

#endif /* __RE_RENDERER_IMAGES_INTERNAL_H__ */
