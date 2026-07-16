#include "RE_Renderer_Mesh_Internal.hpp"

namespace RE {

	bool init_renderer_meshes() {
		if (create_renderer_mesh_square_2D()) {
			if (create_renderer_mesh_square_3D()) {
				if (alloc_memory_for_permanent_meshes()) {
					PRINT_DEBUG("Initializing Vulkan task to fill mesh buffers with coordinates and indices");
					const uint8_t au8LogicalQueueIndices[] = {u8LogicalQueueCount, aRenderTasks[0].logical_queue_index_for_function(RENDER_TASK_SUBINDEX_RENDERING)};
					const VkQueueFlagBits vk_aeQueueTypes[] = {VK_QUEUE_TRANSFER_BIT, VK_QUEUE_GRAPHICS_BIT};
					const uint32_t au32SeparationIds[] = {0, 1};
					const VulkanTask_Queues queueRequirements = {
						.pau8LogicalQueueIndices = au8LogicalQueueIndices,
						.vk_paeQueueTypes = vk_aeQueueTypes,
						.pau32StrictSeparationIds = au32SeparationIds,
						.u32FunctionsCount = 2
					};
					VulkanTask transferTask(queueRequirements, false, false, true);
					PRINT_DEBUG("Recording 1st scope of the Vulkan task for setting coordinates in mesh buffers");
					if (transferTask.record(
							0,
							VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
							[&](VkCommandBuffer vk_hCommandBuffer, uint8_t u8PreviousLogicalQueue, uint8_t u8CurrentLogicalQueue, uint8_t u8NextLogicalQueue) {
								record_cmd_populate_mesh_square_2D(vk_hCommandBuffer);
								record_cmd_populate_mesh_square_3D(vk_hCommandBuffer);
								PRINT_DEBUG("Putting pipeline barrier for releasing ownership of the mesh buffers");
								const VkBufferMemoryBarrier vk_aMeshBufferBarriers[PERMANENT_MESH_COUNT] = {
									{
										.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER,
										.pNext = nullptr,
										.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT,
										.dstAccessMask = VK_ACCESS_NONE,
										.srcQueueFamilyIndex = queueFamilyIndices[u8CurrentLogicalQueue],
										.dstQueueFamilyIndex = queueFamilyIndices[u8NextLogicalQueue],
										.buffer = square2D.vk_hMeshBuffer,
										.offset = 0,
										.size = VK_WHOLE_SIZE
									}, {
										.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER,
										.pNext = nullptr,
										.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT,
										.dstAccessMask = VK_ACCESS_NONE,
										.srcQueueFamilyIndex = queueFamilyIndices[u8CurrentLogicalQueue],
										.dstQueueFamilyIndex = queueFamilyIndices[u8NextLogicalQueue],
										.buffer = square3D.vk_hMeshBuffer,
										.offset = 0,
										.size = VK_WHOLE_SIZE
									}
								};
								vkCmdPipelineBarrier(
										vk_hCommandBuffer,
										VK_PIPELINE_STAGE_TRANSFER_BIT,
										VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
										0,
										0,
										nullptr,
										PERMANENT_MESH_COUNT,
										vk_aMeshBufferBarriers,
										0,
										nullptr);
							})) {
						PRINT_DEBUG("Recording 2nd scope of the Vulkan task for setting coordinates in mesh buffers");
						if (transferTask.record(
								1,
								VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
								[&](VkCommandBuffer vk_hCommandBuffer, uint8_t u8PreviousLogicalQueue, uint8_t u8CurrentLogicalQueue, uint8_t u8NextLogicalQueue) {
									PRINT_DEBUG("Putting pipeline barrier for acquiring ownership of the mesh buffers");
									const VkBufferMemoryBarrier vk_aMeshBufferBarriers[PERMANENT_MESH_COUNT] = {
										{
											.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER,
											.pNext = nullptr,
											.srcAccessMask = VK_ACCESS_NONE,
											.dstAccessMask = VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT,
											.srcQueueFamilyIndex = queueFamilyIndices[u8PreviousLogicalQueue],
											.dstQueueFamilyIndex = queueFamilyIndices[u8CurrentLogicalQueue],
											.buffer = square2D.vk_hMeshBuffer,
											.offset = 0,
											.size = VK_WHOLE_SIZE
										}, {
											.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER,
											.pNext = nullptr,
											.srcAccessMask = VK_ACCESS_NONE,
											.dstAccessMask = VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT | VK_ACCESS_INDEX_READ_BIT,
											.srcQueueFamilyIndex = queueFamilyIndices[u8PreviousLogicalQueue],
											.dstQueueFamilyIndex = queueFamilyIndices[u8CurrentLogicalQueue],
											.buffer = square3D.vk_hMeshBuffer,
											.offset = 0,
											.size = VK_WHOLE_SIZE
										}
									};
									vkCmdPipelineBarrier(
											vk_hCommandBuffer,
											VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
											VK_PIPELINE_STAGE_VERTEX_INPUT_BIT,
											0,
											0,
											nullptr,
											PERMANENT_MESH_COUNT,
											vk_aMeshBufferBarriers,
											0,
											nullptr);
								})) {
							PRINT_DEBUG("Creating temporary fence in Vulkan");
							Vulkan_Fence fence(0);
							if (fence.valid()) {
								PRINT_DEBUG("Submitting Vulkan task populating in mesh buffers");
								constexpr VkPipelineStageFlags2 vk_aeInternalWaits[] = {VK_PIPELINE_STAGE_2_TRANSFER_BIT};
								if (transferTask.submit(0, nullptr, vk_aeInternalWaits, 0, nullptr, fence())) {
									PRINT_DEBUG("Waiting for the temporary Vulkan fence to be signaled");
									fence.wait_for();
									return true;
								} else
									RE_FATAL_ERROR("Failed to submit the Vulkan task to the GPU for populating the permanent mesh buffers");
							} else
								RE_FATAL_ERROR("Failed to create a Vulkan fence for waiting finishing the population of the permanent mesh buffers");
						} else
							RE_FATAL_ERROR("Failed to record the 2nd scope of the Vulkan task for the square meshes");
					} else
						RE_FATAL_ERROR("Failed to record the 1st scope of the Vulkan task for the square meshes");
					free_memory_for_permanent_meshes();
				}
				destroy_renderer_mesh_square_3D();
			}
			destroy_renderer_mesh_square_2D();
		}
		return false;
	}

	void destroy_renderer_meshes() {
		free_memory_for_permanent_meshes();
		destroy_renderer_mesh_square_3D();
		destroy_renderer_mesh_square_2D();
	}

}
