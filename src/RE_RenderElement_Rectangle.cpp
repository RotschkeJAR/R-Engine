#include "RE_RenderElement_Internal.hpp"
#include "RE_Vulkan_Wrapper Functions.hpp"

namespace RE {

	VkBuffer vk_hRectBuffer;
	VkDeviceMemory vk_hRectBufferMemory;
	VkPipeline vk_hRectObaquePipeline, vk_hRectTransparentPipeline;
	
	bool init_render_element_rectangle(Vulkan_Buffer &rStagingRectBuffer, VulkanTask &rRectBufferCreateTask, Vulkan_Fence &rRectBufferTransferFence) {
		PRINT_DEBUG("Initializing temporary, staging Vulkan buffer for rectangle data");
		if (rStagingRectBuffer.init(RE_VK_RECT_BUFFER_TOTAL_SIZE, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, 1, nullptr, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT)) {
			PRINT_DEBUG("Mapping memory of rectangle buffer to fill with vertex data");
			float *pafVertices;
			if (rStagingRectBuffer.map_memory(RE_VK_RECT_BUFFER_VERTICES_OFFSET, RE_VK_RECT_BUFFER_VERTICES_SIZE, reinterpret_cast<void**>(&pafVertices))) {
				pafVertices[0] = -1.0f;
				pafVertices[1] = 1.0f;
				pafVertices[2] = 0.0f;
				pafVertices[3] = 1.0f;
				pafVertices[4] = 1.0f;
				pafVertices[5] = 0.0f;
				pafVertices[6] = 1.0f;
				pafVertices[7] = -1.0f;
				pafVertices[8] = 0.0f;
				pafVertices[9] = -1.0f;
				pafVertices[10] = -1.0f;
				pafVertices[11] = 0.0f;
				PRINT_DEBUG("Unmapping memory area for vertex data of rectangle buffer");
				rStagingRectBuffer.unmap_memory();
				PRINT_DEBUG("Mapping memory of rectangle buffer to fill with index data");
				uint16_t *pau16Indices;
				if (rStagingRectBuffer.map_memory(RE_VK_RECT_BUFFER_INDICES_OFFSET, RE_VK_RECT_BUFFER_INDICES_SIZE, reinterpret_cast<void**>(&pau16Indices))) {
					pau16Indices[0] = 0;
					pau16Indices[1] = 1;
					pau16Indices[2] = 2;
					pau16Indices[3] = 2;
					pau16Indices[4] = 3;
					pau16Indices[5] = 0;
					PRINT_DEBUG("Unmapping memory area for index data of rectangle buffer");
					rStagingRectBuffer.unmap_memory();
					PRINT_DEBUG("Creating permanent Vulkan buffer for rendering rectangles");
					if (create_vulkan_buffer(RE_VK_RECT_BUFFER_TOTAL_SIZE, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, 1, nullptr, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &vk_hRectBuffer, &vk_hRectBufferMemory)) {
						const uint8_t pau8LogicalQueuesToUse[] = {std::numeric_limits<uint8_t>::max(), get_render_graphics_queue_logical_index()};
						constexpr VkQueueFlagBits vk_paeQueueTypesToUse[] = {VK_QUEUE_TRANSFER_BIT, VK_QUEUE_GRAPHICS_BIT};
						constexpr uint32_t pau32SeparationIds[] = {0, 0};
						const VulkanTask_Queues queuesToUse = {
							.pau8LogicalQueueIndices = pau8LogicalQueuesToUse,
							.vk_paeQueueTypes = vk_paeQueueTypesToUse,
							.pau32StrictSeparationIds = pau32SeparationIds,
							.u32FunctionsCount = 2
						};
						PRINT_DEBUG("Initializing task for transferring data of rectangle buffer to GPU");
						if (rRectBufferCreateTask.init(queuesToUse, true)) {
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
							rRectBufferCreateTask.paFunctions[0] = [&](const VkCommandBuffer vk_hCommandBuffer, const uint8_t u8PreviousLogicalQueue, const uint8_t u8CurrentLogicalQueue, const uint8_t u8NextLogicalQueue) {
								PRINT_DEBUG("Copy rectangle buffer data to GPU");
								constexpr VkBufferCopy2 vk_transferRectBufferRegion = {
									.sType = VK_STRUCTURE_TYPE_BUFFER_COPY_2,
									.size = RE_VK_RECT_BUFFER_TOTAL_SIZE
								};
								const VkCopyBufferInfo2 vk_transferRectBuffer = {
									.sType = VK_STRUCTURE_TYPE_COPY_BUFFER_INFO_2,
									.srcBuffer = rStagingRectBuffer.get_buffer(),
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
							};
							rRectBufferCreateTask.paFunctions[1] = [&](const VkCommandBuffer vk_hCommandBuffer, const uint8_t u8PreviousLogicalQueue, const uint8_t u8CurrentLogicalQueue, const uint8_t u8NextLogicalQueue) {
								if (u8PreviousLogicalQueue == u8CurrentLogicalQueue)
									return;
								PRINT_DEBUG("Acquiring ownership of rectangle buffer in render queue");
								vk_rectBufferOwnershipBarrier.srcQueueFamilyIndex = queueFamilyIndices[u8PreviousLogicalQueue];
								vk_rectBufferOwnershipBarrier.dstQueueFamilyIndex = queueFamilyIndices[u8CurrentLogicalQueue];
								vkCmdPipelineBarrier2(vk_hCommandBuffer, &vk_rectBufferOwnership);
							};
							PRINT_DEBUG("Recording task for transferring data of rectangle buffer");
							rRectBufferCreateTask.record(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);
							PRINT_DEBUG("Submitting task to GPU");
							if (rRectBufferCreateTask.submit(0, nullptr, 0, nullptr, rRectBufferTransferFence.get_fence()))
								return true;
						}
						PRINT_DEBUG("Destroying temporary Vulkan buffer for rectangle data due to failure");
						vkFreeMemory(vk_hDevice, vk_hRectBufferMemory, nullptr);
						vkDestroyBuffer(vk_hDevice, vk_hRectBuffer, nullptr);
					}
				}
			}
		}
		return false;
	}

	void destroy_render_element_rectangle() {
		PRINT_DEBUG("Destroying temporary Vulkan buffer for rectangle data");
		vkFreeMemory(vk_hDevice, vk_hRectBufferMemory, nullptr);
		vkDestroyBuffer(vk_hDevice, vk_hRectBuffer, nullptr);
	}

}
