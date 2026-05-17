#ifndef __RE_RENDERER_IMAGES_H__
#define __RE_RENDERER_IMAGES_H__

#include "RE_Renderer_Internal.hpp"

namespace RE {

	extern ScreenPercentageSettings screenPercentageSettings;
	extern Vector2u renderImageSize,
		maxRenderImageSize;
	
	void get_queues_for_swapchain_images(std::vector<uint32_t> &rRenderTaskQueueIndices);
	bool create_renderer_images();
	void destroy_renderer_images();

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
