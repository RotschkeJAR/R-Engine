#include "RE_Renderer_RenderProcedure_Internal.hpp"

namespace RE {

	void sort_game_objects(VkCommandBuffer vk_hCommandBuffer) {
		vkCmdBindPipeline(vk_hCommandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, vk_hComputePipelineDepthSorting);
		vkCmdBindDescriptorSets(
				vk_hCommandBuffer,
				VK_PIPELINE_BIND_POINT_COMPUTE,
				vk_hSortDepthPipelineLayout,
				0,
				1,
				&vk_ahSortableDepthDescSets[uCurrentFrameInFlightIndex],
				0,
				nullptr);
		vkCmdDispatch(vk_hCommandBuffer, 1, 1, 1);
		const VkBufferMemoryBarrier2 vk_bufferBarrierInfo = {
			.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER_2,
			.pNext = nullptr,
			.srcStageMask = VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT,
			.srcAccessMask = VK_ACCESS_2_SHADER_STORAGE_READ_BIT | VK_ACCESS_2_SHADER_STORAGE_WRITE_BIT,
			.dstStageMask = VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT,
			.dstAccessMask = VK_ACCESS_2_SHADER_STORAGE_READ_BIT | VK_ACCESS_2_SHADER_STORAGE_WRITE_BIT,
			.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
			.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
			.buffer = vk_ahSortableDepthBuffers[uCurrentFrameInFlightIndex],
			.offset = 0,
			.size = VK_WHOLE_SIZE
		};
		const VkDependencyInfo vk_dependencyInfo = {
			.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO,
			.pNext = nullptr,
			.dependencyFlags = 0,
			.memoryBarrierCount = 0,
			.pMemoryBarriers = nullptr,
			.bufferMemoryBarrierCount = 1,
			.pBufferMemoryBarriers = &vk_bufferBarrierInfo,
			.imageMemoryBarrierCount = 0,
			.pImageMemoryBarriers = nullptr
		};
		vkCmdPipelineBarrier2(vk_hCommandBuffer, &vk_dependencyInfo);
		vkCmdDispatch(vk_hCommandBuffer, 1, 1, 1);
	}

}
