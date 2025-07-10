#include "RE_RenderBatch_GameObject.hpp"
#include "RE_Renderer_GameObject.hpp"
#include "RE_Manager.hpp"
#include "RE_Vulkan_Wrapper Functions.hpp"
#include "RE_ListBatch_GameObject.hpp"

namespace RE {
	
	RenderBatch_GameObject::RenderBatch_GameObject(Batch_GameObject &rGameObjectBatch) : rGameObjectBatch(rGameObjectBatch), vk_hStagingVertexBuffer(VK_NULL_HANDLE), vk_ahVertexBuffers{}, vk_hStagingVertexBufferMemory(VK_NULL_HANDLE), vk_ahVertexBufferMemories{}, pafVertices(nullptr), vk_transparentVerticesOffsetBytes(0UL), u16TransparentCount(0U) {
		if (vk_hDevice)
			CATCH_SIGNAL(init());
	}
	
	RenderBatch_GameObject::~RenderBatch_GameObject() {
		if (vk_hDevice)
			CATCH_SIGNAL(destroy());
	}
	
	bool RenderBatch_GameObject::init() {
		constexpr uint32_t u32StagingVertexBufferQueueCount = 1U, au32StagingVertexBufferQueues[u32StagingVertexBufferQueueCount] = {RE_VK_QUEUE_TRANSFER_INDEX};
		if (create_vulkan_buffer(RE_VK_TRANSPARENT_GAME_OBJECT_VERTEX_BUFFER_SIZE_BYTES, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, u32StagingVertexBufferQueueCount, au32StagingVertexBufferQueues, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &vk_hStagingVertexBuffer, &vk_hStagingVertexBufferMemory)) {
			vkMapMemory(vk_hDevice, vk_hStagingVertexBufferMemory, 0UL, RE_VK_TRANSPARENT_GAME_OBJECT_VERTEX_BUFFER_SIZE_BYTES, 0, (void**) &pafVertices);
			constexpr uint32_t u32VertexBufferQueueCount = 2U, au32VertexBufferQueues[u32VertexBufferQueueCount] = {RE_VK_QUEUE_GRAPHICS_INDEX, RE_VK_QUEUE_TRANSFER_INDEX};
			uint16_t u16FrameInFlightCreateIndex = 0U;
			while (u16FrameInFlightCreateIndex < RE_VK_FRAMES_IN_FLIGHT) {
				if (!create_vulkan_buffer(RE_VK_TRANSPARENT_GAME_OBJECT_VERTEX_BUFFER_SIZE_BYTES, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, u32VertexBufferQueueCount, au32VertexBufferQueues, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &vk_ahVertexBuffers[u16FrameInFlightCreateIndex], &vk_ahVertexBufferMemories[u16FrameInFlightCreateIndex])) {
					RE_FATAL_ERROR(append_to_string("Failed to create Vulkan vertex buffer at frame in flight-index ", u16FrameInFlightCreateIndex, " for rendering game objects"));
					break;
				}
				u16FrameInFlightCreateIndex++;
			}
			if (u16FrameInFlightCreateIndex == RE_VK_FRAMES_IN_FLIGHT)
				return true;
			for (uint16_t u16FrameInFlightDeleteIndex = 0U; u16FrameInFlightDeleteIndex < u16FrameInFlightCreateIndex; u16FrameInFlightDeleteIndex++) {
				vkFreeMemory(vk_hDevice, vk_ahVertexBufferMemories[u16FrameInFlightDeleteIndex], nullptr);
				vkDestroyBuffer(vk_hDevice, vk_ahVertexBuffers[u16FrameInFlightDeleteIndex], nullptr);
				vk_ahVertexBufferMemories[u16FrameInFlightDeleteIndex] = VK_NULL_HANDLE;
				vk_ahVertexBuffers[u16FrameInFlightDeleteIndex] = VK_NULL_HANDLE;
			}
			vkUnmapMemory(vk_hDevice, vk_hStagingVertexBufferMemory);
			vkFreeMemory(vk_hDevice, vk_hStagingVertexBufferMemory, nullptr);
			vkDestroyBuffer(vk_hDevice, vk_hStagingVertexBuffer, nullptr);
		} else
			RE_FATAL_ERROR("Failed to create a Vulkan staging vertex buffer for transferring vertices to the GPU for rendering game objects");
		pafVertices = nullptr;
		vk_hStagingVertexBufferMemory = VK_NULL_HANDLE;
		vk_hStagingVertexBuffer = VK_NULL_HANDLE;
		return false;
	}
	
