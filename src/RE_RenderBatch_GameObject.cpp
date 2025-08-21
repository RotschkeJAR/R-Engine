#include "RE_RenderBatch_GameObject.hpp"
#include "RE_Renderer_GameObject.hpp"
#include "RE_Manager.hpp"
#include "RE_Vulkan_Wrapper Functions.hpp"
#include "RE_ListBatch_GameObject.hpp"
#include "RE_Renderer Texture.hpp"

namespace RE {
	
	RenderBatch_GameObject::RenderBatch_GameObject(ListBatch_GameObject &rGameObjectBatch) : rGameObjectBatch(rGameObjectBatch), vk_hStagingVertexBuffer(VK_NULL_HANDLE), vk_ahVertexBuffers{}, vk_hStagingVertexBufferMemory(VK_NULL_HANDLE), vk_ahVertexBufferMemories{}, pafVertices(nullptr), vk_transparentVerticesOffsetBytes(0) {
		if (vk_hDevice)
			PUSH_TO_CALLSTACKTRACE(init());
	}
	
	RenderBatch_GameObject::~RenderBatch_GameObject() {
		if (vk_hDevice)
			PUSH_TO_CALLSTACKTRACE(destroy());
	}
	
	bool RenderBatch_GameObject::init() {
		constexpr uint32_t u32StagingVertexBufferQueueCount = 1;
		const std::array<uint32_t, u32StagingVertexBufferQueueCount> au32StagingVertexBufferQueues = {RE_VK_QUEUE_TRANSFER_INDEX};
		if (create_vulkan_buffer(RE_VK_TRANSPARENT_GAME_OBJECT_VERTEX_BUFFER_SIZE_BYTES, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, u32StagingVertexBufferQueueCount, au32StagingVertexBufferQueues.data(), VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &vk_hStagingVertexBuffer, &vk_hStagingVertexBufferMemory)) {
			vkMapMemory(vk_hDevice, vk_hStagingVertexBufferMemory, 0, RE_VK_TRANSPARENT_GAME_OBJECT_VERTEX_BUFFER_SIZE_BYTES, 0, reinterpret_cast<void**>(&pafVertices));
			constexpr uint32_t u32VertexBufferQueueCount = 2;
			const std::array<uint32_t, u32VertexBufferQueueCount> au32VertexBufferQueues = {RE_VK_QUEUE_GRAPHICS_INDEX, RE_VK_QUEUE_TRANSFER_INDEX};
			uint16_t u16FrameInFlightCreateIndex = 0;
			while (u16FrameInFlightCreateIndex < RE_VK_FRAMES_IN_FLIGHT) {
				if (!create_vulkan_buffer(RE_VK_TRANSPARENT_GAME_OBJECT_VERTEX_BUFFER_SIZE_BYTES, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, u32VertexBufferQueueCount, au32VertexBufferQueues.data(), VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &vk_ahVertexBuffers[u16FrameInFlightCreateIndex], &vk_ahVertexBufferMemories[u16FrameInFlightCreateIndex])) {
					RE_FATAL_ERROR("Failed to create Vulkan vertex buffer at frame in flight-index ", u16FrameInFlightCreateIndex, " for rendering game objects");
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
		for (uint16_t u16FrameInFlightDeleteIndex = 0; u16FrameInFlightDeleteIndex < RE_VK_FRAMES_IN_FLIGHT; u16FrameInFlightDeleteIndex++) {
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
	
	void RenderBatch_GameObject::load_vertices(bool &rbNeedsRender) {
		u16OpaqueCount = 0;
		u16TransparentCount = 0;
		std::array<Sprite, RE_VK_RENDERABLE_RECTANGLES_COUNT> texturesToDraw;
		for (uint16_t u16GameObjectIndex = 0; u16GameObjectIndex < rGameObjectBatch.size(); u16GameObjectIndex++) {
			const GameObject *const pObject = rGameObjectBatch.get(u16GameObjectIndex);
			if (!PUSH_TO_CALLSTACKTRACE_AND_RETURN(is_object_active(pObject), bool))
				continue;
			float fTexId;
			if (pObject->spriteRenderer.sprite.hTexture && pObject->spriteRenderer.sprite.hSpriteLayout) {
				int16_t i16I;
				PUSH_TO_CALLSTACKTRACE(submit_sprite(&pObject->spriteRenderer.sprite, i16I));
				fTexId = static_cast<float>(i16I);
			} else
				fTexId = -1.0f;
			if (pObject->spriteRenderer.color[3] < 1.0f) {
				const uint32_t u32TransparentIndex = (RE_VK_RENDERABLE_RECTANGLES_COUNT - u16TransparentCount - 1U) * RE_VK_TRANSPARENT_GAME_OBJECT_VERTEX_TOTAL_SIZE * 4U;
				const float a2fTransparentObjectScale[2] = {
					pObject->transform.scale[0] / 2.0f,
					pObject->transform.scale[1] / 2.0f
				};
				for (uint8_t u8VertexIndex = 0; u8VertexIndex < 4; u8VertexIndex++) {
					const uint32_t u32VertexOffset = u32TransparentIndex + u8VertexIndex * RE_VK_TRANSPARENT_GAME_OBJECT_VERTEX_TOTAL_SIZE;
					switch (u8VertexIndex) {
						case 0:
							pafVertices[u32VertexOffset + 0] = pObject->transform.position[0] - a2fTransparentObjectScale[0];
							pafVertices[u32VertexOffset + 1] = pObject->transform.position[1] + a2fTransparentObjectScale[1];
							pafVertices[u32VertexOffset + 8] = pObject->spriteRenderer.textureOffset[0];
							pafVertices[u32VertexOffset + 9] = pObject->spriteRenderer.textureOffset[1];
							break;
						case 1:
							pafVertices[u32VertexOffset + 0] = pObject->transform.position[0] + a2fTransparentObjectScale[0];
							pafVertices[u32VertexOffset + 1] = pObject->transform.position[1] + a2fTransparentObjectScale[1];
							pafVertices[u32VertexOffset + 8] = pObject->spriteRenderer.textureOffset[0] + pObject->spriteRenderer.textureCoordinates[0];
							pafVertices[u32VertexOffset + 9] = pObject->spriteRenderer.textureOffset[1];
							break;
						case 2:
							pafVertices[u32VertexOffset + 0] = pObject->transform.position[0] + a2fTransparentObjectScale[0];
							pafVertices[u32VertexOffset + 1] = pObject->transform.position[1] - a2fTransparentObjectScale[1];
							pafVertices[u32VertexOffset + 8] = pObject->spriteRenderer.textureOffset[0] + pObject->spriteRenderer.textureCoordinates[0];
							pafVertices[u32VertexOffset + 9] = pObject->spriteRenderer.textureOffset[1] + pObject->spriteRenderer.textureCoordinates[1];
							break;
						case 3:
							pafVertices[u32VertexOffset + 0] = pObject->transform.position[0] - a2fTransparentObjectScale[0];
							pafVertices[u32VertexOffset + 1] = pObject->transform.position[1] - a2fTransparentObjectScale[1];
							pafVertices[u32VertexOffset + 8] = pObject->spriteRenderer.textureOffset[0];
							pafVertices[u32VertexOffset + 9] = pObject->spriteRenderer.textureOffset[1] + pObject->spriteRenderer.textureCoordinates[1];
							break;
					}
					pafVertices[u32VertexOffset + 2] = pObject->transform.position[2];
					pafVertices[u32VertexOffset + 3] = pObject->spriteRenderer.color[0];
					pafVertices[u32VertexOffset + 4] = pObject->spriteRenderer.color[1];
					pafVertices[u32VertexOffset + 5] = pObject->spriteRenderer.color[2];
					pafVertices[u32VertexOffset + 6] = pObject->spriteRenderer.color[3];
					pafVertices[u32VertexOffset + 7] = fTexId;
				}
				u16TransparentCount++;
			} else {
				const uint32_t u32OpaqueIndex = u16OpaqueCount * RE_VK_OPAQUE_GAME_OBJECT_VERTEX_TOTAL_SIZE * 4U;
				const float a2fOpaqueObjectScale[2] = {
					pObject->transform.scale[0] / 2.0f,
					pObject->transform.scale[1] / 2.0f
				};
				for (uint8_t u8VertexIndex = 0; u8VertexIndex < 4; u8VertexIndex++) {
					const uint32_t u32VertexOffset = u32OpaqueIndex + u8VertexIndex * RE_VK_OPAQUE_GAME_OBJECT_VERTEX_TOTAL_SIZE;
					switch (u8VertexIndex) {
						case 0:
							pafVertices[u32VertexOffset + 0] = pObject->transform.position[0] - a2fOpaqueObjectScale[0];
							pafVertices[u32VertexOffset + 1] = pObject->transform.position[1] + a2fOpaqueObjectScale[1];
							pafVertices[u32VertexOffset + 7] = pObject->spriteRenderer.textureOffset[0];
							pafVertices[u32VertexOffset + 8] = pObject->spriteRenderer.textureOffset[1];
							break;
						case 1:
							pafVertices[u32VertexOffset + 0] = pObject->transform.position[0] + a2fOpaqueObjectScale[0];
							pafVertices[u32VertexOffset + 1] = pObject->transform.position[1] + a2fOpaqueObjectScale[1];
							pafVertices[u32VertexOffset + 7] = pObject->spriteRenderer.textureOffset[0] + pObject->spriteRenderer.textureCoordinates[0];
							pafVertices[u32VertexOffset + 8] = pObject->spriteRenderer.textureOffset[1];
							break;
						case 2:
							pafVertices[u32VertexOffset + 0] = pObject->transform.position[0] + a2fOpaqueObjectScale[0];
							pafVertices[u32VertexOffset + 1] = pObject->transform.position[1] - a2fOpaqueObjectScale[1];
							pafVertices[u32VertexOffset + 7] = pObject->spriteRenderer.textureOffset[0] + pObject->spriteRenderer.textureCoordinates[0];
							pafVertices[u32VertexOffset + 8] = pObject->spriteRenderer.textureOffset[1] + pObject->spriteRenderer.textureCoordinates[1];
							break;
						case 3:
							pafVertices[u32VertexOffset + 0] = pObject->transform.position[0] - a2fOpaqueObjectScale[0];
							pafVertices[u32VertexOffset + 1] = pObject->transform.position[1] - a2fOpaqueObjectScale[1];
							pafVertices[u32VertexOffset + 7] = pObject->spriteRenderer.textureOffset[0];
							pafVertices[u32VertexOffset + 8] = pObject->spriteRenderer.textureOffset[1] + pObject->spriteRenderer.textureCoordinates[1];
							break;
					}
					pafVertices[u32VertexOffset + 2] = pObject->transform.position[2];
					pafVertices[u32VertexOffset + 3] = pObject->spriteRenderer.color[0];
					pafVertices[u32VertexOffset + 4] = pObject->spriteRenderer.color[1];
					pafVertices[u32VertexOffset + 5] = pObject->spriteRenderer.color[2];
					pafVertices[u32VertexOffset + 6] = fTexId;
				}
				u16OpaqueCount++;
			}
		}

		rbNeedsRender = u16OpaqueCount || u16TransparentCount;

		const VkBufferCopy vk_a2VertexBufferCopyRegions[2] = {
			{
				.srcOffset = 0,
				.dstOffset = 0,
				.size = u16OpaqueCount * RE_VK_OPAQUE_GAME_OBJECT_VERTEX_TOTAL_SIZE_BYTES * 4U,
			}, {
				.srcOffset = (RE_VK_RENDERABLE_RECTANGLES_COUNT - u16TransparentCount) * RE_VK_TRANSPARENT_GAME_OBJECT_VERTEX_TOTAL_SIZE_BYTES * 4U,
				.dstOffset = vk_a2VertexBufferCopyRegions[0].size,
				.size = u16TransparentCount * RE_VK_TRANSPARENT_GAME_OBJECT_VERTEX_TOTAL_SIZE_BYTES * 4U,
			}
		};
		vk_transparentVerticesOffsetBytes = vk_a2VertexBufferCopyRegions[0].size;
		if (u16OpaqueCount && u16TransparentCount)
			vkCmdCopyBuffer(vk_ahGameObjectVertexTransferCommandBuffers[u8CurrentFrameInFlightIndex], vk_hStagingVertexBuffer, vk_ahVertexBuffers[u8CurrentFrameInFlightIndex], 2, vk_a2VertexBufferCopyRegions);
		else if (u16OpaqueCount)
			vkCmdCopyBuffer(vk_ahGameObjectVertexTransferCommandBuffers[u8CurrentFrameInFlightIndex], vk_hStagingVertexBuffer, vk_ahVertexBuffers[u8CurrentFrameInFlightIndex], 1, &vk_a2VertexBufferCopyRegions[0]);
		else if (u16TransparentCount)
			vkCmdCopyBuffer(vk_ahGameObjectVertexTransferCommandBuffers[u8CurrentFrameInFlightIndex], vk_hStagingVertexBuffer, vk_ahVertexBuffers[u8CurrentFrameInFlightIndex], 1, &vk_a2VertexBufferCopyRegions[1]);
	}

	void RenderBatch_GameObject::render_opaque() {
		if (!u16OpaqueCount)
			return;
		constexpr VkDeviceSize vk_opaqueVertexBufferOffset = 0;
		vkCmdBindVertexBuffers(vk_ahGameObjectSecondaryCommandBuffers[u8CurrentFrameInFlightIndex], 0, 1, &vk_ahVertexBuffers[u8CurrentFrameInFlightIndex], &vk_opaqueVertexBufferOffset);
		vkCmdDrawIndexed(vk_ahGameObjectSecondaryCommandBuffers[u8CurrentFrameInFlightIndex], u16OpaqueCount * 6U, 1, 0, 0, 0);
	}

	void RenderBatch_GameObject::render_transparent() {
		if (!u16TransparentCount)
			return;
		vkCmdBindVertexBuffers(vk_ahGameObjectSecondaryCommandBuffers[u8CurrentFrameInFlightIndex], 0, 1, &vk_ahVertexBuffers[u8CurrentFrameInFlightIndex], &vk_transparentVerticesOffsetBytes);
		vkCmdDrawIndexed(vk_ahGameObjectSecondaryCommandBuffers[u8CurrentFrameInFlightIndex], u16TransparentCount * 6U, 1, 0, 0, 0);
	}

}
