#include "RE_Renderer_Internal.hpp"

namespace RE {
	
	VkBuffer vk_hRectIndexBuffer;
	static VkDeviceMemory vk_hRectIndexBufferMemory;

	bool create_rect_index_buffer(VkBuffer vk_hStagingIndexBuffer, VkDeviceMemory vk_hStagingIndexBufferMemory, VkFence vk_hIndexBufferTransferFence) {
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
		});
		if (PUSH_TO_CALLSTACKTRACE_AND_RETURN(create_vulkan_buffer(RE_VK_INDEX_BUFFER_SIZE, VK_BUFFER_USAGE_TRANSFER_DST_BIT, 1, nullptr, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &vk_hRectIndexBuffer, &vk_hRectIndexBufferMemory), bool)) {
			constexpr VkQueueFlagBits vk_aeIndexBufferTransferQueues[] = {
				VK_QUEUE_TRANSFER_BIT,
				VK_QUEUE_GRAPHICS_BIT
			};
			VulkanTask indexBufferTransferTask(2, vk_aeIndexBufferTransferQueues, false, true);
			indexBufferTransferTask.paFunctions[0] = [&](const VkCommandBuffer vk_hCommandBuffer, const uint32_t u32PreviousQueueFamily, const uint32_t u32CurrentQueueFamily, const uint32_t u32NextQueueFamily) {
				const VkBufferCopy vk_indexBufferCopy = {
					.size = RE_VK_INDEX_BUFFER_SIZE
				};
				vkCmdCopyBuffer(vk_hCommandBuffer, vk_hStagingIndexBuffer, vk_hRectIndexBuffer, 1, &vk_indexBufferCopy);
				if (u32NextQueueFamily != VK_QUEUE_FAMILY_IGNORED && u32CurrentQueueFamily != u32NextQueueFamily) {
					const VkBufferMemoryBarrier vk_indexBufferReleaseBarrier = {
						.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER,
						.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT,
						.dstAccessMask = VK_ACCESS_INDEX_READ_BIT,
						.srcQueueFamilyIndex = u32CurrentQueueFamily,
						.dstQueueFamilyIndex = u32NextQueueFamily,
						.buffer = vk_hRectIndexBuffer,
						.size = RE_VK_INDEX_BUFFER_SIZE
					};
					vkCmdPipelineBarrier(vk_hCommandBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_VERTEX_INPUT_BIT, 0, 0, nullptr, 1, &vk_indexBufferReleaseBarrier, 0, nullptr);
				}
			};
			indexBufferTransferTask.paFunctions[1] = [&](const VkCommandBuffer vk_hCommandBuffer, const uint32_t u32PreviousQueueFamily, const uint32_t u32CurrentQueueFamily, const uint32_t u32NextQueueFamily) {
				if (u32PreviousQueueFamily != VK_QUEUE_FAMILY_IGNORED && u32PreviousQueueFamily != u32CurrentQueueFamily) {
					const VkBufferMemoryBarrier vk_indexBufferAcquireBarrier = {
						.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER,
						.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT,
						.dstAccessMask = VK_ACCESS_INDEX_READ_BIT,
						.srcQueueFamilyIndex = u32PreviousQueueFamily,
						.dstQueueFamilyIndex = u32CurrentQueueFamily,
						.buffer = vk_hRectIndexBuffer,
						.size = RE_VK_INDEX_BUFFER_SIZE
					};
					vkCmdPipelineBarrier(vk_hCommandBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_VERTEX_INPUT_BIT, 0, 0, nullptr, 1, &vk_indexBufferAcquireBarrier, 0, nullptr);
				}
			};
			indexBufferTransferTask.record(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);
			indexBufferDataInit.join();
			indexBufferTransferTask.submit(0, nullptr, nullptr, 0, nullptr, vk_hIndexBufferTransferFence);
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
