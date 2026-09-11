#include "RE_Renderer_RenderProcedure_Internal.hpp"

namespace RE {

	void swapchain_render_pass(VkCommandBuffer vk_hCommandBuffer) {
		VkClearValue vk_aClears[SWAPCHAIN_RENDER_PASS_ATTACHMENT_COUNT];
		VkRenderPassBeginInfo vk_renderPassBeginInfo;
		VkSubpassBeginInfo vk_subpassBeginInfo;
		VkSubpassEndInfo vk_subpassEndInfo;
		begin_swapchain_render_pass(vk_hCommandBuffer, vk_aClears, vk_renderPassBeginInfo, vk_subpassBeginInfo, vk_subpassEndInfo);
		if (should_render_window_frame())
			render_window_frame(vk_hCommandBuffer);
		end_swapchain_render_pass(vk_hCommandBuffer, vk_subpassEndInfo);
	}

}
