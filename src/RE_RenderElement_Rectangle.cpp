#include "RE_RenderElement_Internal.hpp"

namespace RE {

	VkBuffer vk_hRectBuffer;
	static VulkanMemory rectBufferMemory;
	
	bool init_render_element_rectangle(Vulkan_Buffer &rStagingRectBuffer, VulkanTask *pTransferTask, Vulkan_Fence &rTransferFence) {
		PRINT_DEBUG("Initializing temporary, staging Vulkan buffer for rectangle data");
		if (rStagingRectBuffer.create(0, RE_VK_RECT_BUFFER_TOTAL_SIZE, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, 1, nullptr, RE_VK_CPU_RAM, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT)) {
			PRINT_DEBUG("Mapping memory of rectangle buffer to fill with vertex data");
			float *pafVertices;
			if (rStagingRectBuffer.get_memory().map(0, RE_VK_RECT_BUFFER_VERTICES_OFFSET, RE_VK_RECT_BUFFER_VERTICES_SIZE, reinterpret_cast<void**>(&pafVertices))) {
				pafVertices[0] = -1.0f;
				pafVertices[1] = 1.0f;
				pafVertices[2] = 0.0f;
				pafVertices[3] = 0.0f;
				pafVertices[4] = 0.0f;
				pafVertices[5] = 1.0f;
				pafVertices[6] = 1.0f;
				pafVertices[7] = 0.0f;
				pafVertices[8] = 1.0f;
				pafVertices[9] = 0.0f;
				pafVertices[10] = 1.0f;
				pafVertices[11] = -1.0f;
				pafVertices[12] = 0.0f;
				pafVertices[13] = 1.0f;
				pafVertices[14] = 1.0f;
				pafVertices[15] = -1.0f;
				pafVertices[16] = -1.0f;
				pafVertices[17] = 0.0f;
				pafVertices[18] = 0.0f;
				pafVertices[19] = 1.0f;
				PRINT_DEBUG("Unmapping memory area for vertex data of rectangle buffer");
				rStagingRectBuffer.get_memory().unmap();
				PRINT_DEBUG("Mapping memory of rectangle buffer to fill with index data");
				uint16_t *pau16Indices;
				if (rStagingRectBuffer.get_memory().map(0, RE_VK_RECT_BUFFER_INDICES_OFFSET, RE_VK_RECT_BUFFER_INDICES_SIZE, reinterpret_cast<void**>(&pau16Indices))) {
					pau16Indices[0] = 0;
					pau16Indices[1] = 1;
					pau16Indices[2] = 2;
					pau16Indices[3] = 2;
					pau16Indices[4] = 3;
					pau16Indices[5] = 0;
					PRINT_DEBUG("Unmapping memory area for index data of rectangle buffer");
					rStagingRectBuffer.get_memory().unmap();
					PRINT_DEBUG("Creating permanent Vulkan buffer for rendering rectangles");
					if (create_vulkan_buffer(0, RE_VK_RECT_BUFFER_TOTAL_SIZE, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, 1, nullptr, RE_VK_GPU_RAM, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &vk_hRectBuffer, &rectBufferMemory)) {
						VkBufferMemoryBarrier2 vk_rectBufferOwnershipBarrier;
						vk_rectBufferOwnershipBarrier.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER_2;
						vk_rectBufferOwnershipBarrier.pNext = nullptr;
						vk_rectBufferOwnershipBarrier.srcStageMask = VK_PIPELINE_STAGE_2_TRANSFER_BIT;
						vk_rectBufferOwnershipBarrier.srcAccessMask = VK_ACCESS_2_TRANSFER_WRITE_BIT;
						vk_rectBufferOwnershipBarrier.dstStageMask = VK_PIPELINE_STAGE_2_VERTEX_INPUT_BIT;
						vk_rectBufferOwnershipBarrier.dstAccessMask = VK_ACCESS_2_INDEX_READ_BIT | VK_ACCESS_2_VERTEX_ATTRIBUTE_READ_BIT;
						vk_rectBufferOwnershipBarrier.buffer = vk_hRectBuffer;
						vk_rectBufferOwnershipBarrier.offset = 0;
						vk_rectBufferOwnershipBarrier.size = RE_VK_RECT_BUFFER_TOTAL_SIZE;
						const VkDependencyInfo vk_rectBufferOwnership = {
							.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO,
							.bufferMemoryBarrierCount = 1,
							.pBufferMemoryBarriers = &vk_rectBufferOwnershipBarrier
						};
						pTransferTask->record(0, VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT, [&](const VkCommandBuffer vk_hCommandBuffer, const uint8_t u8PreviousLogicalQueue, const uint8_t u8CurrentLogicalQueue, const uint8_t u8NextLogicalQueue) {
								PRINT_DEBUG("Copy rectangle buffer data to GPU");
								constexpr VkBufferCopy2 vk_transferRectBufferRegion = {
									.sType = VK_STRUCTURE_TYPE_BUFFER_COPY_2,
									.size = RE_VK_RECT_BUFFER_TOTAL_SIZE
								};
								const VkCopyBufferInfo2 vk_transferRectBuffer = {
									.sType = VK_STRUCTURE_TYPE_COPY_BUFFER_INFO_2,
									.srcBuffer = rStagingRectBuffer.get(),
									.dstBuffer = vk_hRectBuffer,
									.regionCount = 1,
									.pRegions = &vk_transferRectBufferRegion
								};
								vkCmdCopyBuffer2(vk_hCommandBuffer, &vk_transferRectBuffer);
								if (u8CurrentLogicalQueue == u8NextLogicalQueue)
									return;
								PRINT_DEBUG("Transferring ownership of rectangle buffer to render queue");
								vk_rectBufferOwnershipBarrier.srcQueueFamilyIndex = queueFamilyIndices[u8CurrentLogicalQueue];
								vk_rectBufferOwnershipBarrier.dstQueueFamilyIndex = queueFamilyIndices[u8NextLogicalQueue];
								vkCmdPipelineBarrier2(vk_hCommandBuffer, &vk_rectBufferOwnership);
						});
						pTransferTask->record(1, VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT, [&](const VkCommandBuffer vk_hCommandBuffer, const uint8_t u8PreviousLogicalQueue, const uint8_t u8CurrentLogicalQueue, const uint8_t u8NextLogicalQueue) {
								if (u8PreviousLogicalQueue == u8CurrentLogicalQueue)
									return;
								PRINT_DEBUG("Acquiring ownership of rectangle buffer in render queue");
								vk_rectBufferOwnershipBarrier.srcQueueFamilyIndex = queueFamilyIndices[u8PreviousLogicalQueue];
								vk_rectBufferOwnershipBarrier.dstQueueFamilyIndex = queueFamilyIndices[u8CurrentLogicalQueue];
								vkCmdPipelineBarrier2(vk_hCommandBuffer, &vk_rectBufferOwnership);
						});
						PRINT_DEBUG("Submitting task to GPU");
						constexpr VkPipelineStageFlags2 vk_eWaitStage = VK_PIPELINE_STAGE_2_TRANSFER_BIT;
						if (pTransferTask->submit(0, nullptr, &vk_eWaitStage, 0, nullptr, rTransferFence.get()))
							return true;
						else
							RE_FATAL_ERROR("Failed to submit task to transfer rectangle buffer to GPU");
						PRINT_DEBUG("Destroying temporary Vulkan buffer for rectangle data due to failure");
						vkDestroyBuffer(vk_hDevice, vk_hRectBuffer, nullptr);
						rectBufferMemory.free();
					}
				}
			}
		}
		return false;
	}

	void destroy_render_element_rectangle() {
		PRINT_DEBUG("Destroying temporary Vulkan buffer for rectangle data");
		vkDestroyBuffer(vk_hDevice, vk_hRectBuffer, nullptr);
		rectBufferMemory.free();
	}

}
