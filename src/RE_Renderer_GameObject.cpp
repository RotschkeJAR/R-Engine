#include "RE_Renderer.hpp"
#include "RE_Render System.hpp"
#include "RE_Manager.hpp"

namespace RE {

	typedef float REvertex_t;
#define RE_VK_VERTEX_COUNT (RE_VK_RENDERABLE_RECTANGLES_COUNT * 4U)
#define RE_VK_VERTEX_POSITION_SIZE 3U
#define RE_VK_VERTEX_POSITION_SIZE_BYTES (RE_VK_VERTEX_POSITION_SIZE * sizeof(REvertex_t))
#define RE_VK_VERTEX_POSITION_OFFSET 0U
#define RE_VK_VERTEX_POSITION_OFFSET_BYTES (RE_VK_VERTEX_POSITION_OFFSET * sizeof(REvertex_t))
#define RE_VK_VERTEX_COLOR_SIZE 4U
#define RE_VK_VERTEX_COLOR_SIZE_BYTES (RE_VK_VERTEX_COLOR_SIZE * sizeof(REvertex_t))
#define RE_VK_VERTEX_COLOR_OFFSET RE_VK_VERTEX_POSITION_SIZE
#define RE_VK_VERTEX_COLOR_OFFSET_BYTES (RE_VK_VERTEX_COLOR_OFFSET * sizeof(REvertex_t))
#define RE_VK_VERTEX_TOTAL_SIZE (RE_VK_VERTEX_POSITION_SIZE + RE_VK_VERTEX_COLOR_SIZE)
#define RE_VK_VERTEX_TOTAL_SIZE_BYTES (RE_VK_VERTEX_TOTAL_SIZE * sizeof(REvertex_t))
#define RE_VK_VERTEX_BUFFER_BYTES (RE_VK_VERTEX_TOTAL_SIZE_BYTES * 4U * RE_VK_RENDERABLE_RECTANGLES_COUNT)

	constexpr uint32_t u32GameObjectsVertexInputBindingDescriptionCount = 1U;
	constexpr VkVertexInputBindingDescription vk_gameObjectsVertexInputBindingDescriptions[u32GameObjectsVertexInputBindingDescriptionCount] = {
		{
			.binding = 0U,
			.stride = RE_VK_VERTEX_TOTAL_SIZE_BYTES,
			.inputRate = VK_VERTEX_INPUT_RATE_VERTEX
		}
	};
	constexpr uint32_t u32GameObjectsVertexInputAttributeDescriptionCount = 2U;
	constexpr VkVertexInputAttributeDescription vk_gameObjectsVertexInputAttributeDescriptions[u32GameObjectsVertexInputAttributeDescriptionCount] = {
		{
			.location = 0U,
			.binding = 0U,
			.format = VK_FORMAT_R32G32B32_SFLOAT,
			.offset = RE_VK_VERTEX_POSITION_OFFSET_BYTES
		}, {
			.location = 1U,
			.binding = 0U,
			.format = VK_FORMAT_R32G32B32A32_SFLOAT,
			.offset = RE_VK_VERTEX_COLOR_OFFSET_BYTES
		}
	};
	constexpr uint32_t u32GameObjectVertexBufferQueueCount = 1U;
	constexpr uint32_t u32GameObjectVertexBufferQueues[u32GameObjectVertexBufferQueueCount] = {RE_VK_QUEUE_TRANSFER_INDEX};
	constexpr uint32_t u32GameObjectVertexStagingBufferQueueCount = 1U;
	constexpr uint32_t u32GameObjectVertexStagingBufferQueues[u32GameObjectVertexStagingBufferQueueCount] = {RE_VK_QUEUE_GRAPHICS_INDEX};
	
