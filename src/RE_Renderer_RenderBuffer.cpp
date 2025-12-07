#include "RE_Renderer_Internal.hpp"
#include "RE_Vulkan_Wrapper Functions.hpp"
#include "RE_ListBatch_GameObject.hpp"
#include "RE_List_GameObject.hpp"
#include "RE_Sprite.hpp"

namespace RE {
	
	VkBuffer vk_hStagingRenderBuffer;
	VkDeviceMemory vk_hStagingRenderBufferMemory;
	VkDrawIndexedIndirectCommand *vk_pRenderBufferDrawCommand;
	GameObjectInstanceData *paRenderBufferInstanceData;

	VkBuffer vk_hRenderBuffer;
	VkDeviceMemory vk_hRenderBufferMemory;

	size_t gameObjectCountRenderBuffer;
	std::atomic<uint32_t> gameObjectToRenderCount;

	bool create_render_buffers() {
		PRINT_DEBUG("Creating staging render buffer in Vulkan");
		gameObjectCountRenderBuffer = std::clamp<size_t>(gameObjectBatchList.size(), 3, std::numeric_limits<size_t>::max()) * MAXIMUM_GAME_OBJECTS_PER_BATCH;
		const VkDeviceSize vk_renderBufferByteSize = gameObjectCountRenderBuffer * sizeof(GameObjectInstanceData) + sizeof(VkDrawIndexedIndirectCommand);
		if (create_vulkan_buffer(
				0,
				vk_renderBufferByteSize,
				VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
				1,
				nullptr,
				VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
				&vk_hStagingRenderBuffer,
				&vk_hStagingRenderBufferMemory)) {
			PRINT_DEBUG("Mapping to staging render buffer's memory");
			void *pStagingRenderBufferContent;
			vkMapMemory(vk_hDevice, vk_hStagingRenderBufferMemory, 0, vk_renderBufferByteSize, 0, &pStagingRenderBufferContent);
			vk_pRenderBufferDrawCommand = reinterpret_cast<VkDrawIndexedIndirectCommand*>(pStagingRenderBufferContent);
			paRenderBufferInstanceData = reinterpret_cast<GameObjectInstanceData*>(vk_pRenderBufferDrawCommand + 1);
			PRINT_DEBUG("Creating render buffer on GPU");
			const uint32_t a2u32QueuesForRenderBuffer[2] = {
				queueFamilyIndices[renderTasks[0].get_logical_queue_index_for_function(RENDER_TASK_SUBINDEX_BUFFER_TRANSFER)],
				queueFamilyIndices[renderTasks[0].get_logical_queue_index_for_function(RENDER_TASK_SUBINDEX_RENDERING)]
			};
			if (create_vulkan_buffer(
					0,
					vk_renderBufferByteSize,
					VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
					a2u32QueuesForRenderBuffer[0] == a2u32QueuesForRenderBuffer[1] ? 1 : 2,
					a2u32QueuesForRenderBuffer,
					VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
					&vk_hRenderBuffer,
					&vk_hRenderBufferMemory)) {
				Threadpool renderBufferInitializerThreads;
				std::atomic<size_t> index;
				for (uint32_t i = 0; i < renderBufferInitializerThreads.get_amount_of_threads(); i++)
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
			PRINT_DEBUG("Destroying staging render buffer due to failure creating render buffer on GPU");
			vkDestroyBuffer(vk_hDevice, vk_hStagingRenderBuffer, nullptr);
			vkFreeMemory(vk_hDevice, vk_hStagingRenderBufferMemory, nullptr);
		} else
			RE_FATAL_ERROR("Failed to create staging render buffer in Vulkan");
		return false;
	}

	void destroy_render_buffers() {
		PRINT_DEBUG("Destroying both staging and render buffer in Vulkan");
		vkDestroyBuffer(vk_hDevice, vk_hRenderBuffer, nullptr);
		vkFreeMemory(vk_hDevice, vk_hRenderBufferMemory, nullptr);
		vkUnmapMemory(vk_hDevice, vk_hStagingRenderBufferMemory);
		vkDestroyBuffer(vk_hDevice, vk_hStagingRenderBuffer, nullptr);
		vkFreeMemory(vk_hDevice, vk_hStagingRenderBufferMemory, nullptr);
	}

	bool record_cmd_transfer_buffer() {
		gameObjectToRenderCount = 0;
		for (ListBatch_GameObject *const pBatch : gameObjectBatchList) {
			for (uint16_t u16Index = 0; u16Index < pBatch->size(); u16Index++) {
				const GameObject *const pGameObject = pBatch->at(u16Index);
				paRenderBufferInstanceData[gameObjectToRenderCount].a16fModelMatrix[12] = pGameObject->transform.position[0];
				paRenderBufferInstanceData[gameObjectToRenderCount].a16fModelMatrix[13] = pGameObject->transform.position[1];
				paRenderBufferInstanceData[gameObjectToRenderCount].a16fModelMatrix[14] = pGameObject->transform.position[2];
				paRenderBufferInstanceData[gameObjectToRenderCount].a16fModelMatrix[0] = pGameObject->transform.scale[0];
				paRenderBufferInstanceData[gameObjectToRenderCount].a16fModelMatrix[5] = pGameObject->transform.scale[1];
				paRenderBufferInstanceData[gameObjectToRenderCount].a16fModelMatrix[10] = pGameObject->transform.scale[2];
				paRenderBufferInstanceData[gameObjectToRenderCount].a4fColor[0] = pGameObject->spriteRenderer.color[0];
				paRenderBufferInstanceData[gameObjectToRenderCount].a4fColor[1] = pGameObject->spriteRenderer.color[1];
				paRenderBufferInstanceData[gameObjectToRenderCount].a4fColor[2] = pGameObject->spriteRenderer.color[2];
				paRenderBufferInstanceData[gameObjectToRenderCount].a4fColor[3] = pGameObject->spriteRenderer.color[3];
				paRenderBufferInstanceData[gameObjectToRenderCount].u32TextureId = static_cast<uint32_t>(pGameObject->spriteRenderer.hSprite ? reinterpret_cast<const VulkanSprite*>(pGameObject->spriteRenderer.hSprite)->u16UniformIndex : 0x8000);
				gameObjectToRenderCount++;
			}
		}
		vk_pRenderBufferDrawCommand->indexCount = 6;
		vk_pRenderBufferDrawCommand->instanceCount = gameObjectToRenderCount;
		vk_pRenderBufferDrawCommand->firstIndex = 0;
		vk_pRenderBufferDrawCommand->vertexOffset = 0;
		vk_pRenderBufferDrawCommand->firstInstance = 0;
		return renderTasks[u8CurrentFrameInFlightIndex].record(RENDER_TASK_SUBINDEX_BUFFER_TRANSFER, VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT, [&](const VkCommandBuffer vk_hCommandBuffer, const uint8_t u8PreviousLogicalQueue, const uint8_t u8CurrentLogicalQueue, const uint8_t u8NextLogicalQueue) {
			const VkBufferCopy2 vk_copyRenderBufferRegion = {
				.sType = VK_STRUCTURE_TYPE_BUFFER_COPY_2,
				.srcOffset = 0,
				.dstOffset = 0,
				.size = sizeof(VkDrawIndexedIndirectCommand) + gameObjectToRenderCount * sizeof(GameObjectInstanceData),
			};
			const VkCopyBufferInfo2 vk_copyRenderBufferInfo = {
				.sType = VK_STRUCTURE_TYPE_COPY_BUFFER_INFO_2,
				.srcBuffer = vk_hStagingRenderBuffer,
				.dstBuffer = vk_hRenderBuffer,
				.regionCount = 1,
				.pRegions = &vk_copyRenderBufferRegion
			};
			vkCmdCopyBuffer2(vk_hCommandBuffer, &vk_copyRenderBufferInfo);
		});
	}

}
