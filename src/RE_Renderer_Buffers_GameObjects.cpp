#include "RE_Renderer_Buffers_Internal.hpp"
#include "RE_Vulkan_Wrappers.hpp"
#include "RE_ListBatch_GameObject.hpp"
#include "RE_List_GameObject.hpp"
#include "RE_Sprite.hpp"
#include "RE_Manager.hpp"

namespace RE {
	
	static Vulkan_Buffer aStagingRawGameObjectBuffers[RE_VK_FRAMES_IN_FLIGHT];
	VkDrawIndexedIndirectCommand *vk_apGameObjectBufferDrawCommands[RE_VK_FRAMES_IN_FLIGHT];
	static GameObjectInstanceData *apaGameObjectBufferInstanceData[RE_VK_FRAMES_IN_FLIGHT];

	Vulkan_Buffer aRawGameObjectBuffers[RE_VK_FRAMES_IN_FLIGHT];

	bool create_object_buffers() {
		const VkDeviceSize vk_objectBufferByteSize = gameObjectCounterBuffer * sizeof(GameObjectInstanceData) + sizeof(VkDrawIndexedIndirectCommand);
		uint8_t u8FrameInFlightCreateIndex = 0;
		while (u8FrameInFlightCreateIndex < RE_VK_FRAMES_IN_FLIGHT) {
			if (is_transfer_necessary()) {
				PRINT_DEBUG("Creating staging game object buffer in Vulkan at index ", u8FrameInFlightCreateIndex);
				if (aStagingRawGameObjectBuffers[u8FrameInFlightCreateIndex].create(
						0,
						vk_objectBufferByteSize,
						VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
						1,
						nullptr,
						RE_VK_CPU_RAM,
						0)) {
					PRINT_DEBUG("Mapping to staging game object buffer's memory at index ", u8FrameInFlightCreateIndex);
					void *pStagingRenderBufferContent;
					if (aStagingRawGameObjectBuffers[u8FrameInFlightCreateIndex].get_memory().map(0, 0, vk_objectBufferByteSize, &pStagingRenderBufferContent)) {
						PRINT_DEBUG("Querying queues the staging game object buffer will be used in");
						vk_apGameObjectBufferDrawCommands[u8FrameInFlightCreateIndex] = reinterpret_cast<VkDrawIndexedIndirectCommand*>(pStagingRenderBufferContent);
						apaGameObjectBufferInstanceData[u8FrameInFlightCreateIndex] = reinterpret_cast<GameObjectInstanceData*>(vk_apGameObjectBufferDrawCommands[u8FrameInFlightCreateIndex] + 1);
						constexpr uint32_t au32ObjectBufferQueues[] = {
							RENDER_TASK_SUBINDEX_BUFFER_TRANSFER, 
							RENDER_TASK_SUBINDEX_PROCESSING, 
							RENDER_TASK_SUBINDEX_RENDERING
						};
						const VulkanQueueCollection queuesForObjectBuffer = renderTasks[0].queues_of_functions(au32ObjectBufferQueues, sizeof(au32ObjectBufferQueues) / sizeof(au32ObjectBufferQueues[0]));
						PRINT_DEBUG("Creating raw game object buffer on GPU");
						if (aRawGameObjectBuffers[u8FrameInFlightCreateIndex].create(
								0,
								vk_objectBufferByteSize,
								VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
								queuesForObjectBuffer.u8QueueCount,
								queuesForObjectBuffer.queueFamilyIndices.get(),
								RE_VK_GPU_RAM,
								0)) {
							PRINT_DEBUG("Initializing staging game object buffer with default values");
							Threadpool gameObjectBufferInitializerThreads;
							std::atomic<size_t> index;
							for (uint32_t i = 0; i < gameObjectBufferInitializerThreads.amount_of_threads(); i++)
								gameObjectBufferInitializerThreads.execute(
										[&]() {
											while (true) {
												const size_t ownIndex = index.fetch_add(1);
												if (ownIndex >= gameObjectCounterBuffer)
													return;
												for (uint8_t u8MatrixIndex = 0; u8MatrixIndex < 16; u8MatrixIndex++)
													switch (u8MatrixIndex) {
														case 0:
														case 5:
														case 10:
														case 15:
															apaGameObjectBufferInstanceData[u8FrameInFlightCreateIndex][ownIndex].a16fModelMatrix[u8MatrixIndex] = 1.0f;
															break;
														default:
															apaGameObjectBufferInstanceData[u8FrameInFlightCreateIndex][ownIndex].a16fModelMatrix[u8MatrixIndex] = 0.0f;
													}
												apaGameObjectBufferInstanceData[u8FrameInFlightCreateIndex][ownIndex].u32TextureId = 0x8000;
											}
										});
							u8FrameInFlightCreateIndex++;
							continue;
						} else
							RE_FATAL_ERROR("Failed to create raw game object buffer in Vulkan at index ", u8FrameInFlightCreateIndex);
					} else
						RE_FATAL_ERROR("Failed mapping Vulkan memory of staging game object buffer at index ", u8FrameInFlightCreateIndex);
					PRINT_DEBUG("Destroying staging game object buffer at frame-in-flight index ", u8FrameInFlightCreateIndex, " due to failure mapping to its memory");
					aStagingRawGameObjectBuffers[u8FrameInFlightCreateIndex].destroy();
				} else
					RE_FATAL_ERROR("Failed to create staging game object buffer in Vulkan at index ", u8FrameInFlightCreateIndex);
			} else {
				PRINT_DEBUG("Creating raw game object buffer shared across CPU and GPU at frame-in-flight index ", u8FrameInFlightCreateIndex);
				constexpr uint32_t au32ObjectBufferQueues[] = {
					RENDER_TASK_SUBINDEX_PROCESSING, 
					RENDER_TASK_SUBINDEX_RENDERING
				};
				const VulkanQueueCollection queuesForObjectBuffer = renderTasks[0].queues_of_functions(au32ObjectBufferQueues, sizeof(au32ObjectBufferQueues) / sizeof(au32ObjectBufferQueues[0]));
				if (aRawGameObjectBuffers[u8FrameInFlightCreateIndex].create(
						0,
						vk_objectBufferByteSize,
						VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
						queuesForObjectBuffer.u8QueueCount,
						queuesForObjectBuffer.queueFamilyIndices.get(),
						RE_VK_SHARED_RAM,
						0)) {
					PRINT_DEBUG("Mapping memory of raw game object buffer at frame-in-flight index ", u8FrameInFlightCreateIndex, " to CPU");
					void *pRenderBufferContent;
					if (aRawGameObjectBuffers[u8FrameInFlightCreateIndex].get_memory().map(0, 0, vk_objectBufferByteSize, &pRenderBufferContent)) {
						PRINT_DEBUG("Initializing raw game object buffer at frame-in-flight index ", u8FrameInFlightCreateIndex, " with default values");
						vk_apGameObjectBufferDrawCommands[u8FrameInFlightCreateIndex] = reinterpret_cast<VkDrawIndexedIndirectCommand*>(pRenderBufferContent);
						apaGameObjectBufferInstanceData[u8FrameInFlightCreateIndex] = reinterpret_cast<GameObjectInstanceData*>(vk_apGameObjectBufferDrawCommands[u8FrameInFlightCreateIndex] + 1);
						Threadpool gameObjectBufferInitializerThreads;
						std::atomic<size_t> index;
						for (uint32_t i = 0; i < gameObjectBufferInitializerThreads.amount_of_threads(); i++)
							gameObjectBufferInitializerThreads.execute(
									[&]() {
										while (true) {
											const size_t ownIndex = index.fetch_add(1);
											if (ownIndex >= gameObjectCounterBuffer)
												return;
											for (uint8_t u8MatrixIndex = 0; u8MatrixIndex < 16; u8MatrixIndex++)
												switch (u8MatrixIndex) {
													case 0:
													case 5:
													case 10:
													case 15:
														apaGameObjectBufferInstanceData[u8FrameInFlightCreateIndex][ownIndex].a16fModelMatrix[u8MatrixIndex] = 1.0f;
														break;
													default:
														apaGameObjectBufferInstanceData[u8FrameInFlightCreateIndex][ownIndex].a16fModelMatrix[u8MatrixIndex] = 0.0f;
												}
											apaGameObjectBufferInstanceData[u8FrameInFlightCreateIndex][ownIndex].u32TextureId = 0x8000;
										}
									});
						PRINT_DEBUG("Prerecording command buffer of all render tasks");
						if (renderTasks[u8FrameInFlightCreateIndex].record(
								RENDER_TASK_SUBINDEX_BUFFER_TRANSFER, 
								0, 
								[&](const VkCommandBuffer vk_hCommandBuffer, const uint8_t u8PreviousLogicalQueue, const uint8_t u8CurrentLogicalQueue, const uint8_t u8NextLogicalQueue) {})) {
							gameObjectBufferInitializerThreads.join();
							u8FrameInFlightCreateIndex++;
							continue;
						} else
							RE_FATAL_ERROR("Failed prerecording Vulkan command buffer for render task at frame-in-flight index ", u8FrameInFlightCreateIndex);
					} else
						RE_FATAL_ERROR("Failed to map raw game object buffer's memory at index ", u8FrameInFlightCreateIndex);
					PRINT_DEBUG("Destroying raw game object buffer at frame-in-flight index ", u8FrameInFlightCreateIndex, " due to failure mapping to its memory");
					aRawGameObjectBuffers[u8FrameInFlightCreateIndex].destroy();
				} else
					RE_FATAL_ERROR("Failed to create raw game object buffer in Vulkan at index ", u8FrameInFlightCreateIndex);
			}
			break;
		}
		if (u8FrameInFlightCreateIndex == RE_VK_FRAMES_IN_FLIGHT)
			return true;
		for (uint8_t u8FrameInFlightDestroyIndex = 0; u8FrameInFlightDestroyIndex < u8FrameInFlightCreateIndex; u8FrameInFlightDestroyIndex++) {
			PRINT_DEBUG("Destroying game object buffers at frame-in-flight index ", u8FrameInFlightDestroyIndex, " due to failure setting them up");
			if (is_transfer_necessary())
				aStagingRawGameObjectBuffers[u8FrameInFlightDestroyIndex].destroy();
			aRawGameObjectBuffers[u8FrameInFlightDestroyIndex].destroy();
		}
		return false;
	}