	Renderer_GameObject::Renderer_GameObject(const Vulkan_RenderPass *pRenderPass) : gameObjectsVertexShader("shaders/vertex.spv"), 
	gameObjectsFragmentShader("shaders/fragment.spv"), 
	gameObjectsGraphicsPipeline(&gameObjectsVertexShader, &gameObjectsFragmentShader, u32GameObjectsVertexInputBindingDescriptionCount, vk_gameObjectsVertexInputBindingDescriptions, u32GameObjectsVertexInputAttributeDescriptionCount, vk_gameObjectsVertexInputAttributeDescriptions, &gameObjectsPipelineLayout, pRenderPass), 
	gameObjectVertexBuffer(RE_VK_VERTEX_BUFFER_BYTES, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, u32GameObjectVertexBufferQueues, u32GameObjectVertexBufferQueueCount, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT), 
	gameObjectVertexStagingBuffer(RE_VK_VERTEX_BUFFER_BYTES, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, u32GameObjectVertexStagingBufferQueues, u32GameObjectVertexStagingBufferQueueCount, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT), 
#if (RE_VK_FRAMES_IN_FLIGHT == 2)
	vertexBufferTransferCommandBuffers{Vulkan_CommandBuffer(VK_COMMAND_BUFFER_LEVEL_PRIMARY, pCommandPools[RE_VK_COMMAND_POOL_TRANSFER_INDEX]), Vulkan_CommandBuffer(VK_COMMAND_BUFFER_LEVEL_PRIMARY, pCommandPools[RE_VK_COMMAND_POOL_TRANSFER_INDEX])}, 
#else
# error Update the array initialization above!
#endif
	u16GameObjectsToRenderCount(0U) {
		bool bVertexBufferTransferCommandBuffersSuccessfullyCreated = false;
		for (uint8_t u8CurrentTransferCommandBufferIndex = 0U; u8CurrentTransferCommandBufferIndex < RE_VK_FRAMES_IN_FLIGHT; u8CurrentTransferCommandBufferIndex++)
			if (vertexBufferTransferCommandBuffers[u8CurrentTransferCommandBufferIndex].is_valid()) {
				bVertexBufferTransferCommandBuffersSuccessfullyCreated = true;
				break;
			}
		if (!bVertexBufferTransferCommandBuffersSuccessfullyCreated || !gameObjectsVertexShader.is_valid() || !gameObjectsFragmentShader.is_valid() || !gameObjectsGraphicsPipeline.is_valid() || !gameObjectVertexBuffer.is_valid() || !gameObjectVertexStagingBuffer.is_valid()) {

		}
		gameObjectVertexStagingBuffer.map_memory((void**)&pVertices, 0UL, RE_VK_VERTEX_BUFFER_BYTES);
	}

	Renderer_GameObject::~Renderer_GameObject() {
		gameObjectVertexStagingBuffer.unmap_memory();
	}

	void Renderer_GameObject::record_secondary_command_buffer(const uint32_t u32CommandBufferIndex) const {
		CATCH_SIGNAL(ppSecondaryCommandBuffers[u32CommandBufferIndex]->begin_recording_command_buffer(VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT, &Renderer::pInstance->renderPass, 0U, Renderer::pInstance->ppFramebuffers[u32CommandBufferIndex]));
		if (u16GameObjectsToRenderCount) {
			CATCH_SIGNAL(ppSecondaryCommandBuffers[u32CommandBufferIndex]->cmd_bind_graphics_pipeline(&gameObjectsGraphicsPipeline));
			CATCH_SIGNAL(ppSecondaryCommandBuffers[u32CommandBufferIndex]->cmd_bind_index_buffer(&Renderer::pInstance->rectangleIndexBuffer, VK_INDEX_TYPE_UINT16));
			CATCH_SIGNAL(ppSecondaryCommandBuffers[u32CommandBufferIndex]->cmd_bind_vertex_buffer(&gameObjectVertexBuffer, 0UL));
			CATCH_SIGNAL(ppSecondaryCommandBuffers[u32CommandBufferIndex]->cmd_set_viewport(Renderer::pInstance->vk_cameraViewportArea));
			CATCH_SIGNAL(ppSecondaryCommandBuffers[u32CommandBufferIndex]->cmd_set_scissor(Renderer::pInstance->vk_cameraScissorArea));
			CATCH_SIGNAL(ppSecondaryCommandBuffers[u32CommandBufferIndex]->cmd_draw_indexed(u16GameObjectsToRenderCount * 6U, 0U, 0U));
		}
		CATCH_SIGNAL(ppSecondaryCommandBuffers[u32CommandBufferIndex]->end_recording_command_buffer());
	}

	void Renderer_GameObject::add_secondary_command_buffer(const Vulkan_CommandBuffer& primaryCommandBuffer, const uint32_t u32FramebufferIndex) const {
		CATCH_SIGNAL(primaryCommandBuffer.cmd_execute(ppSecondaryCommandBuffers[u32FramebufferIndex]));
	}

