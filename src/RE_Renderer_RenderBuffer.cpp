#include "RE_Renderer_Internal.hpp"
#include "RE_Vulkan_Wrappers.hpp"
#include "RE_ListBatch_GameObject.hpp"
#include "RE_List_GameObject.hpp"
#include "RE_Sprite.hpp"
#include "RE_Manager.hpp"

namespace RE {
	
	static VkBuffer vk_ahStagingRenderBuffers[RE_VK_FRAMES_IN_FLIGHT];
	static VulkanMemory aStagingRenderBufferMemories[RE_VK_FRAMES_IN_FLIGHT];
	VkDrawIndexedIndirectCommand *vk_apRenderBufferDrawCommands[RE_VK_FRAMES_IN_FLIGHT];
	GameObjectInstanceData *apaRenderBufferInstanceData[RE_VK_FRAMES_IN_FLIGHT];

	VkBuffer vk_ahRenderBuffers[RE_VK_FRAMES_IN_FLIGHT];
	static VulkanMemory aRenderBufferMemories[RE_VK_FRAMES_IN_FLIGHT];

	size_t gameObjectCountRenderBuffer;
	std::atomic<uint32_t> gameObjectsToRenderCount;

	bool create_render_buffers() {
		gameObjectCountRenderBuffer = std::clamp<size_t>(gameObjectBatchList.size(), 3, std::numeric_limits<size_t>::max()) * MAXIMUM_GAME_OBJECTS_PER_BATCH;
		const VkDeviceSize vk_renderBufferByteSize = gameObjectCountRenderBuffer * sizeof(GameObjectInstanceData) + sizeof(VkDrawIndexedIndirectCommand);
		uint8_t u8RenderBufferCreateIndex = 0;
		while (u8RenderBufferCreateIndex < RE_VK_FRAMES_IN_FLIGHT) {
			if (is_transfer_necessary()) {
				PRINT_DEBUG("Creating staging render buffer in Vulkan at index ", u8RenderBufferCreateIndex);
				if (create_vulkan_buffer(
						0,
						vk_renderBufferByteSize,
						VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
						1,
						nullptr,
						RE_VK_CPU_RAM,
						0,
						&vk_ahStagingRenderBuffers[u8RenderBufferCreateIndex],
						&aStagingRenderBufferMemories[u8RenderBufferCreateIndex])) {
					PRINT_DEBUG("Mapping to staging render buffer's memory at index ", u8RenderBufferCreateIndex);
					void *pStagingRenderBufferContent;
					if (vkMapMemory(vk_hDevice, aStagingRenderBufferMemories[u8RenderBufferCreateIndex].get(), 0, vk_renderBufferByteSize, 0, &pStagingRenderBufferContent) == VK_SUCCESS) {
						vk_apRenderBufferDrawCommands[u8RenderBufferCreateIndex] = reinterpret_cast<VkDrawIndexedIndirectCommand*>(pStagingRenderBufferContent);
						apaRenderBufferInstanceData[u8RenderBufferCreateIndex] = reinterpret_cast<GameObjectInstanceData*>(vk_apRenderBufferDrawCommands[u8RenderBufferCreateIndex] + 1);
						const uint32_t a2u32QueuesForRenderBuffer[2] = {
							queueFamilyIndices[renderTasks[0].logical_queue_index_for_function(RENDER_TASK_SUBINDEX_BUFFER_TRANSFER)],
							queueFamilyIndices[renderTasks[0].logical_queue_index_for_function(RENDER_TASK_SUBINDEX_RENDERING)]
						};
						if (create_vulkan_buffer(
								0,
								vk_renderBufferByteSize,
								VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
								a2u32QueuesForRenderBuffer[0] == a2u32QueuesForRenderBuffer[1] ? 1 : 2,
								a2u32QueuesForRenderBuffer,
								RE_VK_GPU_RAM,
								0,
								&vk_ahRenderBuffers[u8RenderBufferCreateIndex],
								&aRenderBufferMemories[u8RenderBufferCreateIndex])) {
							Threadpool renderBufferInitializerThreads;
							std::atomic<size_t> index;
							for (uint32_t i = 0; i < renderBufferInitializerThreads.amount_of_threads(); i++)
								renderBufferInitializerThreads.execute(
										[&]() {
											while (true) {
												const size_t ownIndex = index.fetch_add(1);
												if (ownIndex >= gameObjectCountRenderBuffer)
													return;
												for (uint8_t u8MatrixIndex = 0; u8MatrixIndex < 16; u8MatrixIndex++)
													switch (u8MatrixIndex) {
														case 0:
														case 5:
														case 10:
														case 15:
															apaRenderBufferInstanceData[u8RenderBufferCreateIndex][ownIndex].a16fModelMatrix[u8MatrixIndex] = 1.0f;
															break;
														default:
															apaRenderBufferInstanceData[u8RenderBufferCreateIndex][ownIndex].a16fModelMatrix[u8MatrixIndex] = 0.0f;
													}
												apaRenderBufferInstanceData[u8RenderBufferCreateIndex][ownIndex].u32TextureId = 0x8000;
											}
										});
							u8RenderBufferCreateIndex++;
							continue;
						} else
							RE_FATAL_ERROR("Failed to create render buffer in Vulkan at index ", u8RenderBufferCreateIndex);
					} else
						RE_FATAL_ERROR("Failed mapping Vulkan memory of staging render buffer at index ", u8RenderBufferCreateIndex);
					vkDestroyBuffer(vk_hDevice, vk_ahStagingRenderBuffers[u8RenderBufferCreateIndex], nullptr);
					aStagingRenderBufferMemories[u8RenderBufferCreateIndex].free();
				} else
					RE_FATAL_ERROR("Failed to create staging render buffer in Vulkan at index ", u8RenderBufferCreateIndex);
			} else {
				if (create_vulkan_buffer(
						0,
						vk_renderBufferByteSize,
						VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
						1,
						nullptr,
						RE_VK_SHARED_RAM,
						0,
						&vk_ahRenderBuffers[u8RenderBufferCreateIndex],
						&aRenderBufferMemories[u8RenderBufferCreateIndex])) {
					void *pRenderBufferContent;
					if (vkMapMemory(vk_hDevice, aRenderBufferMemories[u8RenderBufferCreateIndex].get(), 0, vk_renderBufferByteSize, 0, &pRenderBufferContent) == VK_SUCCESS) {
						vk_apRenderBufferDrawCommands[u8RenderBufferCreateIndex] = reinterpret_cast<VkDrawIndexedIndirectCommand*>(pRenderBufferContent);
						apaRenderBufferInstanceData[u8RenderBufferCreateIndex] = reinterpret_cast<GameObjectInstanceData*>(vk_apRenderBufferDrawCommands[u8RenderBufferCreateIndex] + 1);
						Threadpool renderBufferInitializerThreads;
						std::atomic<size_t> index;
						for (uint32_t i = 0; i < renderBufferInitializerThreads.amount_of_threads(); i++)
							renderBufferInitializerThreads.execute(
									[&]() {
										while (true) {
											const size_t ownIndex = index.fetch_add(1);
											if (ownIndex >= gameObjectCountRenderBuffer)
												return;
											for (uint8_t u8MatrixIndex = 0; u8MatrixIndex < 16; u8MatrixIndex++)
												switch (u8MatrixIndex) {
													case 0:
													case 5:
													case 10:
													case 15:
														apaRenderBufferInstanceData[u8RenderBufferCreateIndex][ownIndex].a16fModelMatrix[u8MatrixIndex] = 1.0f;
														break;
													default:
														apaRenderBufferInstanceData[u8RenderBufferCreateIndex][ownIndex].a16fModelMatrix[u8MatrixIndex] = 0.0f;
												}
											apaRenderBufferInstanceData[u8RenderBufferCreateIndex][ownIndex].u32TextureId = 0x8000;
										}
									});
						if (renderTasks[u8RenderBufferCreateIndex].record(
								RENDER_TASK_SUBINDEX_BUFFER_TRANSFER, 
								0, 
								[&](const VkCommandBuffer vk_hCommandBuffer, const uint8_t u8PreviousLogicalQueue, const uint8_t u8CurrentLogicalQueue, const uint8_t u8NextLogicalQueue) {})) {
							renderBufferInitializerThreads.join();
							u8RenderBufferCreateIndex++;
							continue;
						} else
							RE_FATAL_ERROR("Failed prerecording Vulkan command buffer for render task at index ", u8RenderBufferCreateIndex);
					} else
						RE_FATAL_ERROR("Failed to map render buffer's memory at index ", u8RenderBufferCreateIndex);
					vkDestroyBuffer(vk_hDevice, vk_ahRenderBuffers[u8RenderBufferCreateIndex], nullptr);
					aRenderBufferMemories[u8RenderBufferCreateIndex].free();
				} else
					RE_FATAL_ERROR("Failed to create render buffer in Vulkan at index ", u8RenderBufferCreateIndex);
			}
			break;
		}
		if (u8RenderBufferCreateIndex < RE_VK_FRAMES_IN_FLIGHT) {
			for (uint8_t u8RenderBufferDestroyIndex = 0; u8RenderBufferDestroyIndex < u8RenderBufferCreateIndex; u8RenderBufferDestroyIndex++) {
				if (is_transfer_necessary()) {
					vkDestroyBuffer(vk_hDevice, vk_ahStagingRenderBuffers[u8RenderBufferDestroyIndex], nullptr);
					aStagingRenderBufferMemories[u8RenderBufferDestroyIndex].free();
				}
				vkDestroyBuffer(vk_hDevice, vk_ahRenderBuffers[u8RenderBufferDestroyIndex], nullptr);
				aRenderBufferMemories[u8RenderBufferDestroyIndex].free();
			}
			return false;
		}
		return true;
	}

