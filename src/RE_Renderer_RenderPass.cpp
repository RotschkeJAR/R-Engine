#include "RE_Renderer_RenderPass_Internal.hpp"

namespace RE {

	VkRenderPass vk_hRenderPass = VK_NULL_HANDLE,
		vk_hSwapchainRenderPass = VK_NULL_HANDLE;
	uint32_t u32SubpassScenery = 0,
		u32SwapchainSubpassWindowFrame = 0;
#define bUseDynamic false

	bool create_render_pass() {
		return bUseDynamic ? create_render_pass_dynamic() : create_render_pass_static();
	}

	void destroy_render_pass() {
		if (bUseDynamic)
			destroy_render_pass_dynamic();
		else
			destroy_render_pass_static();
	}

	void begin_render_pass(
			VkCommandBuffer vk_hCommandBuffer,
			VkClearValue (&vk_raClears)[RENDER_PASS_ATTACHMENT_COUNT],
			VkRenderPassBeginInfo &vk_rRenderPassBeginInfo,
			VkSubpassBeginInfo &vk_rSubpassBeginInfo,
			VkSubpassEndInfo &vk_rSubpassEndInfo) {
		if (bUseDynamic)
			begin_render_pass_dynamic(vk_hCommandBuffer);
		else
			begin_render_pass_static(
					vk_hCommandBuffer,
					vk_raClears,
					vk_rRenderPassBeginInfo,
					vk_rSubpassBeginInfo,
					vk_rSubpassEndInfo);
	}

	void next_render_subpass(
			VkCommandBuffer vk_hCommandBuffer,
			VkSubpassBeginInfo &vk_rSubpassBeginInfo,
			VkSubpassEndInfo &vk_rSubpassEndInfo) {
		if (bUseDynamic)
			next_render_subpass_dynamic(vk_hCommandBuffer);
		else
			next_render_subpass_static(vk_hCommandBuffer, vk_rSubpassBeginInfo, vk_rSubpassEndInfo);
	}

	void end_render_pass(
			VkCommandBuffer vk_hCommandBuffer,
			VkSubpassEndInfo &vk_rSubpassEndInfo) {
		if (bUseDynamic)
			end_render_pass_dynamic(vk_hCommandBuffer);
		else
			end_render_pass_static(vk_hCommandBuffer, vk_rSubpassEndInfo);
	}

	void begin_swapchain_render_pass(
			VkCommandBuffer vk_hCommandBuffer,
			VkClearValue (&vk_raClears)[RENDER_PASS_ATTACHMENT_COUNT],
			VkRenderPassBeginInfo &vk_rRenderPassBeginInfo,
			VkSubpassBeginInfo &vk_rSubpassBeginInfo) {
		if (bUseDynamic)
			begin_swapchain_render_pass_dynamic(vk_hCommandBuffer);
		else
			begin_swapchain_render_pass_static(vk_hCommandBuffer, vk_raClears, vk_rRenderPassBeginInfo, vk_rSubpassBeginInfo);
	}

	void next_swapchain_render_pass(
			VkCommandBuffer vk_hCommandBuffer,
			VkSubpassBeginInfo &vk_rSubpassBeginInfo,
			VkSubpassEndInfo &vk_rSubpassEndInfo) {
		if (bUseDynamic)
			next_swapchain_render_pass_dynamic(vk_hCommandBuffer);
		else
			next_swapchain_render_pass_static(vk_hCommandBuffer, vk_rSubpassBeginInfo, vk_rSubpassEndInfo);
	}

	void end_swapchain_render_pass(
			VkCommandBuffer vk_hCommandBuffer,
			VkSubpassEndInfo &vk_rSubpassEndInfo) {
		if (bUseDynamic)
			end_swapchain_render_pass_dynamic(vk_hCommandBuffer);
		else
			end_swapchain_render_pass_static(vk_hCommandBuffer, vk_rSubpassEndInfo);
	}

	void setup_graphics_pipelines_render_pass(
			VkGraphicsPipelineCreateInfo (&vk_arGraphicsPipelineCreateInfos)[GRAPHICS_PIPELINE_COUNT],
			VkPipelineRenderingCreateInfo &vk_rDynamicRenderPassInfo) {
		if (bUseDynamic)
			setup_graphics_pipelines_render_pass_dynamic(vk_arGraphicsPipelineCreateInfos, vk_rDynamicRenderPassInfo);
		else
			setup_graphics_pipelines_render_pass_static(vk_arGraphicsPipelineCreateInfos);
	}

	void setup_swapchain_pipelines_render_pass(
			VkGraphicsPipelineCreateInfo &vk_rGraphicsPipelineCreateInfo,
			VkPipelineRenderingCreateInfo &vk_rDynamicRenderPassInfo) {
		if (bUseDynamic)
			setup_swapchain_pipelines_render_pass_dynamic(vk_rGraphicsPipelineCreateInfo, vk_rDynamicRenderPassInfo);
		else
			setup_swapchain_pipelines_render_pass_static(vk_rGraphicsPipelineCreateInfo);
	}

}
