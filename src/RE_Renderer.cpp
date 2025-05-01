#include "RE_Renderer.hpp"
#include "RE_Internal Header.hpp"
#include "RE_Render System.hpp"

namespace RE {

	typedef uint16_t REindex_t;
#define RE_VK_INDEX_BUFFER_SIZE RE_VK_RENDERABLE_RECTANGLES_COUNT * 6U
#define RE_VK_INDEX_BUFFER_BYTES RE_VK_INDEX_BUFFER_SIZE * sizeof(REindex_t)

	SubRenderer::SubRenderer() : ppSecondaryCommandBuffers(nullptr), bValid(false) {
		ppSecondaryCommandBuffers = new Vulkan_CommandBuffer*[u32SwapchainImageCount];
		if (!CATCH_SIGNAL_AND_RETURN(alloc_vk_command_buffers(VK_COMMAND_BUFFER_LEVEL_SECONDARY, pCommandPools[RE_VK_COMMAND_POOL_GRAPHICS_INDEX], u32SwapchainImageCount, ppSecondaryCommandBuffers), bool)) {
			RE_FATAL_ERROR("Failed to allocate secondary command buffers for subrenderer");
			DELETE_ARRAY_SAFELY(ppSecondaryCommandBuffers);
			return;
		}
	}

	SubRenderer::~SubRenderer() {
		if (ppSecondaryCommandBuffers)
			CATCH_SIGNAL(free_vk_command_buffers(pCommandPools[RE_VK_COMMAND_POOL_GRAPHICS_INDEX], u32SwapchainImageCount, ppSecondaryCommandBuffers));
		DELETE_ARRAY_SAFELY(ppSecondaryCommandBuffers);
	}

	bool SubRenderer::is_valid() const {
		return bValid;
	}



	constexpr uint32_t u32IndexBufferQueueTypes[] = {RE_VK_QUEUE_GRAPHICS_INDEX, RE_VK_QUEUE_TRANSFER_INDEX};
	Camera *pActiveCamera = nullptr;
	Renderer *Renderer::pInstance = nullptr;

