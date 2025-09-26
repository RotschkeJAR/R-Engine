#ifndef __RE_RENDER_ELEMENT_H__
#define __RE_RENDER_ELEMENT_H__

#include "RE_Renderer.hpp"
#include "RE_Vulkan_Wrapper Classes.hpp"
#include "RE_GPU.hpp"

namespace RE {

#define RE_VK_RECT_BUFFER_VERTEX_FORMAT VK_FORMAT_R32G32_SFLOAT
#define RE_VK_RECT_BUFFER_VERTEX_STRIDE_SIZE (sizeof(float) * 2)
#define RE_VK_RECT_BUFFER_VERTICES_OFFSET 0
#define RE_VK_RECT_BUFFER_VERTICES_SIZE (RE_VK_RECT_BUFFER_VERTEX_STRIDE_SIZE * 4)
#define RE_VK_RECT_BUFFER_INDICES_OFFSET RE_VK_RECT_BUFFER_VERTICES_SIZE
#define RE_VK_RECT_BUFFER_INDICES_SIZE (sizeof(uint16_t) * 6)
#define RE_VK_RECT_BUFFER_TOTAL_SIZE (RE_VK_RECT_BUFFER_INDICES_SIZE + RE_VK_RECT_BUFFER_VERTICES_SIZE)
	/**
	 * Buffer for being used as vertex and index buffer simultaneously for rendering rectangle shaped forms.
	 * 		| 32-bit-float-vertices | 16-bit-unsigned-indices |
	 */
	extern VkBuffer vk_hRectBuffer;
	
	bool init_render_elements(Vulkan_Buffer &rStagingRectBuffer, VulkanTask &rRectBufferCreateTask, Vulkan_Fence &rRectBufferTransferFence);
	void destroy_render_elements();

}

#endif /* __RE_RENDER_ELEMENT_H__ */