	void RenderBatch_GameObject::destroy() {
		for (uint16_t u16FrameInFlightDeleteIndex = 0U; u16FrameInFlightDeleteIndex < RE_VK_FRAMES_IN_FLIGHT; u16FrameInFlightDeleteIndex++) {
			vkFreeMemory(vk_hDevice, vk_ahVertexBufferMemories[u16FrameInFlightDeleteIndex], nullptr);
			vkDestroyBuffer(vk_hDevice, vk_ahVertexBuffers[u16FrameInFlightDeleteIndex], nullptr);
			vk_ahVertexBufferMemories[u16FrameInFlightDeleteIndex] = VK_NULL_HANDLE;
			vk_ahVertexBuffers[u16FrameInFlightDeleteIndex] = VK_NULL_HANDLE;
		}
		vkUnmapMemory(vk_hDevice, vk_hStagingVertexBufferMemory);
		vkFreeMemory(vk_hDevice, vk_hStagingVertexBufferMemory, nullptr);
		vkDestroyBuffer(vk_hDevice, vk_hStagingVertexBuffer, nullptr);
		pafVertices = nullptr;
		vk_hStagingVertexBufferMemory = VK_NULL_HANDLE;
		vk_hStagingVertexBuffer = VK_NULL_HANDLE;
	}
	