	void destroy_render_buffers() {
		for (uint8_t u8RenderBufferDestroyIndex = 0; u8RenderBufferDestroyIndex < RE_VK_FRAMES_IN_FLIGHT; u8RenderBufferDestroyIndex++) {
			if (is_transfer_necessary()) {
				vkDestroyBuffer(vk_hDevice, vk_ahStagingRenderBuffers[u8RenderBufferDestroyIndex], nullptr);
				aStagingRenderBufferMemories[u8RenderBufferDestroyIndex].free();
			}
			vkDestroyBuffer(vk_hDevice, vk_ahRenderBuffers[u8RenderBufferDestroyIndex], nullptr);
			aRenderBufferMemories[u8RenderBufferDestroyIndex].free();
		}
	}

	bool record_cmd_transfer_buffer() {
		gameObjectsToRenderCount = 0;
		for (ListBatch_GameObject *const pBatch : gameObjectBatchList) {
			for (uint16_t u16Index = 0; u16Index < pBatch->size(); u16Index++) {
				const GameObject *const pGameObject = pBatch->at(u16Index);
				if (pGameObject->u32SceneParentId && pGameObject->u32SceneParentId != pCurrentScene->u32Id)
					continue;
				apaRenderBufferInstanceData[u8CurrentFrameInFlightIndex][gameObjectsToRenderCount].a16fModelMatrix[12] = pGameObject->transform.position[0];
				apaRenderBufferInstanceData[u8CurrentFrameInFlightIndex][gameObjectsToRenderCount].a16fModelMatrix[13] = pGameObject->transform.position[1];
				apaRenderBufferInstanceData[u8CurrentFrameInFlightIndex][gameObjectsToRenderCount].a16fModelMatrix[14] = pGameObject->transform.position[2];
				apaRenderBufferInstanceData[u8CurrentFrameInFlightIndex][gameObjectsToRenderCount].a16fModelMatrix[0] = pGameObject->transform.scale[0];
				apaRenderBufferInstanceData[u8CurrentFrameInFlightIndex][gameObjectsToRenderCount].a16fModelMatrix[5] = pGameObject->transform.scale[1];
				apaRenderBufferInstanceData[u8CurrentFrameInFlightIndex][gameObjectsToRenderCount].a16fModelMatrix[10] = pGameObject->transform.scale[2];
				apaRenderBufferInstanceData[u8CurrentFrameInFlightIndex][gameObjectsToRenderCount].a4fColor[0] = pGameObject->spriteRenderer.color[0];
				apaRenderBufferInstanceData[u8CurrentFrameInFlightIndex][gameObjectsToRenderCount].a4fColor[1] = pGameObject->spriteRenderer.color[1];
				apaRenderBufferInstanceData[u8CurrentFrameInFlightIndex][gameObjectsToRenderCount].a4fColor[2] = pGameObject->spriteRenderer.color[2];
				apaRenderBufferInstanceData[u8CurrentFrameInFlightIndex][gameObjectsToRenderCount].a4fColor[3] = pGameObject->spriteRenderer.color[3];
				apaRenderBufferInstanceData[u8CurrentFrameInFlightIndex][gameObjectsToRenderCount].u32TextureId = static_cast<uint32_t>(pGameObject->spriteRenderer.hSprite ? reinterpret_cast<const VulkanSprite*>(pGameObject->spriteRenderer.hSprite)->u16UniformIndex : 0x8000);
				gameObjectsToRenderCount++;
			}
		}
		vk_apRenderBufferDrawCommands[u8CurrentFrameInFlightIndex]->indexCount = 6;
		vk_apRenderBufferDrawCommands[u8CurrentFrameInFlightIndex]->instanceCount = gameObjectsToRenderCount;
		vk_apRenderBufferDrawCommands[u8CurrentFrameInFlightIndex]->firstIndex = 0;
		vk_apRenderBufferDrawCommands[u8CurrentFrameInFlightIndex]->vertexOffset = 0;
		vk_apRenderBufferDrawCommands[u8CurrentFrameInFlightIndex]->firstInstance = 0;
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
					.srcBuffer = vk_ahStagingRenderBuffers[u8CurrentFrameInFlightIndex],
					.dstBuffer = vk_ahRenderBuffers[u8CurrentFrameInFlightIndex],
					.regionCount = 1,
					.pRegions = &vk_copyRenderBufferRegion
				};
				vkCmdCopyBuffer2(vk_hCommandBuffer, &vk_copyRenderBufferInfo);
			});
		else
			return true;
	}

}
