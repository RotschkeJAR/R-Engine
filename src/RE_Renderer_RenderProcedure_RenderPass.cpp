#include "RE_Renderer_RenderProcedure_Internal.hpp"

namespace RE {

	void render_pass(VkCommandBuffer vk_hCommandBuffer) {
		VkClearValue vk_aClears[RENDER_PASS_ATTACHMENT_COUNT];
		VkRenderPassBeginInfo vk_renderPassBeginInfo;
		VkSubpassBeginInfo vk_subpassBeginInfo;
		VkSubpassEndInfo vk_subpassEndInfo;
		begin_render_pass(
				vk_hCommandBuffer,
				vk_aClears,
				vk_renderPassBeginInfo,
				vk_subpassBeginInfo,
				vk_subpassEndInfo);
		vkCmdBindPipeline(vk_hCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, vk_hGraphicsPipeline2D);
		const VkDescriptorSet vk_ahDescSets[] = {
			vk_ahGameObjectsDescSets[uCurrentFrameInFlightIndex],
			cameraDescSets[0],
			vk_hSpriteDescSet
		};
		vkCmdBindDescriptorSets(
				vk_hCommandBuffer,
				VK_PIPELINE_BIND_POINT_GRAPHICS,
				vk_hGraphicsPipelineLayout,
				0,
				sizeof(vk_ahDescSets) / sizeof(vk_ahDescSets[0]),
				vk_ahDescSets,
				0,
				nullptr);
		const VkViewport vk_viewport = {
			.x = 0.0f,
			.y = 0.0f,
			.width = static_cast<float>(renderImageSize[0]),
			.height = static_cast<float>(renderImageSize[1]),
			.minDepth = 0.0f,
			.maxDepth = 1.0f
		};
		vkCmdSetViewport(vk_hCommandBuffer, 0, 1, &vk_viewport);
		const VkRect2D vk_scissor = {
			.offset = {
				.x = 0,
				.y = 0
			},
			.extent = {
				.width = renderImageSize[0],
				.height = renderImageSize[1]
			}
		};
		vkCmdSetScissor(vk_hCommandBuffer, 0, 1, &vk_scissor);
		const VkBuffer vk_ahVertexBuffers[] = {
			square2D.vk_hMeshBuffer,
			vk_ahSortableDepthBuffers[uCurrentFrameInFlightIndex]
		};
		constexpr VkDeviceSize vk_aVertexBufferOffsets[sizeof(vk_ahVertexBuffers) / sizeof(vk_ahVertexBuffers[0])] = {};
		vkCmdBindVertexBuffers(vk_hCommandBuffer, 0, sizeof(vk_ahVertexBuffers) / sizeof(vk_ahVertexBuffers[0]), vk_ahVertexBuffers, vk_aVertexBufferOffsets);
		vkCmdDraw(vk_hCommandBuffer, square2D.u32VertexCount, 1, 0, 0);
		end_render_pass(vk_hCommandBuffer, vk_subpassEndInfo);
	}

}