	void RenderBatch_GameObject::render_opaque() {
		u16TransparentCount = 0U;
		uint16_t u16OpaqueCount = 0U;
		for (uint16_t u16GameObjectIndex = 0U; u16GameObjectIndex < rGameObjectBatch.size(); u16GameObjectIndex++) {
			const GameObject *const pObject = rGameObjectBatch.get(u16GameObjectIndex);
			if (!is_object_active(pObject))
				continue;
			if (pObject->spriteRenderer.color[3] < 1.0f) {
				const uint32_t u32TransparentIndex = (RE_VK_RENDERABLE_RECTANGLES_COUNT - u16TransparentCount - 1U) * RE_VK_TRANSPARENT_GAME_OBJECT_VERTEX_TOTAL_SIZE * 4U;
				const float a2fTransparentObjectScale[2] = {
					pObject->transform.scale[0] / 2.0f,
					pObject->transform.scale[1] / 2.0f
				};
				for (uint8_t u8VertexIndex = 0U; u8VertexIndex < 4U; u8VertexIndex++) {
					const uint32_t u32VertexOffset = u32TransparentIndex + u8VertexIndex * RE_VK_TRANSPARENT_GAME_OBJECT_VERTEX_TOTAL_SIZE;
					switch (u8VertexIndex) {
						case 0U:
							pafVertices[u32VertexOffset + 0U] = pObject->transform.position[0] - a2fTransparentObjectScale[0];
							pafVertices[u32VertexOffset + 1U] = pObject->transform.position[1] + a2fTransparentObjectScale[1];
							break;
						case 1U:
							pafVertices[u32VertexOffset + 0U] = pObject->transform.position[0] + a2fTransparentObjectScale[0];
							pafVertices[u32VertexOffset + 1U] = pObject->transform.position[1] + a2fTransparentObjectScale[1];
							break;
						case 2U:
							pafVertices[u32VertexOffset + 0U] = pObject->transform.position[0] + a2fTransparentObjectScale[0];
							pafVertices[u32VertexOffset + 1U] = pObject->transform.position[1] - a2fTransparentObjectScale[1];
							break;
						case 3U:
							pafVertices[u32VertexOffset + 0U] = pObject->transform.position[0] - a2fTransparentObjectScale[0];
							pafVertices[u32VertexOffset + 1U] = pObject->transform.position[1] - a2fTransparentObjectScale[1];
							break;
					}
					pafVertices[u32VertexOffset + 2U] = pObject->transform.position[2];
					pafVertices[u32VertexOffset + 3U] = pObject->spriteRenderer.color[0];
					pafVertices[u32VertexOffset + 4U] = pObject->spriteRenderer.color[1];
					pafVertices[u32VertexOffset + 5U] = pObject->spriteRenderer.color[2];
					pafVertices[u32VertexOffset + 6U] = pObject->spriteRenderer.color[3];
				}
				u16TransparentCount++;
			} else {
				const uint32_t u32OpaqueIndex = u16OpaqueCount * RE_VK_OPAQUE_GAME_OBJECT_VERTEX_TOTAL_SIZE * 4U;
				const float a2fOpaqueObjectScale[2] = {
					pObject->transform.scale[0] / 2.0f,
					pObject->transform.scale[1] / 2.0f
				};
				for (uint8_t u8VertexIndex = 0U; u8VertexIndex < 4U; u8VertexIndex++) {
					const uint32_t u32VertexOffset = u32OpaqueIndex + u8VertexIndex * RE_VK_OPAQUE_GAME_OBJECT_VERTEX_TOTAL_SIZE;
					switch (u8VertexIndex) {
						case 0U:
							pafVertices[u32VertexOffset + 0U] = pObject->transform.position[0] - a2fOpaqueObjectScale[0];
							pafVertices[u32VertexOffset + 1U] = pObject->transform.position[1] + a2fOpaqueObjectScale[1];
							break;
						case 1U:
							pafVertices[u32VertexOffset + 0U] = pObject->transform.position[0] + a2fOpaqueObjectScale[0];
							pafVertices[u32VertexOffset + 1U] = pObject->transform.position[1] + a2fOpaqueObjectScale[1];
							break;
						case 2U:
							pafVertices[u32VertexOffset + 0U] = pObject->transform.position[0] + a2fOpaqueObjectScale[0];
							pafVertices[u32VertexOffset + 1U] = pObject->transform.position[1] - a2fOpaqueObjectScale[1];
							break;
						case 3U:
							pafVertices[u32VertexOffset + 0U] = pObject->transform.position[0] - a2fOpaqueObjectScale[0];
							pafVertices[u32VertexOffset + 1U] = pObject->transform.position[1] - a2fOpaqueObjectScale[1];
							break;
					}
					pafVertices[u32VertexOffset + 2U] = pObject->transform.position[2];
					pafVertices[u32VertexOffset + 3U] = pObject->spriteRenderer.color[0];
					pafVertices[u32VertexOffset + 4U] = pObject->spriteRenderer.color[1];
					pafVertices[u32VertexOffset + 5U] = pObject->spriteRenderer.color[2];
				}
				u16OpaqueCount++;
			}
		}

		const VkBufferCopy vk_a2VertexBufferCopyRegions[2] = {
			{
				.srcOffset = 0UL,
				.dstOffset = 0UL,
				.size = u16OpaqueCount * RE_VK_OPAQUE_GAME_OBJECT_VERTEX_TOTAL_SIZE_BYTES * 4U,
			}, {
				.srcOffset = (RE_VK_RENDERABLE_RECTANGLES_COUNT - u16TransparentCount) * RE_VK_TRANSPARENT_GAME_OBJECT_VERTEX_TOTAL_SIZE_BYTES * 4U,
				.dstOffset = vk_a2VertexBufferCopyRegions[0].size,
				.size = u16TransparentCount * RE_VK_TRANSPARENT_GAME_OBJECT_VERTEX_TOTAL_SIZE_BYTES * 4U,
			}
		};
		vk_transparentVerticesOffsetBytes = vk_a2VertexBufferCopyRegions[0].size;
		if (u16OpaqueCount && u16TransparentCount)
			vkCmdCopyBuffer(vk_ahGameObjectVertexTransferCommandBuffers[u8CurrentFrameInFlightIndex], vk_hStagingVertexBuffer, vk_ahVertexBuffers[u8CurrentFrameInFlightIndex], 2U, vk_a2VertexBufferCopyRegions);
		else if (u16OpaqueCount)
			vkCmdCopyBuffer(vk_ahGameObjectVertexTransferCommandBuffers[u8CurrentFrameInFlightIndex], vk_hStagingVertexBuffer, vk_ahVertexBuffers[u8CurrentFrameInFlightIndex], 1U, &vk_a2VertexBufferCopyRegions[0]);
		else if (u16TransparentCount)
			vkCmdCopyBuffer(vk_ahGameObjectVertexTransferCommandBuffers[u8CurrentFrameInFlightIndex], vk_hStagingVertexBuffer, vk_ahVertexBuffers[u8CurrentFrameInFlightIndex], 1U, &vk_a2VertexBufferCopyRegions[1]);
		if (u16OpaqueCount) {
			const VkDeviceSize vk_opaqueVertexBufferOffset = 0UL;
			vkCmdBindVertexBuffers(vk_ahGameObjectSecondaryCommandBuffers[u8CurrentFrameInFlightIndex], 0U, 1U, &vk_ahVertexBuffers[u8CurrentFrameInFlightIndex], &vk_opaqueVertexBufferOffset);
			vkCmdDrawIndexed(vk_ahGameObjectSecondaryCommandBuffers[u8CurrentFrameInFlightIndex], u16OpaqueCount * 6U, 1U, 0U, 0, 0U);
		}
	}

	void RenderBatch_GameObject::render_transparent() {
		if (!u16TransparentCount)
			return;
		vkCmdBindVertexBuffers(vk_ahGameObjectSecondaryCommandBuffers[u8CurrentFrameInFlightIndex], 0U, 1U, &vk_ahVertexBuffers[u8CurrentFrameInFlightIndex], &vk_transparentVerticesOffsetBytes);
		vkCmdDrawIndexed(vk_ahGameObjectSecondaryCommandBuffers[u8CurrentFrameInFlightIndex], u16TransparentCount * 6U, 1U, 0U, 0, 0U);
	}

}
