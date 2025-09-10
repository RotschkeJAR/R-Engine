#include "RE_Renderer_Internal.hpp"

namespace RE {
	
	VkBuffer vk_hRectIndexBuffer;
	static VkDeviceMemory vk_hRectIndexBufferMemory;

	bool create_rect_index_buffer(VkBuffer vk_hStagingIndexBuffer, VkDeviceMemory vk_hStagingIndexBufferMemory, VulkanTask &rIndexBufferTransferTask, Vulkan_TimelineSemaphore &rIndexBufferTransferTimelineSemaphore) {
		uint16_t *pau16Indices;
		vkMapMemory(vk_hDevice, vk_hStagingIndexBufferMemory, 0, RE_VK_INDEX_BUFFER_SIZE, 0, reinterpret_cast<void**>(&pau16Indices));
		std::thread indexBufferDataInit([&]() {
			for (uint32_t u32RectIndex = 0; u32RectIndex < RE_VK_RENDERABLE_RECTANGLES_COUNT; u32RectIndex++) {
				pau16Indices[u32RectIndex * 6 + 0] = u32RectIndex * 4 + 0;
				pau16Indices[u32RectIndex * 6 + 1] = u32RectIndex * 4 + 1;
				pau16Indices[u32RectIndex * 6 + 2] = u32RectIndex * 4 + 2;
				pau16Indices[u32RectIndex * 6 + 3] = u32RectIndex * 4 + 1;
				pau16Indices[u32RectIndex * 6 + 4] = u32RectIndex * 4 + 3;
				pau16Indices[u32RectIndex * 6 + 5] = u32RectIndex * 4 + 2;
			}
			vkUnmapMemory(vk_hDevice, vk_hStagingIndexBufferMemory);
			rIndexBufferTransferTimelineSemaphore.set_to(RE_VK_TIMELINE_SEMAPHORE_TRANSFER);
		});
		if (PUSH_TO_CALLSTACKTRACE_AND_RETURN(create_vulkan_buffer(RE_VK_INDEX_BUFFER_SIZE, VK_BUFFER_USAGE_TRANSFER_DST_BIT, 1, nullptr, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &vk_hRectIndexBuffer, &vk_hRectIndexBufferMemory), bool)) {
			constexpr VkQueueFlagBits vk_aeIndexBufferTransferQueues[] = {
				VK_QUEUE_TRANSFER_BIT,
				VK_QUEUE_GRAPHICS_BIT
			};
			const uint8_t au8RenderGraphicsLogicalQueueIndex[] = {
				u8LogicalQueueCount,
				get_render_graphics_queue_logical_index()
			};
			const VulkanTask_Queues indexBufferTransferQueues = {
				.u32FunctionsCount = 2,
				.pau8LogicalQueueIndices = au8RenderGraphicsLogicalQueueIndex,
				.vk_paeQueueTypes = vk_aeIndexBufferTransferQueues
			};
			PUSH_TO_CALLSTACKTRACE(rIndexBufferTransferTask.init(indexBufferTransferQueues, true));
			rIndexBufferTransferTask.paFunctions[0] = [&](const VkCommandBuffer vk_hCommandBuffer, const uint8_t u8PreviousLogicalQueue, const uint8_t u8CurrentLogicalQueue, const uint8_t u8NextLogicalQueue) {
				constexpr VkBufferCopy2 vk_indexBufferCopyData = {
					.sType = VK_STRUCTURE_TYPE_BUFFER_COPY_2,
					.size = RE_VK_INDEX_BUFFER_SIZE
				};
				const VkCopyBufferInfo2 vk_indexBufferCopyInfo = {
					.sType = VK_STRUCTURE_TYPE_COPY_BUFFER_INFO_2,
					.srcBuffer = vk_hStagingIndexBuffer,
					.dstBuffer = vk_hRectIndexBuffer,
					.regionCount = 1,
					.pRegions = &vk_indexBufferCopyData
				};
				vkCmdCopyBuffer2(vk_hCommandBuffer, &vk_indexBufferCopyInfo);
				if (u8NextLogicalQueue == RE_VK_LOGICAL_QUEUE_IGNORED || u8CurrentLogicalQueue == u8NextLogicalQueue)
					return;
				const VkBufferMemoryBarrier2 vk_indexBufferReleaseBarrier = {
					.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER_2,
					.srcStageMask = VK_PIPELINE_STAGE_2_TRANSFER_BIT,
					.srcAccessMask = VK_ACCESS_2_TRANSFER_WRITE_BIT,
					.dstStageMask = VK_PIPELINE_STAGE_2_VERTEX_INPUT_BIT,
					.dstAccessMask = VK_ACCESS_2_INDEX_READ_BIT,
					.srcQueueFamilyIndex = queueFamilyIndices[u8CurrentLogicalQueue],
					.dstQueueFamilyIndex = queueFamilyIndices[u8NextLogicalQueue],
					.buffer = vk_hRectIndexBuffer,
					.size = RE_VK_INDEX_BUFFER_SIZE
				};
				const VkDependencyInfo vk_ownershipTransfer = {
					.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO,
					.bufferMemoryBarrierCount = 1,
					.pBufferMemoryBarriers = &vk_indexBufferReleaseBarrier
				};
				vkCmdPipelineBarrier2(vk_hCommandBuffer, &vk_ownershipTransfer);
			};
			rIndexBufferTransferTask.paFunctions[1] = [&](const VkCommandBuffer vk_hCommandBuffer, const uint8_t u8PreviousLogicalQueue, const uint8_t u8CurrentLogicalQueue, const uint8_t u8NextLogicalQueue) {
				if (u8PreviousLogicalQueue == RE_VK_LOGICAL_QUEUE_IGNORED || u8CurrentLogicalQueue == u8PreviousLogicalQueue)
					return;
				const VkBufferMemoryBarrier2 vk_indexBufferReleaseBarrier = {
					.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER_2,
					.srcStageMask = VK_PIPELINE_STAGE_2_TRANSFER_BIT,
					.srcAccessMask = VK_ACCESS_2_TRANSFER_WRITE_BIT,
					.dstStageMask = VK_PIPELINE_STAGE_2_VERTEX_INPUT_BIT,
					.dstAccessMask = VK_ACCESS_2_INDEX_READ_BIT,
					.srcQueueFamilyIndex = queueFamilyIndices[u8PreviousLogicalQueue],
					.dstQueueFamilyIndex = queueFamilyIndices[u8CurrentLogicalQueue],
					.buffer = vk_hRectIndexBuffer,
					.size = RE_VK_INDEX_BUFFER_SIZE
				};
				const VkDependencyInfo vk_ownershipTransfer = {
					.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO,
					.bufferMemoryBarrierCount = 1,
					.pBufferMemoryBarriers = &vk_indexBufferReleaseBarrier
				};
				vkCmdPipelineBarrier2(vk_hCommandBuffer, &vk_ownershipTransfer);
			};
			PUSH_TO_CALLSTACKTRACE(rIndexBufferTransferTask.record(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT));
			indexBufferDataInit.join();
			constexpr uint64_t a2u64TimelineSemaphoreValues[2] = {
				RE_VK_TIMELINE_SEMAPHORE_TRANSFER,
				RE_VK_TIMELINE_SEMAPHORE_FINISH
			};
			const VkTimelineSemaphoreSubmitInfo vk_timelineSubmitInfo = {
				.sType = VK_STRUCTURE_TYPE_TIMELINE_SEMAPHORE_SUBMIT_INFO,
				.waitSemaphoreValueCount = 1,
				.pWaitSemaphoreValues = &a2u64TimelineSemaphoreValues[0],
				.signalSemaphoreValueCount = 1,
				.pSignalSemaphoreValues = &a2u64TimelineSemaphoreValues[1]
			};
			constexpr VkPipelineStageFlags vk_a1ePipelineToWaitAt[1] = {
				VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT
			};
			PUSH_TO_CALLSTACKTRACE(rIndexBufferTransferTask.submit(&vk_timelineSubmitInfo, 1, rIndexBufferTransferTimelineSemaphore.get_timeline_semaphore_ptr(), vk_a1ePipelineToWaitAt, 1, rIndexBufferTransferTimelineSemaphore.get_timeline_semaphore_ptr(), VK_NULL_HANDLE));
			return true;
		}
		if (indexBufferDataInit.joinable())
			indexBufferDataInit.join();
		return false;
	}

	void destroy_rect_index_buffer() {
		vkFreeMemory(vk_hDevice, vk_hRectIndexBufferMemory, nullptr);
		vkDestroyBuffer(vk_hDevice, vk_hRectIndexBuffer, nullptr);
	}

}