	void destroy_object_buffers() {
		for (uint8_t u8FrameInFlightDestroyIndex = 0; u8FrameInFlightDestroyIndex < RE_VK_FRAMES_IN_FLIGHT; u8FrameInFlightDestroyIndex++) {
			PRINT_DEBUG("Destroying game object buffers at frame-in-flight index ", u8FrameInFlightDestroyIndex);
			if (is_transfer_necessary())
				aStagingRawGameObjectBuffers[u8FrameInFlightDestroyIndex].destroy();
			aRawGameObjectBuffers[u8FrameInFlightDestroyIndex].destroy();
		}
	}

	bool record_cmd_transfer_buffer() {
		gameObjectsToRenderCount = 0;
		for (ListBatch_GameObject *const pBatch : gameObjectBatchList) {
			for (uint16_t u16Index = 0; u16Index < pBatch->size(); u16Index++) {
				const GameObject *const pGameObject = pBatch->at(u16Index);
				if (pGameObject->u32SceneParentId && pGameObject->u32SceneParentId != pCurrentScene->u32Id)
					continue;
				apaGameObjectBufferInstanceData[u8CurrentFrameInFlightIndex][gameObjectsToRenderCount].a16fModelMatrix[12] = pGameObject->transform.position[0];
				apaGameObjectBufferInstanceData[u8CurrentFrameInFlightIndex][gameObjectsToRenderCount].a16fModelMatrix[13] = pGameObject->transform.position[1];
				apaGameObjectBufferInstanceData[u8CurrentFrameInFlightIndex][gameObjectsToRenderCount].a16fModelMatrix[14] = pGameObject->transform.position[2];
				apaGameObjectBufferInstanceData[u8CurrentFrameInFlightIndex][gameObjectsToRenderCount].a16fModelMatrix[0] = pGameObject->transform.scale[0];
				apaGameObjectBufferInstanceData[u8CurrentFrameInFlightIndex][gameObjectsToRenderCount].a16fModelMatrix[5] = pGameObject->transform.scale[1];
				apaGameObjectBufferInstanceData[u8CurrentFrameInFlightIndex][gameObjectsToRenderCount].a16fModelMatrix[10] = pGameObject->transform.scale[2];
				apaGameObjectBufferInstanceData[u8CurrentFrameInFlightIndex][gameObjectsToRenderCount].a4fColor[0] = pGameObject->spriteRenderer.color[0];
				apaGameObjectBufferInstanceData[u8CurrentFrameInFlightIndex][gameObjectsToRenderCount].a4fColor[1] = pGameObject->spriteRenderer.color[1];
				apaGameObjectBufferInstanceData[u8CurrentFrameInFlightIndex][gameObjectsToRenderCount].a4fColor[2] = pGameObject->spriteRenderer.color[2];
				apaGameObjectBufferInstanceData[u8CurrentFrameInFlightIndex][gameObjectsToRenderCount].a4fColor[3] = pGameObject->spriteRenderer.color[3];
				apaGameObjectBufferInstanceData[u8CurrentFrameInFlightIndex][gameObjectsToRenderCount].u32TextureId = static_cast<uint32_t>(pGameObject->spriteRenderer.hSprite ? reinterpret_cast<const VulkanSprite*>(pGameObject->spriteRenderer.hSprite)->u16UniformIndex : 0x8000);
				gameObjectsToRenderCount++;
			}
		}
		vk_apGameObjectBufferDrawCommands[u8CurrentFrameInFlightIndex]->indexCount = 6;
		vk_apGameObjectBufferDrawCommands[u8CurrentFrameInFlightIndex]->instanceCount = gameObjectsToRenderCount;
		vk_apGameObjectBufferDrawCommands[u8CurrentFrameInFlightIndex]->firstIndex = 0;
		vk_apGameObjectBufferDrawCommands[u8CurrentFrameInFlightIndex]->vertexOffset = 0;
		vk_apGameObjectBufferDrawCommands[u8CurrentFrameInFlightIndex]->firstInstance = 0;
		if (is_transfer_necessary())
			return renderTasks[u8CurrentFrameInFlightIndex].record(RENDER_TASK_SUBINDEX_BUFFER_TRANSFER, VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT, [&](const VkCommandBuffer vk_hCommandBuffer, const uint8_t u8PreviousLogicalQueue, const uint8_t u8CurrentLogicalQueue, const uint8_t u8NextLogicalQueue) {
						const VkBufferCopy2 vk_copyRenderBufferRegion = {
							.sType = VK_STRUCTURE_TYPE_BUFFER_COPY_2,
							.srcOffset = 0,
							.dstOffset = 0,
							.size = sizeof(VkDrawIndexedIndirectCommand) + gameObjectsToRenderCount * sizeof(GameObjectInstanceData),
						};
						const VkCopyBufferInfo2 vk_copyRenderBufferInfo = {
							.sType = VK_STRUCTURE_TYPE_COPY_BUFFER_INFO_2,
							.srcBuffer = aStagingRawGameObjectBuffers[u8CurrentFrameInFlightIndex].get(),
							.dstBuffer = aRawGameObjectBuffers[u8CurrentFrameInFlightIndex].get(),
							.regionCount = 1,
							.pRegions = &vk_copyRenderBufferRegion
						};
						vkCmdCopyBuffer2(vk_hCommandBuffer, &vk_copyRenderBufferInfo);
					});
		else
			return true;
	}

}
