#include "RE_Renderer.hpp"
#include "RE_Render System.hpp"
#include "RE_Manager.hpp"

namespace RE {

#define RE_VK_VERTEX_COUNT (RE_VK_RENDERABLE_RECTANGLES_COUNT * 4U)
#define RE_VK_VERTEX_POSITION_SIZE 3U
#define RE_VK_VERTEX_POSITION_SIZE_BYTES (RE_VK_VERTEX_POSITION_SIZE * sizeof(REgameObjectVertex_t))
#define RE_VK_VERTEX_POSITION_OFFSET 0U
#define RE_VK_VERTEX_POSITION_OFFSET_BYTES (RE_VK_VERTEX_POSITION_OFFSET * sizeof(REgameObjectVertex_t))
#define RE_VK_VERTEX_COLOR_SIZE 4U
#define RE_VK_VERTEX_COLOR_SIZE_BYTES (RE_VK_VERTEX_COLOR_SIZE * sizeof(REgameObjectVertex_t))
#define RE_VK_VERTEX_COLOR_OFFSET RE_VK_VERTEX_POSITION_SIZE
#define RE_VK_VERTEX_COLOR_OFFSET_BYTES (RE_VK_VERTEX_COLOR_OFFSET * sizeof(REgameObjectVertex_t))
#define RE_VK_VERTEX_TOTAL_SIZE (RE_VK_VERTEX_POSITION_SIZE + RE_VK_VERTEX_COLOR_SIZE)
#define RE_VK_VERTEX_TOTAL_SIZE_BYTES (RE_VK_VERTEX_TOTAL_SIZE * sizeof(REgameObjectVertex_t))
#define RE_VK_VERTEX_BUFFER_SIZE_BYTES (RE_VK_VERTEX_TOTAL_SIZE_BYTES * 4U * RE_VK_RENDERABLE_RECTANGLES_COUNT)

#define RE_VK_VIEW_MATRIX_SIZE (4U * 4U)
#define RE_VK_VIEW_MATRIX_OFFSET 0U
#define RE_VK_PROJECTION_MATRIX_SIZE (4U * 4U)
#define RE_VK_PROJECTION_MATRIX_OFFSET RE_VK_VIEW_MATRIX_SIZE
#define RE_VK_UNIFORM_BUFFER_SIZE (RE_VK_VIEW_MATRIX_SIZE + RE_VK_PROJECTION_MATRIX_SIZE)
#define RE_VK_UNIFORM_BUFFER_SIZE_BYTES (RE_VK_UNIFORM_BUFFER_SIZE * sizeof(float))

#define RE_VK_DESCRIPTOR_SET_LAYOUT_BINDING_COUNT 1U
#define RE_VK_DESCRIPTOR_POOL_SIZE 1U
#define RE_VK_VERTEX_INPUT_BINDING_DESCRIPTION_COUNT 1U
#define RE_VK_VERTEX_INPUT_ATTRIB_DESCRIPTION_COUNT 2U
#define RE_VK_VERTEX_BUFFER_QUEUE_COUNT 2U
#define RE_VK_VERTEX_STAGING_BUFFER_QUEUE_COUNT 1U
#define RE_VK_UNIFORM_BUFFER_QUEUE_COUNT 2U
#define RE_VK_UNIFORM_STAGING_BUFFER_QUEUE_COUNT 1U

