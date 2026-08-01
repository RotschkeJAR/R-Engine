#ifndef __RE_RENDERER_IMAGES_H__
#define __RE_RENDERER_IMAGES_H__ 1

#include "RE_Renderer_Internal.hpp"

namespace RE {

	extern Vector2u renderImageSize;
	
	bool create_renderer_images();
	void destroy_renderer_images();
	bool create_swapchain_related_images();
	void destroy_swapchain_related_images();

	// Render target
	extern VkImage vk_hRenderTargetImage;
	extern VkImageView vk_ahRenderTargetImageViews[RE_VK_FRAMES_IN_FLIGHT];

	// Singlesampled images
	extern VkImage vk_hSinglesampledImage;
	extern VkImageView vk_ahSinglesampledImageViews[RE_VK_FRAMES_IN_FLIGHT];

	// Depth-stencil images
	extern VkImage vk_hDepthStencilImage;
	extern VkImageView vk_ahDepthStencilImageViews[RE_VK_FRAMES_IN_FLIGHT];
	extern VkFormat vk_eSelectedDepthStencilFormat;

	// Characters
	extern VkImage vk_hCharacterImage;
	extern VkImageView vk_hCharacterImageView;

}

#endif /* __RE_RENDERER_IMAGES_H__ */