	void Renderer_GameObject::render(const uint32_t u32CurrentFramebufferIndex, const uint8_t u8CurrentFrameInFlight) {
		if (gameObjects.empty()) {
			CATCH_SIGNAL(pDeviceQueues[RE_VK_QUEUE_TRANSFER_INDEX]->submit_to_queue(0U, nullptr, nullptr, 0U, nullptr, 1U, &semaphoreWaitForVertexBufferTransfer[u8CurrentFrameInFlight], nullptr));
			return;
		}
		u16GameObjectsToRenderCount = 0U;
		for (GameObject *pObject : gameObjects) {
			if (u16GameObjectsToRenderCount == RE_VK_RENDERABLE_RECTANGLES_COUNT) {
				RE_ERROR("There are objects, which couldn't be rendered due to the memory limit");
				break;
			}
			DEFINE_SIGNAL_GUARD_DETAILED(sigGuardGameObjectRender, append_to_string("Rendering game object at ", pObject).c_str());
			if (!Manager::pInstance->is_object_active(pObject))
				continue;
			for (uint32_t u32CurrentGameObjectVertexIndex = 0U; u32CurrentGameObjectVertexIndex < 4U; u32CurrentGameObjectVertexIndex++) {
				const uint32_t u32OffsetInVertices = RE_VK_VERTEX_TOTAL_SIZE * 4U * u16GameObjectsToRenderCount + RE_VK_VERTEX_TOTAL_SIZE * u32CurrentGameObjectVertexIndex;
				const float vertexXOffset = pObject->transform.scale[0] / 2.0f;
				const float vertexYOffset = pObject->transform.scale[1] / 2.0f;
				switch (u32CurrentGameObjectVertexIndex) {
					case 0U:
						pVertices[u32OffsetInVertices + 0U] = pObject->transform.position[0] - vertexXOffset;
						pVertices[u32OffsetInVertices + 1U] = pObject->transform.position[1] + vertexYOffset;
						pVertices[u32OffsetInVertices + 2U] = pObject->transform.position[2];
						break;
					case 1U:
						pVertices[u32OffsetInVertices + 0U] = pObject->transform.position[0] + vertexXOffset;
						pVertices[u32OffsetInVertices + 1U] = pObject->transform.position[1] + vertexYOffset;
						pVertices[u32OffsetInVertices + 2U] = pObject->transform.position[2];
						break;
					case 2U:
						pVertices[u32OffsetInVertices + 0U] = pObject->transform.position[0] + vertexXOffset;
						pVertices[u32OffsetInVertices + 1U] = pObject->transform.position[1] - vertexYOffset;
						pVertices[u32OffsetInVertices + 2U] = pObject->transform.position[2];
						break;
					case 3U:
						pVertices[u32OffsetInVertices + 0U] = pObject->transform.position[0] - vertexXOffset;
						pVertices[u32OffsetInVertices + 1U] = pObject->transform.position[1] - vertexYOffset;
						pVertices[u32OffsetInVertices + 2U] = pObject->transform.position[2];
						break;
				}
				pVertices[u32OffsetInVertices + 3U] = pObject->spriteRenderer.color[0];
				pVertices[u32OffsetInVertices + 4U] = pObject->spriteRenderer.color[1];
				pVertices[u32OffsetInVertices + 5U] = pObject->spriteRenderer.color[2];
				pVertices[u32OffsetInVertices + 6U] = pObject->spriteRenderer.color[3];
			}
			u16GameObjectsToRenderCount++;
		}
		CATCH_SIGNAL(record_secondary_command_buffer(u32CurrentFramebufferIndex));
		if (!u16GameObjectsToRenderCount) {
			CATCH_SIGNAL(pDeviceQueues[RE_VK_QUEUE_TRANSFER_INDEX]->submit_to_queue(0U, nullptr, nullptr, 0U, nullptr, 1U, &semaphoreWaitForVertexBufferTransfer[u8CurrentFrameInFlight], nullptr));
			return;
		}
		CATCH_SIGNAL(vertexBufferTransferCommandBuffers[u8CurrentFrameInFlight].reset_command_buffer(0));
		CATCH_SIGNAL(vertexBufferTransferCommandBuffers[u8CurrentFrameInFlight].begin_recording_command_buffer(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT));
		CATCH_SIGNAL(vertexBufferTransferCommandBuffers[u8CurrentFrameInFlight].cmd_copy_buffer(&gameObjectVertexStagingBuffer, &gameObjectVertexBuffer, u16GameObjectsToRenderCount * 4U * RE_VK_VERTEX_TOTAL_SIZE_BYTES));
		CATCH_SIGNAL(vertexBufferTransferCommandBuffers[u8CurrentFrameInFlight].end_recording_command_buffer());
		CATCH_SIGNAL(pDeviceQueues[RE_VK_QUEUE_TRANSFER_INDEX]->submit_to_queue(0U, nullptr, nullptr, 1U, &vertexBufferTransferCommandBuffers[u8CurrentFrameInFlight], 1U, &semaphoreWaitForVertexBufferTransfer[u8CurrentFrameInFlight], nullptr));
	}

}
