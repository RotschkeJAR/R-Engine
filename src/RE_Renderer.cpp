#include "RE_Renderer.hpp"
#include "RE_Internal Header.hpp"
#include "RE_Render System.hpp"

namespace RE {

	typedef uint16_t REindex;
#define RE_VK_INDEX_BUFFER_SIZE RE_VK_RENDERABLE_OBJECTS_COUNT * 6U
#define RE_VK_INDEX_BUFFER_BYTES RE_VK_INDEX_BUFFER_SIZE * sizeof(REindex)

	SubRenderer::SubRenderer() : secondaryCommandBuffer(VK_COMMAND_BUFFER_LEVEL_SECONDARY, pCommandPools[RE_VK_COMMAND_POOL_GRAPHICS_INDEX]), bValid(false) {}
	SubRenderer::~SubRenderer() {}

	const Vulkan_CommandBuffer* SubRenderer::get_vulkan_command_buffer_ptr() const {
		return &secondaryCommandBuffer;
	}
	
	VkCommandBuffer SubRenderer::get_actual_command_buffer() const {
		return secondaryCommandBuffer.get_command_buffer();
	}

	bool SubRenderer::is_valid() const {
		return bValid;
	}

	SubRenderer::operator const Vulkan_CommandBuffer*() const {
		return this->get_vulkan_command_buffer_ptr();
	}

	SubRenderer::operator VkCommandBuffer() const {
		return this->get_actual_command_buffer();
	}



	constexpr uint32_t u32IndexBufferQueueTypes[] = {RE_VK_QUEUE_GRAPHICS_INDEX, RE_VK_QUEUE_TRANSFER_INDEX};

	Renderer::Renderer() : primaryCommandBuffer(VK_COMMAND_BUFFER_LEVEL_PRIMARY, pCommandPools[RE_VK_COMMAND_POOL_GRAPHICS_INDEX]), bValid(false), rectangleIndexBuffer(RE_VK_INDEX_BUFFER_BYTES, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, u32IndexBufferQueueTypes, sizeof(u32IndexBufferQueueTypes) / sizeof(u32IndexBufferQueueTypes[0]), VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT) {
		constexpr uint32_t u32IndexStagingBufferQueueTypes[] = {RE_VK_QUEUE_TRANSFER_INDEX};
		const Vulkan_Buffer indexStagingBuffer(RE_VK_INDEX_BUFFER_BYTES, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, u32IndexStagingBufferQueueTypes, sizeof(u32IndexStagingBufferQueueTypes) / sizeof(u32IndexStagingBufferQueueTypes[0]), VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
		REindex *const pIndices = new REindex[RE_VK_INDEX_BUFFER_SIZE];
		for (uint32_t u32IndicesGameObjectIndex = 0U; u32IndicesGameObjectIndex < RE_VK_RENDERABLE_OBJECTS_COUNT; u32IndicesGameObjectIndex++) {
			pIndices[u32IndicesGameObjectIndex + 0U] = u32IndicesGameObjectIndex * 4U + 0U;
			pIndices[u32IndicesGameObjectIndex + 1U] = u32IndicesGameObjectIndex * 4U + 1U;
			pIndices[u32IndicesGameObjectIndex + 2U] = u32IndicesGameObjectIndex * 4U + 2U;
			pIndices[u32IndicesGameObjectIndex + 3U] = u32IndicesGameObjectIndex * 4U + 1U;
			pIndices[u32IndicesGameObjectIndex + 4U] = u32IndicesGameObjectIndex * 4U + 2U;
			pIndices[u32IndicesGameObjectIndex + 5U] = u32IndicesGameObjectIndex * 4U + 3U;
		}
		CATCH_SIGNAL(indexStagingBuffer.upload_data(pIndices, 0UL, RE_VK_INDEX_BUFFER_BYTES));

		const Vulkan_CommandBuffer indexBufferTransferCommandBuffer(VK_COMMAND_BUFFER_LEVEL_PRIMARY, pCommandPools[RE_VK_COMMAND_POOL_TRANSFER_INDEX]);
		CATCH_SIGNAL(indexBufferTransferCommandBuffer.begin_recording_command_buffer(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT));
		CATCH_SIGNAL(indexBufferTransferCommandBuffer.cmd_copy_buffer(&indexStagingBuffer, &rectangleIndexBuffer, RE_VK_INDEX_BUFFER_BYTES));
		CATCH_SIGNAL(indexBufferTransferCommandBuffer.end_recording_command_buffer());
		VkPipelineStageFlags vk_ePipelineStagesToWaitFor[] = {VK_PIPELINE_STAGE_NONE};
		pDeviceQueues[RE_VK_QUEUE_TRANSFER_INDEX]->submit_to_queue(0U, nullptr, vk_ePipelineStagesToWaitFor, 1U, &indexBufferTransferCommandBuffer, 0U, nullptr, nullptr);

		primaryCommandBuffer.begin_recording_command_buffer(0);
		gameObjectRenderer.record_command_buffer(primaryCommandBuffer);
		primaryCommandBuffer.end_recording_command_buffer();

		pDeviceQueues[RE_VK_QUEUE_TRANSFER_INDEX]->wait_for_idle_queue();
		bValid = true;
	}
	Renderer::~Renderer() {}

	void Renderer::render() {
		
	}

	void Renderer::window_resize_event() {

	}

	bool Renderer::is_valid() const {
		return bValid;
	}

}
