#include "RE_Renderer_Mesh.hpp"

namespace RE {

	static VulkanMemory squareMeshesMemory;
	static VulkanMesh square2D,
		square3D;

	static bool create_square_meshes() {
		const bool bIndexUint8Exists = are_vulkan_features_enabled<ENABLED_FEATURE_INDEX_UINT_8_BIT>();
		PRINT_DEBUG("Creating mesh buffer for 2D square in Vulkan");
		if (create_vulkan_buffer(0,
				sizeof(VertexData) * 4,
				VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
				1,
				nullptr,
				&square2D.vk_hMeshBuffer)) {
			square2D.u32VertexCount = 4;
			square2D.u32IndexCount = 0;
			square2D.vk_eIndexType = VK_INDEX_TYPE_NONE_KHR;
			square2D.eMeshType = MESH_TYPE_2D;
			PRINT_DEBUG("Creating mesh buffer for 3D square in Vulkan");
			if (create_vulkan_buffer(0,
					sizeof(VertexData) * 4 + (bIndexUint8Exists ? (sizeof(uint8_t) * 8) : (sizeof(uint16_t) * 6)),
					VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
					1,
					nullptr,
					&square3D.vk_hMeshBuffer)) {
				square3D.vk_indicesOffset = sizeof(VertexData) * 4;
				square3D.u32VertexCount = 4;
				square3D.u32IndexCount = 6;
				square3D.eMeshType = MESH_TYPE_3D;
				PRINT_DEBUG("Fetching memory requirements of both mesh buffers");
				VkMemoryRequirements vk_aMemoryRequirements[2];
				vkGetBufferMemoryRequirements(vk_hDevice, square2D.vk_hMeshBuffer, &vk_aMemoryRequirements[0]);
				vkGetBufferMemoryRequirements(vk_hDevice, square3D.vk_hMeshBuffer, &vk_aMemoryRequirements[1]);
				VkDeviceSize vk_size = 0;
				for (const VkMemoryRequirements &vk_rMemoryRequirements : vk_aMemoryRequirements)
					vk_size = next_multiple_inclusive<VkDeviceSize>(vk_size, vk_rMemoryRequirements.alignment) + vk_rMemoryRequirements.size;
				PRINT_DEBUG("Allocating memory for both mesh buffers in Vulkan");
				if (squareMeshesMemory.alloc(vk_size, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, vk_aMemoryRequirements[0].memoryTypeBits & vk_aMemoryRequirements[1].memoryTypeBits) == VK_SUCCESS) {
					PRINT_DEBUG("Binding memory to both mesh buffers in Vulkan");
					if (bind_vulkan_memory(square3D.vk_hMeshBuffer, squareMeshesMemory.get(), next_multiple_inclusive<VkDeviceSize>(vk_aMemoryRequirements[0].size, vk_aMemoryRequirements[1].alignment))
							&& bind_vulkan_memory(square2D.vk_hMeshBuffer, squareMeshesMemory.get())) {
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
						if (transferTask.record(0,
								VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
								[&](const VkCommandBuffer vk_hCommandBuffer, const uint8_t u8PreviousLogicalQueue, const uint8_t u8CurrentLogicalQueue, const uint8_t u8NextLogicalQueue) {
									PRINT_DEBUG("Updating the 2D square-mesh buffer with vertices");
									constexpr VertexData aSquare2DVertices[] = {
										{
											.a4f32Coords = {-0.5f, 0.5f, 0.0f, 1.0f},
											.a2f32TexCoords = {0.0f, 0.0f}
										}, {
											.a4f32Coords = {0.5f, 0.5f, 0.0f, 1.0f},
											.a2f32TexCoords = {1.0f, 0.0f}
										}, {
											.a4f32Coords = {-0.5f, -0.5f, 0.0f, 1.0f},
											.a2f32TexCoords = {0.0f, 1.0f}
										}, {
											.a4f32Coords = {0.5f, -0.5f, 0.0f, 1.0f},
											.a2f32TexCoords = {1.0f, 1.0f}
										}
									};
									vkCmdUpdateBuffer(vk_hCommandBuffer, square2D.vk_hMeshBuffer, 0, sizeof(aSquare2DVertices), aSquare2DVertices);
									PRINT_DEBUG("Updating the 3D square-mesh buffer with vertices");
									constexpr VertexData aSquare3DVertices[] = {
										{
											.a4f32Coords = {-1.0f, 1.0f, 0.0f, 1.0f},
											.a2f32TexCoords = {0.0f, 0.0f}
										}, {
											.a4f32Coords = {1.0f, 1.0f, 0.0f, 1.0f},
											.a2f32TexCoords = {1.0f, 0.0f}
										}, {
											.a4f32Coords = {1.0f, -1.0f, 0.0f, 1.0f},
											.a2f32TexCoords = {1.0f, 1.0f}
										}, {
											.a4f32Coords = {-1.0f, -1.0f, 0.0f, 1.0f},
											.a2f32TexCoords = {0.0f, 1.0f}
										}
									};
									vkCmdUpdateBuffer(vk_hCommandBuffer, square3D.vk_hMeshBuffer, 0, sizeof(aSquare3DVertices), aSquare3DVertices);
									PRINT_DEBUG("Updating the 3D square-mesh buffer with indices");
									if (bIndexUint8Exists) {
										constexpr uint8_t au8Square3DIndices[] = {0, 1, 2, 2, 3, 0, /* padding */ 0, 0};
										vkCmdUpdateBuffer(vk_hCommandBuffer, square3D.vk_hMeshBuffer, square3D.vk_indicesOffset, sizeof(au8Square3DIndices), au8Square3DIndices);
										square3D.vk_eIndexType = VK_INDEX_TYPE_UINT8;
									} else {
										constexpr uint16_t au16Square3DIndices[] = {0, 1, 2, 2, 3, 0};
										vkCmdUpdateBuffer(vk_hCommandBuffer, square3D.vk_hMeshBuffer, square3D.vk_indicesOffset, sizeof(au16Square3DIndices), au16Square3DIndices);
										square3D.vk_eIndexType = VK_INDEX_TYPE_UINT16;
									}
									PRINT_DEBUG("Putting pipeline barrier for releasing ownership of the mesh buffers");
									const VkBufferMemoryBarrier vk_aMeshBufferBarriers[] = {
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
									vkCmdPipelineBarrier(vk_hCommandBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, 0, 0, nullptr, sizeof(vk_aMeshBufferBarriers) / sizeof(vk_aMeshBufferBarriers[0]), vk_aMeshBufferBarriers, 0, nullptr);
								})) {
							PRINT_DEBUG("Recording 2nd scope of the Vulkan task for setting coordinates in mesh buffers");
							if (transferTask.record(1,
									VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
									[&](const VkCommandBuffer vk_hCommandBuffer, const uint8_t u8PreviousLogicalQueue, const uint8_t u8CurrentLogicalQueue, const uint8_t u8NextLogicalQueue) {
										PRINT_DEBUG("Putting pipeline barrier for acquiring ownership of the mesh buffers");
										const VkBufferMemoryBarrier vk_aMeshBufferBarriers[] = {
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
										vkCmdPipelineBarrier(vk_hCommandBuffer, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_VERTEX_INPUT_BIT, 0, 0, nullptr, sizeof(vk_aMeshBufferBarriers) / sizeof(vk_aMeshBufferBarriers[0]), vk_aMeshBufferBarriers, 0, nullptr);
									})) {
								PRINT_DEBUG("Creating temporary fence in Vulkan and submitting Vulkan task setting coordinates in mesh buffers");
								Vulkan_Fence fence(0);
								constexpr VkPipelineStageFlags2 vk_aeInternalWaits[] = {VK_PIPELINE_STAGE_2_TRANSFER_BIT};
								if (fence.valid() && transferTask.submit(0, nullptr, vk_aeInternalWaits, 0, nullptr, fence.get())) {
									PRINT_DEBUG("Waiting for the temporary Vulkan fence to be signaled");
									fence.wait_for();
									return true;
								} else
									RE_FATAL_ERROR("Failed to submit the Vulkan task to the GPU for setting coordinates in the mesh buffers");
							} else
								RE_FATAL_ERROR("Failed to record the 2nd scope of the Vulkan task for the square meshes");
						} else
							RE_FATAL_ERROR("Failed to record the 1st scope of the Vulkan task for the square meshes");
					} else
						RE_FATAL_ERROR("Failed to bind memory to the 2D and 3D square-meshes");
					PRINT_DEBUG("Freeing memory used for the square-mesh buffers due to failure setting them up");
					squareMeshesMemory.free();
				} else
					RE_FATAL_ERROR("Failed to allocate memory for the 2D and 3D square-meshes");
				PRINT_DEBUG("Destroying 3D square mesh buffer due to failure setting them up");
				vkDestroyBuffer(vk_hDevice, square3D.vk_hMeshBuffer, nullptr);
			} else
				RE_FATAL_ERROR("Failed to create a mesh buffer for the 3D square in Vulkan");
			PRINT_DEBUG("Destroying 2D square mesh buffer due to failure setting them up");
			vkDestroyBuffer(vk_hDevice, square2D.vk_hMeshBuffer, nullptr);
		} else
			RE_FATAL_ERROR("Failed to create a mesh buffer for the 2D square in Vulkan");
		return false;
	}

	static void destroy_square_meshes() {
		PRINT_DEBUG("Destroying square meshes and freeing their memory");
		vkDestroyBuffer(vk_hDevice, square3D.vk_hMeshBuffer, nullptr);
		vkDestroyBuffer(vk_hDevice, square2D.vk_hMeshBuffer, nullptr);
		squareMeshesMemory.free();
	}

	bool init_renderer_meshes() {
		if (create_square_meshes()) {
			return true;
		}
		return false;
	}

	void destroy_renderer_meshes() {
		destroy_square_meshes();
	}

	[[nodiscard]]
	Mesh get_mesh_square_2D() {
		return reinterpret_cast<Mesh>(&square2D);
	}

	[[nodiscard]]
	Mesh get_mesh_square_3D() {
		return reinterpret_cast<Mesh>(&square3D);
	}

}
