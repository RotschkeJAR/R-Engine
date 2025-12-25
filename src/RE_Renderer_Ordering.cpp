#include "RE_Renderer_Internal.hpp"
#include "RE_RenderElement.hpp"
#include "RE_List_GameObject.hpp"

namespace RE {
	
	void order_rendering(const VkCommandBuffer vk_hCommandBuffer) {
		const VkViewport vk_viewport = {
			.x = static_cast<float>(vk_cameraProjectionOnscreen.offset.x),
			.y = static_cast<float>(vk_cameraProjectionOnscreen.offset.y),
			.width = static_cast<float>(vk_cameraProjectionOnscreen.extent.width),
			.height = static_cast<float>(vk_cameraProjectionOnscreen.extent.height),
			.minDepth = 0.0f,
			.maxDepth = 1.0f
		};
		record_cmd_begin_renderpass(vk_hCommandBuffer);
		PRINT_DEBUG("Binding graphics pipeline ", vk_hGraphicsPipeline3D);
		vkCmdBindPipeline(vk_hCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, vk_hGraphicsPipeline3D);
		vkCmdSetViewport(vk_hCommandBuffer, 0, 1, &vk_viewport);
		vkCmdSetScissor(vk_hCommandBuffer, 0, 1, &vk_cameraProjectionOnscreen);
		PRINT_DEBUG("Binding Vulkan descriptor sets");
		const VkDescriptorSet vk_ahDescSets[] = {cameraDescSets[u8CurrentFrameInFlightIndex], vk_hTextureDescSet};
		vkCmdBindDescriptorSets(vk_hCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, vk_hGraphicsPipelineLayout, 0, sizeof(vk_ahDescSets) / sizeof(vk_ahDescSets[0]), vk_ahDescSets, 0, nullptr);
		PRINT_DEBUG("Binding vertex, instance and index buffers");
		const VkBuffer vk_aVertexBuffers[] = {vk_hRectBuffer, renderBuffer.get()};
		const VkDeviceSize vk_aBufferOffsets[] = {RE_VK_RECT_BUFFER_VERTICES_OFFSET, sizeof(VkDrawIndexedIndirectCommand)},
			vk_aBufferSizes[] = {RE_VK_RECT_BUFFER_VERTICES_SIZE, gameObjectsToRenderCount * sizeof(GameObjectInstanceData)};
		vkCmdBindVertexBuffers2(vk_hCommandBuffer, 0, 2, vk_aVertexBuffers, vk_aBufferOffsets, vk_aBufferSizes, nullptr);
		vkCmdBindIndexBuffer(vk_hCommandBuffer, vk_hRectBuffer, RE_VK_RECT_BUFFER_INDICES_OFFSET, VK_INDEX_TYPE_UINT16);
		PRINT_DEBUG("Drawing indirectly");
		vkCmdDrawIndexedIndirect(vk_hCommandBuffer, renderBuffer.get(), 0, 1, sizeof(VkDrawIndexedIndirectCommand));
		record_cmd_end_renderpass(vk_hCommandBuffer);
	}

}
