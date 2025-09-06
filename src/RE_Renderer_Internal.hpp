#ifndef __RE_RENDERER_INTERNAL_H__
#define __RE_RENDERER_INTERNAL_H__

#include "RE_Renderer.hpp"
#include "RE_Render System.hpp"
#include "RE_Texture.hpp"
#include "RE_Vulkan_Wrapper Functions.hpp"
#include "RE_Vulkan_Wrapper Classes.hpp"

namespace RE {

	/**
	 * Rendering rectangle-scheme:
	 * 
	 * 1st triangle: 0, 1, 2
	 * 2nd triangle: 1, 3, 2
	 *  
	 *  0 -------------------- 1
	 *  |               ______/|
	 *  |        ______/       |
	 *  | ______/              |
	 *  |/                     |
	 *  2 -------------------- 3
	 */

	// Index buffer
#define RE_VK_INDEX_BUFFER_SIZE (RE_VK_RENDERABLE_RECTANGLES_COUNT * 6U * sizeof(uint16_t))
	extern VkBuffer vk_hRectIndexBuffer;
	bool create_rect_index_buffer(VkBuffer vk_hStagingIndexBuffer, VkDeviceMemory vk_hStagingIndexBufferMemory, VkFence vk_hIndexBufferTransferFence);
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
