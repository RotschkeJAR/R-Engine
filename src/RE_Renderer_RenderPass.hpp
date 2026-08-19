#ifndef __RE_RENDERER_RENDER_PASS_H__
#define __RE_RENDERER_RENDER_PASS_H__ 1

#include "RE_Renderer_Pipelines.hpp"
#include "RE_Renderer_Internal.hpp"

namespace RE {

#define RENDER_PASS_ATTACHMENT_COUNT           3
#define RENDER_PASS_ATTACHMENT_COLOR           0
#define RENDER_PASS_ATTACHMENT_DEPTH_STENCIL   1
#define RENDER_PASS_ATTACHMENT_RESOLVE_DST     2

	extern VkRenderPass vk_hRenderPass,
		vk_hSwapchainRenderPass;
	extern uint32_t u32SubpassScenery,
		u32SwapchainSubpassWindowFrame;

	bool create_render_pass();
	void destroy_render_pass();

	void begin_render_pass(
			VkCommandBuffer vk_hCommandBuffer,
			VkClearValue (&vk_raClears)[RENDER_PASS_ATTACHMENT_COUNT],
			VkRenderPassBeginInfo &vk_rRenderPassBeginInfo,
			VkSubpassBeginInfo &vk_rSubpassBeginInfo,
			VkSubpassEndInfo &vk_rSubpassEndInfo);
	void next_render_subpass(
			VkCommandBuffer vk_hCommandBuffer,
			VkSubpassBeginInfo &vk_rSubpassBeginInfo,
			VkSubpassEndInfo &vk_rSubpassEndInfo);
	void end_render_pass(
			VkCommandBuffer vk_hCommandBuffer,
			VkSubpassEndInfo &vk_rSubpassEndInfo);

	void begin_swapchain_render_pass(
			VkCommandBuffer vk_hCommandBuffer,
			VkClearValue (&vk_raClears)[RENDER_PASS_ATTACHMENT_COUNT],
			VkRenderPassBeginInfo &vk_rRenderPassBeginInfo,
			VkSubpassBeginInfo &vk_rSubpassBeginInfo);
	void next_swapchain_render_pass(
			VkCommandBuffer vk_hCommandBuffer,
			VkSubpassBeginInfo &vk_rSubpassBeginInfo,
			VkSubpassEndInfo &vk_rSubpassEndInfo);
	void end_swapchain_render_pass(
			VkCommandBuffer vk_hCommandBuffer,
			VkSubpassEndInfo &vk_rSubpassEndInfo);

	void setup_graphics_pipelines_render_pass(
			VkGraphicsPipelineCreateInfo (&vk_arGraphicsPipelineCreateInfos)[GRAPHICS_PIPELINE_COUNT],
			VkPipelineRenderingCreateInfo &vk_rDynamicRenderPassInfo);
	void setup_swapchain_pipelines_render_pass(
			VkGraphicsPipelineCreateInfo &vk_rGraphicsPipelineCreateInfo,
			VkPipelineRenderingCreateInfo &vk_rDynamicRenderPassInfo);

}

#endif /* __RE_RENDERER_RENDER_PASS_H__ */