	constexpr VkDescriptorSetLayoutBinding vk_descriptorSetLayoutBindings[RE_VK_DESCRIPTOR_SET_LAYOUT_BINDING_COUNT] = {
		{
			.binding = 0U,
			.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
			.descriptorCount = 1U,
			.stageFlags = VK_SHADER_STAGE_VERTEX_BIT,
			.pImmutableSamplers = nullptr
		}
	};
	constexpr VkDescriptorPoolSize vk_descriptorPoolSize[RE_VK_DESCRIPTOR_POOL_SIZE] = {
		{
			.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
			.descriptorCount = RE_VK_FRAMES_IN_FLIGHT
		}
	};
	constexpr VkVertexInputBindingDescription vk_vertexInputBindingDescriptions[RE_VK_VERTEX_INPUT_BINDING_DESCRIPTION_COUNT] = {
		{
			.binding = 0U,
			.stride = RE_VK_VERTEX_TOTAL_SIZE_BYTES,
			.inputRate = VK_VERTEX_INPUT_RATE_VERTEX
		}
	};
	constexpr VkVertexInputAttributeDescription vk_vertexInputAttributeDescriptions[RE_VK_VERTEX_INPUT_ATTRIB_DESCRIPTION_COUNT] = {
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
	constexpr uint32_t u32vertexBuffersQueues[RE_VK_VERTEX_BUFFER_QUEUE_COUNT] = {RE_VK_QUEUE_TRANSFER_INDEX, RE_VK_QUEUE_GRAPHICS_INDEX};
	constexpr uint32_t u32vertexStagingBufferQueues[RE_VK_VERTEX_STAGING_BUFFER_QUEUE_COUNT] = {RE_VK_QUEUE_TRANSFER_INDEX};
	constexpr uint32_t u32UniformBufferQueues[RE_VK_UNIFORM_BUFFER_QUEUE_COUNT] = {RE_VK_QUEUE_TRANSFER_INDEX, RE_VK_QUEUE_GRAPHICS_INDEX};
	constexpr uint32_t u32UniformStagingBufferQueues[RE_VK_UNIFORM_STAGING_BUFFER_QUEUE_COUNT] = {RE_VK_QUEUE_TRANSFER_INDEX};
	
	Renderer_GameObject::Renderer_GameObject(const Vulkan_RenderPass *pRenderPass) 
#if (RE_VK_FRAMES_IN_FLIGHT == 2)
	: vertexShader("shaders/vertex.spv"), 
	fragmentShader("shaders/fragment.spv"), 
	descriptorSetLayout(RE_VK_DESCRIPTOR_SET_LAYOUT_BINDING_COUNT, vk_descriptorSetLayoutBindings), 
	descriptorPool(0, RE_VK_DESCRIPTOR_POOL_SIZE, vk_descriptorPoolSize, RE_VK_FRAMES_IN_FLIGHT), 
	ppDescriptorSets(nullptr), 
	pipelineLayout(RE_VK_DESCRIPTOR_SET_LAYOUT_BINDING_COUNT, &descriptorSetLayout), 
	graphicsPipeline(&vertexShader, &fragmentShader, RE_VK_VERTEX_INPUT_BINDING_DESCRIPTION_COUNT, vk_vertexInputBindingDescriptions, RE_VK_VERTEX_INPUT_ATTRIB_DESCRIPTION_COUNT, vk_vertexInputAttributeDescriptions, &pipelineLayout, pRenderPass), 
	vertexBuffers{Vulkan_Buffer(RE_VK_VERTEX_BUFFER_SIZE_BYTES, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, u32vertexBuffersQueues, RE_VK_VERTEX_BUFFER_QUEUE_COUNT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT), Vulkan_Buffer(RE_VK_VERTEX_BUFFER_SIZE_BYTES, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, u32vertexBuffersQueues, RE_VK_VERTEX_BUFFER_QUEUE_COUNT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT)}, 
	vertexStagingBuffer(RE_VK_VERTEX_BUFFER_SIZE_BYTES, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, u32vertexStagingBufferQueues, RE_VK_VERTEX_STAGING_BUFFER_QUEUE_COUNT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT), 
	uniformBuffers{Vulkan_Buffer(RE_VK_UNIFORM_BUFFER_SIZE_BYTES, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, u32UniformBufferQueues, RE_VK_UNIFORM_BUFFER_QUEUE_COUNT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT), Vulkan_Buffer(RE_VK_UNIFORM_BUFFER_SIZE_BYTES, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, u32UniformBufferQueues, RE_VK_UNIFORM_BUFFER_QUEUE_COUNT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT)}, 
	uniformStagingBuffer(RE_VK_UNIFORM_BUFFER_SIZE_BYTES, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, u32UniformStagingBufferQueues, RE_VK_UNIFORM_STAGING_BUFFER_QUEUE_COUNT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT), 
	ppVertexBufferTransferCommandBuffers(nullptr), 
	pVertices(nullptr), 
	pfUniformBufferData(nullptr), 
	u16GameObjectsToRenderCount(0U) 
#else
# error Update the array initializations above!
#endif
	{
		if (!is_subrenderer_valid())
			return;
		bool bSuccessfullyCreated = true;
		for (uint8_t u8CurrentDeviceLocalBufferIndex = 0U; u8CurrentDeviceLocalBufferIndex < RE_VK_FRAMES_IN_FLIGHT; u8CurrentDeviceLocalBufferIndex++)
			if (!vertexBuffers[u8CurrentDeviceLocalBufferIndex].is_valid() || !uniformBuffers[u8CurrentDeviceLocalBufferIndex].is_valid()) {
				bSuccessfullyCreated = false;
				break;
			}
		if (!bSuccessfullyCreated || !vertexShader.is_valid() || !fragmentShader.is_valid() || !descriptorSetLayout.is_valid() || !pipelineLayout.is_valid() || !graphicsPipeline.is_valid() || !vertexStagingBuffer.is_valid() || !uniformStagingBuffer.is_valid()) {
			RE_FATAL_ERROR("Failed creating/allocating necessary ressources in Vulkan for rendering game objects");
			return;
		}
		ppDescriptorSets = new Vulkan_DescriptorSet*[RE_VK_FRAMES_IN_FLIGHT];
		if (!CATCH_SIGNAL_AND_RETURN(alloc_vk_descriptor_sets(&descriptorPool, &descriptorSetLayout, RE_VK_FRAMES_IN_FLIGHT, ppDescriptorSets), bool)) {
			RE_FATAL_ERROR("Failed allocating uniform buffer descriptor sets in Vulkan");
			DELETE_ARRAY_SAFELY(ppDescriptorSets);
			return;
		}
		ppVertexBufferTransferCommandBuffers = new Vulkan_CommandBuffer*[RE_VK_FRAMES_IN_FLIGHT];
		if (!CATCH_SIGNAL_AND_RETURN(alloc_vk_command_buffers(pCommandPools[RE_VK_COMMAND_POOL_TRANSFER_INDEX], VK_COMMAND_BUFFER_LEVEL_PRIMARY, RE_VK_FRAMES_IN_FLIGHT, ppVertexBufferTransferCommandBuffers), bool)) {
			RE_FATAL_ERROR("Failed allocating vertex buffer transfer command buffers in Vulkan");
			DELETE_ARRAY_SAFELY(ppVertexBufferTransferCommandBuffers);
			return;
		}
		vertexStagingBuffer.map_memory((void**)&pVertices, 0UL, RE_VK_VERTEX_BUFFER_SIZE_BYTES);
		uniformStagingBuffer.map_memory((void**)&pfUniformBufferData, 0UL, RE_VK_UNIFORM_BUFFER_SIZE_BYTES);
		std::fill(pfUniformBufferData, pfUniformBufferData + RE_VK_UNIFORM_BUFFER_SIZE_BYTES, 0.0f);
		pfUniformBufferData[0] = 1;
		pfUniformBufferData[5] = 1;
		pfUniformBufferData[10] = 1;
		pfUniformBufferData[15] = 1;
		pfUniformBufferData[16] = 1;
		pfUniformBufferData[21] = 1;
		pfUniformBufferData[26] = 1;
		pfUniformBufferData[31] = 1;
	}

	Renderer_GameObject::~Renderer_GameObject() {
		vertexStagingBuffer.unmap_memory();
		uniformStagingBuffer.unmap_memory();
		if (ppVertexBufferTransferCommandBuffers) {
			CATCH_SIGNAL(free_vk_command_buffers(RE_VK_FRAMES_IN_FLIGHT, ppVertexBufferTransferCommandBuffers));
			delete[] ppVertexBufferTransferCommandBuffers;
		}
	}

	void Renderer_GameObject::record_secondary_command_buffer(const uint32_t u32CurrentFramebufferIndex, const uint8_t u8CurrentFrameInFlight) const {
		CATCH_SIGNAL(ppSecondaryCommandBuffers[u32CurrentFramebufferIndex]->begin_recording_command_buffer(VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT, &Renderer::pInstance->renderPass, 0U, Renderer::pInstance->ppFramebuffers[u32CurrentFramebufferIndex]));
		if (u16GameObjectsToRenderCount) {
			CATCH_SIGNAL(ppSecondaryCommandBuffers[u32CurrentFramebufferIndex]->cmd_bind_graphics_pipeline(&graphicsPipeline));
			CATCH_SIGNAL(ppSecondaryCommandBuffers[u32CurrentFramebufferIndex]->cmd_bind_index_buffer(&Renderer::pInstance->rectangleIndexBuffer, VK_INDEX_TYPE_UINT16));
			CATCH_SIGNAL(ppSecondaryCommandBuffers[u32CurrentFramebufferIndex]->cmd_bind_vertex_buffer(&vertexBuffers[u8CurrentFrameInFlight], 0UL));
			CATCH_SIGNAL(ppSecondaryCommandBuffers[u32CurrentFramebufferIndex]->cmd_bind_descriptor_set(VK_PIPELINE_BIND_POINT_GRAPHICS, &pipelineLayout, ppDescriptorSets[u8CurrentFrameInFlight]));
			CATCH_SIGNAL(ppSecondaryCommandBuffers[u32CurrentFramebufferIndex]->cmd_set_viewport(Renderer::pInstance->vk_cameraViewportArea));
			CATCH_SIGNAL(ppSecondaryCommandBuffers[u32CurrentFramebufferIndex]->cmd_set_scissor(Renderer::pInstance->vk_cameraScissorArea));
			CATCH_SIGNAL(ppSecondaryCommandBuffers[u32CurrentFramebufferIndex]->cmd_draw_indexed(u16GameObjectsToRenderCount * 6U, 0U, 0U));
		}
		CATCH_SIGNAL(ppSecondaryCommandBuffers[u32CurrentFramebufferIndex]->end_recording_command_buffer());
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
				const Vector2f vertexOffset(pObject->transform.scale[0] / 2.0f, pObject->transform.scale[1] / 2.0f);
				switch (u32CurrentGameObjectVertexIndex) {
					case 0U:
						pVertices[u32OffsetInVertices + 0U] = pObject->transform.position[0] - vertexOffset[0];
						pVertices[u32OffsetInVertices + 1U] = pObject->transform.position[1] + vertexOffset[1];
						pVertices[u32OffsetInVertices + 2U] = pObject->transform.position[2];
						break;
					case 1U:
						pVertices[u32OffsetInVertices + 0U] = pObject->transform.position[0] + vertexOffset[0];
						pVertices[u32OffsetInVertices + 1U] = pObject->transform.position[1] + vertexOffset[1];
						pVertices[u32OffsetInVertices + 2U] = pObject->transform.position[2];
						break;
					case 2U:
						pVertices[u32OffsetInVertices + 0U] = pObject->transform.position[0] + vertexOffset[0];
						pVertices[u32OffsetInVertices + 1U] = pObject->transform.position[1] - vertexOffset[1];
						pVertices[u32OffsetInVertices + 2U] = pObject->transform.position[2];
						break;
					case 3U:
						pVertices[u32OffsetInVertices + 0U] = pObject->transform.position[0] - vertexOffset[0];
						pVertices[u32OffsetInVertices + 1U] = pObject->transform.position[1] - vertexOffset[1];
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
		if (u16GameObjectsToRenderCount) {
			if (pActiveCamera) {
				// TODO: Create view matrix

				// Orthographic matrix calculation
				const float fOrthoCamLeft = pActiveCamera->position[0] - pActiveCamera->scale[0] / 2.0f, fOrthoCamRight = pActiveCamera->position[0] + pActiveCamera->scale[0] / 2.0f, fOrthoCamBottom = pActiveCamera->position[1] - pActiveCamera->scale[1] / 2.0f, fOrthoCamTop = pActiveCamera->position[1] + pActiveCamera->scale[1] / 2.0f, fOrthoCamWidth = fOrthoCamRight - fOrthoCamLeft, fOrthoCamHeight = fOrthoCamTop - fOrthoCamBottom;
				constexpr float fOrthoCamFar = -100.0f, fOrthoCamNear = 100.0f, fOrthoCamDistance = fOrthoCamFar - fOrthoCamNear;
				pfUniformBufferData[16] = 2.0f / fOrthoCamWidth;
				pfUniformBufferData[21] = 2.0f / fOrthoCamHeight;
				pfUniformBufferData[26] = 2.0f / fOrthoCamDistance;
				pfUniformBufferData[28] = -(fOrthoCamRight + fOrthoCamLeft) / fOrthoCamWidth;
				pfUniformBufferData[29] = -(fOrthoCamTop + fOrthoCamBottom) / fOrthoCamHeight;
				pfUniformBufferData[30] = -(fOrthoCamFar + fOrthoCamNear) / fOrthoCamDistance;
			}
			CATCH_SIGNAL(ppDescriptorSets[u8CurrentFrameInFlight]->update_set_with_buffer(&uniformBuffers[u8CurrentFrameInFlight], 0UL, RE_VK_UNIFORM_BUFFER_SIZE_BYTES, 0U, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER));
		}
		CATCH_SIGNAL(record_secondary_command_buffer(u32CurrentFramebufferIndex, u8CurrentFrameInFlight));
		if (!u16GameObjectsToRenderCount) {
			CATCH_SIGNAL(pDeviceQueues[RE_VK_QUEUE_TRANSFER_INDEX]->submit_to_queue(0U, nullptr, nullptr, 0U, nullptr, 1U, &semaphoreWaitForVertexBufferTransfer[u8CurrentFrameInFlight], nullptr));
			return;
		}
		CATCH_SIGNAL(ppVertexBufferTransferCommandBuffers[u8CurrentFrameInFlight]->reset_command_buffer(0));
		CATCH_SIGNAL(ppVertexBufferTransferCommandBuffers[u8CurrentFrameInFlight]->begin_recording_command_buffer(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT));
		CATCH_SIGNAL(ppVertexBufferTransferCommandBuffers[u8CurrentFrameInFlight]->cmd_copy_buffer(&vertexStagingBuffer, &vertexBuffers[u8CurrentFrameInFlight], u16GameObjectsToRenderCount * 4U * RE_VK_VERTEX_TOTAL_SIZE_BYTES));
		CATCH_SIGNAL(ppVertexBufferTransferCommandBuffers[u8CurrentFrameInFlight]->cmd_copy_buffer(&uniformStagingBuffer, &uniformBuffers[u8CurrentFrameInFlight], RE_VK_UNIFORM_BUFFER_SIZE_BYTES));
		CATCH_SIGNAL(ppVertexBufferTransferCommandBuffers[u8CurrentFrameInFlight]->end_recording_command_buffer());
		CATCH_SIGNAL(pDeviceQueues[RE_VK_QUEUE_TRANSFER_INDEX]->submit_to_queue(0U, nullptr, nullptr, 1U, ppVertexBufferTransferCommandBuffers[u8CurrentFrameInFlight], 1U, &semaphoreWaitForVertexBufferTransfer[u8CurrentFrameInFlight], nullptr));
	}

	bool Renderer_GameObject::is_valid() const {
		if (!is_subrenderer_valid())
			return false;
		if (!ppDescriptorSets || !ppVertexBufferTransferCommandBuffers)
			return false;
		for (uint8_t u8FrameInFlightIndex = 0U; u8FrameInFlightIndex < RE_VK_FRAMES_IN_FLIGHT; u8FrameInFlightIndex++)
			if (!ppDescriptorSets[u8FrameInFlightIndex]->is_valid() || !vertexBuffers[u8FrameInFlightIndex].is_valid() || !uniformBuffers[u8FrameInFlightIndex].is_valid() || !ppVertexBufferTransferCommandBuffers[u8FrameInFlightIndex]->is_valid())
				return false;
		return vertexShader.is_valid() && fragmentShader.is_valid() && descriptorSetLayout.is_valid() && descriptorPool.is_valid() && pipelineLayout.is_valid() && graphicsPipeline.is_valid() && vertexStagingBuffer.is_valid() && uniformStagingBuffer.is_valid() && pVertices && pfUniformBufferData;
	}

}
