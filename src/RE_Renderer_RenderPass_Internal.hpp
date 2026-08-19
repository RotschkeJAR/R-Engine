#ifndef __RE_RENDERER_RENDER_PASS_INTERNAL_H__
#define __RE_RENDERER_RENDER_PASS_INTERNAL_H__ 1

#include "RE_Renderer_RenderPass.hpp"

namespace RE {

	bool create_render_pass_static();
	void destroy_render_pass_static();
	void begin_render_pass_static(
			VkCommandBuffer vk_hCommandBuffer,
			VkClearValue (&vk_raClears)[RENDER_PASS_ATTACHMENT_COUNT],
			VkRenderPassBeginInfo &vk_rRenderPassBeginInfo,
			VkSubpassBeginInfo &vk_rSubpassBeginInfo,
			VkSubpassEndInfo &vk_rSubpassEndInfo);
	void next_render_subpass_static(
			VkCommandBuffer vk_hCommandBuffer,
			VkSubpassBeginInfo &vk_rSubpassBeginInfo,
			VkSubpassEndInfo &vk_rSubpassEndInfo);
	void end_render_pass_static(
			VkCommandBuffer vk_hCommandBuffer,
			VkSubpassEndInfo &vk_rSubpassEndInfo);
	void begin_swapchain_render_pass_static(
			VkCommandBuffer vk_hCommandBuffer,
			VkClearValue (&vk_raClears)[RENDER_PASS_ATTACHMENT_COUNT],
			VkRenderPassBeginInfo &vk_rRenderPassBeginInfo,
			VkSubpassBeginInfo &vk_rSubpassBeginInfo);
	void next_swapchain_render_pass_static(
			VkCommandBuffer vk_hCommandBuffer,
			VkSubpassBeginInfo &vk_rSubpassBeginInfo,
			VkSubpassEndInfo &vk_rSubpassEndInfo);
	void end_swapchain_render_pass_static(
			VkCommandBuffer vk_hCommandBuffer,
			VkSubpassEndInfo &vk_rSubpassEndInfo);
	void setup_graphics_pipelines_render_pass_static(VkGraphicsPipelineCreateInfo (&vk_arGraphicsPipelineCreateInfos)[GRAPHICS_PIPELINE_COUNT]);
	void setup_swapchain_pipelines_render_pass_static(VkGraphicsPipelineCreateInfo &vk_rGraphicsPipelineCreateInfo);

	bool create_render_pass_dynamic();
	void destroy_render_pass_dynamic();
	void begin_render_pass_dynamic(VkCommandBuffer vk_hCommandBuffer);
	void next_render_subpass_dynamic(VkCommandBuffer vk_hCommandBuffer);
	void end_render_pass_dynamic(VkCommandBuffer vk_hCommandBuffer);
	void begin_swapchain_render_pass_dynamic(VkCommandBuffer vk_hCommandBuffer);
	void next_swapchain_render_pass_dynamic(VkCommandBuffer vk_hCommandBuffer);
	void end_swapchain_render_pass_dynamic(VkCommandBuffer vk_hCommandBuffer);
	void setup_graphics_pipelines_render_pass_dynamic(
			VkGraphicsPipelineCreateInfo (&vk_arGraphicsPipelineCreateInfos)[GRAPHICS_PIPELINE_COUNT],
			VkPipelineRenderingCreateInfo &vk_rDynamicRenderPassInfo);
	void setup_swapchain_pipelines_render_pass_dynamic(
			VkGraphicsPipelineCreateInfo &vk_rGraphicsPipelineCreateInfo,
			VkPipelineRenderingCreateInfo &vk_rDynamicRenderPassInfo);

}

#endif /* __RE_RENDERER_RENDER_PASS_INTERNAL_H__ */