	Renderer::Renderer() : ppPrimaryCommandBuffer(nullptr), 
	gameObjectRenderer(&renderPass), 
	bValid(false), 
	rectangleIndexBuffer(RE_VK_INDEX_BUFFER_BYTES, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, u32IndexBufferQueueTypes, sizeof(u32IndexBufferQueueTypes) / sizeof(u32IndexBufferQueueTypes[0]), VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT), 
	renderFence(VK_FENCE_CREATE_SIGNALED_BIT), 
	ppFramebuffers(nullptr), 
	vk_maxViewportArea({}), 
	vk_maxScissorArea({}) {
		if (pInstance) {
			RE_FATAL_ERROR("Another instance of Renderer has been constructed.");
			return;
		}
		pInstance = this;
		constexpr uint32_t u32IndexStagingBufferQueueTypes[] = {RE_VK_QUEUE_TRANSFER_INDEX};
		const Vulkan_Buffer indexStagingBuffer(RE_VK_INDEX_BUFFER_BYTES, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, u32IndexStagingBufferQueueTypes, sizeof(u32IndexStagingBufferQueueTypes) / sizeof(u32IndexStagingBufferQueueTypes[0]), VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
		REindex_t indices[RE_VK_INDEX_BUFFER_SIZE];
		for (uint32_t u32IndicesGameObjectIndex = 0U; u32IndicesGameObjectIndex < RE_VK_RENDERABLE_RECTANGLES_COUNT; u32IndicesGameObjectIndex++) {
			indices[u32IndicesGameObjectIndex * 6U + 0U] = u32IndicesGameObjectIndex * 4U + 0U;
			indices[u32IndicesGameObjectIndex * 6U + 1U] = u32IndicesGameObjectIndex * 4U + 1U;
			indices[u32IndicesGameObjectIndex * 6U + 2U] = u32IndicesGameObjectIndex * 4U + 2U;
			indices[u32IndicesGameObjectIndex * 6U + 3U] = u32IndicesGameObjectIndex * 4U + 0U;
			indices[u32IndicesGameObjectIndex * 6U + 4U] = u32IndicesGameObjectIndex * 4U + 2U;
			indices[u32IndicesGameObjectIndex * 6U + 5U] = u32IndicesGameObjectIndex * 4U + 3U;
		}
		CATCH_SIGNAL(indexStagingBuffer.upload_data(&indices, 0UL, RE_VK_INDEX_BUFFER_BYTES));

		const Vulkan_CommandBuffer indexBufferTransferCommandBuffer(VK_COMMAND_BUFFER_LEVEL_PRIMARY, pCommandPools[RE_VK_COMMAND_POOL_TRANSFER_INDEX]);
		CATCH_SIGNAL(indexBufferTransferCommandBuffer.begin_recording_command_buffer(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT));
		CATCH_SIGNAL(indexBufferTransferCommandBuffer.cmd_copy_buffer(&indexStagingBuffer, &rectangleIndexBuffer, RE_VK_INDEX_BUFFER_BYTES));
		CATCH_SIGNAL(indexBufferTransferCommandBuffer.end_recording_command_buffer());
		Vulkan_Fence indexBufferTransferFence;
		VkPipelineStageFlags vk_ePipelineStagesToWaitFor[] = {VK_PIPELINE_STAGE_NONE};
		CATCH_SIGNAL(pDeviceQueues[RE_VK_QUEUE_TRANSFER_INDEX]->submit_to_queue(0U, nullptr, vk_ePipelineStagesToWaitFor, 1U, &indexBufferTransferCommandBuffer, 0U, nullptr, &indexBufferTransferFence));

		vk_semaphoresToWaitForBeforeRendering[0] = semaphoreAcquireSwapchainImage;
		vk_semaphoresToWaitForBeforeRendering[1] = gameObjectRenderer.semaphoreWaitForVertexBufferTransfer;
		CATCH_SIGNAL(create_framebuffers());
		CATCH_SIGNAL(create_command_buffers());
		calculate_render_area();
		vk_maxViewportArea.maxDepth = 1.0f;

		CATCH_SIGNAL(indexBufferTransferFence.wait_for_fence());
		bValid = true;
	}

	Renderer::~Renderer() {
		if (pInstance != this)
			return;
		pInstance = nullptr;
		CATCH_SIGNAL(destroy_command_buffers());
		CATCH_SIGNAL(destroy_framebuffers());
	}

	void Renderer::create_framebuffers() {
		ppFramebuffers = new const Vulkan_Framebuffer*[u32SwapchainImageCount];
		for (uint32_t u32FramebufferCreateIndex = 0U; u32FramebufferCreateIndex < u32SwapchainImageCount; u32FramebufferCreateIndex++)
			CATCH_SIGNAL_DETAILED(ppFramebuffers[u32FramebufferCreateIndex] = new Vulkan_Framebuffer(&renderPass, 1U, &vk_phSwapchainImageViews[u32FramebufferCreateIndex], vk_swapchainResolution.width, vk_swapchainResolution.height), append_to_string("Index: ", u32FramebufferCreateIndex).c_str());
	}

	void Renderer::destroy_framebuffers() {
		if (!ppFramebuffers)
			return;
		CATCH_SIGNAL(renderFence.wait_for_fence());
		for (uint32_t u32FramebufferDestroyIndex = 0U; u32FramebufferDestroyIndex < u32SwapchainImageCount; u32FramebufferDestroyIndex++)
			CATCH_SIGNAL_DETAILED(delete ppFramebuffers[u32FramebufferDestroyIndex], append_to_string("Framebuffer Index: ", u32FramebufferDestroyIndex).c_str());
		DELETE_ARRAY_SAFELY(ppFramebuffers);
	}

	void Renderer::create_command_buffers() {
		ppPrimaryCommandBuffer = new Vulkan_CommandBuffer*[u32SwapchainImageCount];
		if (!CATCH_SIGNAL_AND_RETURN(alloc_vk_command_buffers(VK_COMMAND_BUFFER_LEVEL_PRIMARY, pCommandPools[RE_VK_COMMAND_POOL_GRAPHICS_INDEX], u32SwapchainImageCount, ppPrimaryCommandBuffer), bool)) {
			DELETE_ARRAY_SAFELY(ppPrimaryCommandBuffer);
			RE_FATAL_ERROR("Failed to allocate primary command buffers");
			return;
		}
	}
	
	void Renderer::destroy_command_buffers() {
		if (ppPrimaryCommandBuffer) {
			CATCH_SIGNAL(free_vk_command_buffers(pCommandPools[RE_VK_COMMAND_POOL_GRAPHICS_INDEX], u32SwapchainImageCount, ppPrimaryCommandBuffer));
			DELETE_ARRAY_SAFELY(ppPrimaryCommandBuffer);
		}
	}

	void Renderer::calculate_render_area() {
		vk_maxViewportArea.width = static_cast<float>(vk_swapchainResolution.width);
		vk_maxViewportArea.height = static_cast<float>(vk_swapchainResolution.height);
		vk_maxScissorArea.extent = vk_swapchainResolution;
	}

	void Renderer::record_command_buffer(const uint32_t u32CommandBufferRecordIndex) {
		CATCH_SIGNAL(ppPrimaryCommandBuffer[u32CommandBufferRecordIndex]->reset_command_buffer(0));
		CATCH_SIGNAL(ppPrimaryCommandBuffer[u32CommandBufferRecordIndex]->begin_recording_command_buffer(0));
		const float fClearColor[4] = {0.0f, 0.0f, 0.0f, 1.0f};
		const int32_t i32ClearColor[4] = {0, 0, 0, 255};
		const uint32_t u32ClearColor[4] = {0U, 0U, 0U, 255U};
		CATCH_SIGNAL(ppPrimaryCommandBuffer[u32CommandBufferRecordIndex]->cmd_begin_renderpass(fClearColor, i32ClearColor, u32ClearColor, 0.0f, 0U, &renderPass, ppFramebuffers[u32CommandBufferRecordIndex], {{0, 0}, {vk_swapchainResolution.width, vk_swapchainResolution.height}}, VK_SUBPASS_CONTENTS_SECONDARY_COMMAND_BUFFERS));
		CATCH_SIGNAL(gameObjectRenderer.add_secondary_command_buffer(*ppPrimaryCommandBuffer[u32CommandBufferRecordIndex], u32CommandBufferRecordIndex));
		CATCH_SIGNAL(ppPrimaryCommandBuffer[u32CommandBufferRecordIndex]->cmd_end_renderpass());
		CATCH_SIGNAL(ppPrimaryCommandBuffer[u32CommandBufferRecordIndex]->end_recording_command_buffer());
	}

	void Renderer::render() {
		if (pActiveCamera) {
			CATCH_SIGNAL(pActiveCamera->update());
		}
		CATCH_SIGNAL(renderFence.wait_for_and_reset_fence());
		uint32_t u32NextSwapchainImageIndex;
		CATCH_SIGNAL(RenderSystem::pInstance->get_next_swapchain_image(&semaphoreAcquireSwapchainImage, &u32NextSwapchainImageIndex));
		CATCH_SIGNAL(gameObjectRenderer.render(u32NextSwapchainImageIndex));
		CATCH_SIGNAL_DETAILED(record_command_buffer(u32NextSwapchainImageIndex), append_to_string("Primary Command Buffer Index: ", u32NextSwapchainImageIndex).c_str());
		VkPipelineStageFlags vk_ePipelinesStagesToWaitFor[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT};
		VkCommandBuffer commandBuffersForRendering[] = {ppPrimaryCommandBuffer[u32NextSwapchainImageIndex]->get_command_buffer()};
		VkSemaphore vk_semaphoresToSignalAfterRendering[] = {semaphoreRenderFinished.get_semaphore()};
		VkFence vk_fenceToSignal = renderFence.get_fence();
		CATCH_SIGNAL(pDeviceQueues[RE_VK_QUEUE_GRAPHICS_INDEX]->submit_to_queue(2U, vk_semaphoresToWaitForBeforeRendering, vk_ePipelinesStagesToWaitFor, 1U, commandBuffersForRendering, 1U, vk_semaphoresToSignalAfterRendering, vk_fenceToSignal));
		CATCH_SIGNAL(pDeviceQueues[RE_VK_QUEUE_PRESENT_INDEX]->submit_to_present_queue(1U, &semaphoreRenderFinished, 1U, &vk_hSwapchain, &u32NextSwapchainImageIndex));
	}

	void Renderer::window_resize_event() {
		CATCH_SIGNAL(renderFence.wait_for_fence());
		CATCH_SIGNAL(destroy_command_buffers());
		CATCH_SIGNAL(destroy_framebuffers());
		CATCH_SIGNAL(create_framebuffers());
		CATCH_SIGNAL(create_command_buffers());
		calculate_render_area();
	}

	bool Renderer::is_valid() const {
		return bValid;
	}

}
