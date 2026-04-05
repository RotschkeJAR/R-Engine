#ifndef __RE_RENDERER_IMAGES_H__
#define __RE_RENDERER_IMAGES_H__

#include "RE_Renderer_Internal.hpp"

namespace RE {

	extern ScreenPercentageSettings screenPercentageSettings;
	extern Vector2u renderImageSize,
		maxRenderImageSize;
	
	bool create_images_renderer();
	void destroy_images_renderer();
	bool record_cmd_blitting_render_image();

	// Render target
	extern VkImage vk_hRenderTargetImage;
	extern std::array<VkImageView, RE_VK_FRAMES_IN_FLIGHT> renderTargetImageViews;

	// Singlesampled images
	extern VkImage vk_hSinglesampledImage;
	extern std::array<VkImageView, RE_VK_FRAMES_IN_FLIGHT> singlesampledImageViews;

	// Depth-stencil images
	extern VkImage vk_hDepthImage,
		vk_hStencilImage;
	extern std::array<VkImageView, RE_VK_FRAMES_IN_FLIGHT> depthImageViews,
		stencilImageViews;
	extern VkFormat vk_eSelectedDepthFormat,
		vk_eStelectedStencilFormat;
	extern VkImageLayout vk_eDepthLayout,
		vk_eStencilLayout;

}

#endif /* __RE_RENDERER_IMAGES_H__ */
