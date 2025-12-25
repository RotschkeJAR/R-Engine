#include "RE_Renderer_Internal.hpp"
#include "RE_Vulkan_Wrappers.hpp"
#include "RE_ListBatch_GameObject.hpp"
#include "RE_List_GameObject.hpp"
#include "RE_Sprite.hpp"
#include "RE_Manager.hpp"

namespace RE {
	
	static Vulkan_Buffer stagingRenderBuffer;
	VkDrawIndexedIndirectCommand *vk_pRenderBufferDrawCommand;
	GameObjectInstanceData *paRenderBufferInstanceData;

	Vulkan_Buffer renderBuffer;

	size_t gameObjectCountRenderBuffer;
	std::atomic<uint32_t> gameObjectsToRenderCount;

	bool create_render_buffers() {
		gameObjectCountRenderBuffer = std::clamp<size_t>(gameObjectBatchList.size(), 3, std::numeric_limits<size_t>::max()) * MAXIMUM_GAME_OBJECTS_PER_BATCH;
		const VkDeviceSize vk_renderBufferByteSize = gameObjectCountRenderBuffer * sizeof(GameObjectInstanceData) + sizeof(VkDrawIndexedIndirectCommand);
		if (is_transfer_necessary()) {
			PRINT_DEBUG("Creating staging render buffer in Vulkan");
			if (stagingRenderBuffer.create(
					0,
					vk_renderBufferByteSize,
					VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
					1,
					nullptr,
					RE_VK_CPU_RAM,
					0)) {
				PRINT_DEBUG("Mapping to staging render buffer's memory");
				void *pStagingRenderBufferContent;
				if (stagingRenderBuffer.get_memory().map(0, 0, vk_renderBufferByteSize, &pStagingRenderBufferContent)) {
					vk_pRenderBufferDrawCommand = reinterpret_cast<VkDrawIndexedIndirectCommand*>(pStagingRenderBufferContent);
					paRenderBufferInstanceData = reinterpret_cast<GameObjectInstanceData*>(vk_pRenderBufferDrawCommand + 1);
					PRINT_DEBUG("Creating render buffer on GPU");
					const uint32_t a2u32QueuesForRenderBuffer[2] = {
						queueFamilyIndices[renderTasks[0].logical_queue_index_for_function(RENDER_TASK_SUBINDEX_BUFFER_TRANSFER)],
						queueFamilyIndices[renderTasks[0].logical_queue_index_for_function(RENDER_TASK_SUBINDEX_RENDERING)]
					};
					if (renderBuffer.create(
							0,
							vk_renderBufferByteSize,
							VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
							a2u32QueuesForRenderBuffer[0] == a2u32QueuesForRenderBuffer[1] ? 1 : 2,
							a2u32QueuesForRenderBuffer,
							RE_VK_GPU_RAM,
							0)) {
						Threadpool renderBufferInitializerThreads;
						std::atomic<size_t> index;
						for (uint32_t i = 0; i < renderBufferInitializerThreads.amount_of_threads(); i++)
							renderBufferInitializerThreads.execute([&]() {
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
												paRenderBufferInstanceData[ownIndex].a16fModelMatrix[u8MatrixIndex] = 1.0f;
												break;
											default:
												paRenderBufferInstanceData[ownIndex].a16fModelMatrix[u8MatrixIndex] = 0.0f;
										}
									paRenderBufferInstanceData[ownIndex].u32TextureId = 0x8000;
								}
							});
						return true;
					} else
						RE_FATAL_ERROR("Failed to create render buffer on GPU");
					stagingRenderBuffer.get_memory().unmap();
				} else
					RE_FATAL_ERROR("Failed to map CPU to staging render buffer");
				PRINT_DEBUG("Destroying staging render buffer due to failure creating render buffer on GPU");
				vkDestroyBuffer(vk_hDevice, stagingRenderBuffer, nullptr);
				stagingRenderBuffer.destroy();
			} else
				RE_FATAL_ERROR("Failed to create staging render buffer in Vulkan");
		} else {
			if (renderBuffer.create(
					0,
					vk_renderBufferByteSize,
					VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
					1,
					nullptr,
					RE_VK_SHARED_RAM,
					0)) {
				void *pRenderBufferContent;
				if (renderBuffer.get_memory().map(0, 0, vk_renderBufferByteSize, &pRenderBufferContent)) {
					vk_pRenderBufferDrawCommand = reinterpret_cast<VkDrawIndexedIndirectCommand*>(pRenderBufferContent);
					paRenderBufferInstanceData = reinterpret_cast<GameObjectInstanceData*>(vk_pRenderBufferDrawCommand + 1);
					Threadpool renderBufferInitializerThreads;
					std::atomic<size_t> index;
					for (uint32_t i = 0; i < renderBufferInitializerThreads.amount_of_threads(); i++)
						renderBufferInitializerThreads.execute([&]() {
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
											paRenderBufferInstanceData[ownIndex].a16fModelMatrix[u8MatrixIndex] = 1.0f;
											break;
										default:
											paRenderBufferInstanceData[ownIndex].a16fModelMatrix[u8MatrixIndex] = 0.0f;
									}
								paRenderBufferInstanceData[ownIndex].u32TextureId = 0x8000;
							}
						});
					bool bPrerecordingSuccessful = true;
					for (uint8_t u8FrameInFlightIndex = 0; u8FrameInFlightIndex < RE_VK_FRAMES_IN_FLIGHT; u8FrameInFlightIndex++)
						if (!renderTasks[u8FrameInFlightIndex].record(RENDER_TASK_SUBINDEX_BUFFER_TRANSFER, 0, [&](const VkCommandBuffer vk_hCommandBuffer, const uint8_t u8PreviousLogicalQueue, const uint8_t u8CurrentLogicalQueue, const uint8_t u8NextLogicalQueue) {})) {
							bPrerecordingSuccessful = false;
							break;
						}
					if (bPrerecordingSuccessful)
						return true;
					renderBufferInitializerThreads.join();
					renderBuffer.get_memory().unmap();
				}
				renderBuffer.destroy();
			}
		}
		return false;
	}

	void destroy_render_buffers() {
		PRINT_DEBUG("Destroying both staging and render buffer in Vulkan");
		renderBuffer.destroy();
		if (is_transfer_necessary()) {
			stagingRenderBuffer.get_memory().unmap();
			stagingRenderBuffer.destroy();
		}
	}

	bool record_cmd_transfer_buffer() {
		gameObjectsToRenderCount = 0;
		for (ListBatch_GameObject *const pBatch : gameObjectBatchList) {
			for (uint16_t u16Index = 0; u16Index < pBatch->size(); u16Index++) {
				const GameObject *const pGameObject = pBatch->at(u16Index);
				if (pGameObject->u32SceneParentId && pGameObject->u32SceneParentId != pCurrentScene->u32Id)
					continue;
				paRenderBufferInstanceData[gameObjectsToRenderCount].a16fModelMatrix[12] = pGameObject->transform.position[0];
				paRenderBufferInstanceData[gameObjectsToRenderCount].a16fModelMatrix[13] = pGameObject->transform.position[1];
				paRenderBufferInstanceData[gameObjectsToRenderCount].a16fModelMatrix[14] = pGameObject->transform.position[2];
				paRenderBufferInstanceData[gameObjectsToRenderCount].a16fModelMatrix[0] = pGameObject->transform.scale[0];
				paRenderBufferInstanceData[gameObjectsToRenderCount].a16fModelMatrix[5] = pGameObject->transform.scale[1];
				paRenderBufferInstanceData[gameObjectsToRenderCount].a16fModelMatrix[10] = pGameObject->transform.scale[2];
				paRenderBufferInstanceData[gameObjectsToRenderCount].a4fColor[0] = pGameObject->spriteRenderer.color[0];
				paRenderBufferInstanceData[gameObjectsToRenderCount].a4fColor[1] = pGameObject->spriteRenderer.color[1];
				paRenderBufferInstanceData[gameObjectsToRenderCount].a4fColor[2] = pGameObject->spriteRenderer.color[2];
				paRenderBufferInstanceData[gameObjectsToRenderCount].a4fColor[3] = pGameObject->spriteRenderer.color[3];
				paRenderBufferInstanceData[gameObjectsToRenderCount].u32TextureId = static_cast<uint32_t>(pGameObject->spriteRenderer.hSprite ? reinterpret_cast<const VulkanSprite*>(pGameObject->spriteRenderer.hSprite)->u16UniformIndex : 0x8000);
				gameObjectsToRenderCount++;
			}
		}
		vk_pRenderBufferDrawCommand->indexCount = 6;
		vk_pRenderBufferDrawCommand->instanceCount = gameObjectsToRenderCount;
		vk_pRenderBufferDrawCommand->firstIndex = 0;
		vk_pRenderBufferDrawCommand->vertexOffset = 0;
		vk_pRenderBufferDrawCommand->firstInstance = 0;
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
					.srcBuffer = stagingRenderBuffer.get(),
					.dstBuffer = renderBuffer.get(),
					.regionCount = 1,
					.pRegions = &vk_copyRenderBufferRegion
				};
				vkCmdCopyBuffer2(vk_hCommandBuffer, &vk_copyRenderBufferInfo);
			});
		else
			return true;
	}

}
