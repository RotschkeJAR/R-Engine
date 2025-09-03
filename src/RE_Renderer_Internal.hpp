#ifndef __RE_RENDERER_INTERNAL_H__
#define __RE_RENDERER_INTERNAL_H__

#include "RE_Renderer.hpp"
#include "RE_Render System.hpp"
#include "RE_Texture.hpp"

namespace RE {

	// Index buffer
	extern VkBuffer vk_hRectIndexBuffer;
	bool create_rect_index_buffer();
	void destroy_rect_index_buffer();

	// MSAA
	extern VkSampleCountFlagBits vk_eMsaaCount;

	// Depth-stencil images
	extern std::array<VkImage, RE_VK_FRAMES_IN_FLIGHT * 2> vk_ahDepthStencilImages;
	extern std::array<VkImageView, vk_ahDepthStencilImages.size()> vk_ahDepthStencilImageViews;
	bool create_depth_stencil_buffers();
	void destroy_depth_stencil_buffers();

}

#endif /* __RE_RENDERER_INTERNAL_H__ */
