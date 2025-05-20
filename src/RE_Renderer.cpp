#include "RE_Renderer.hpp"
#include "RE_Internal Header.hpp"
#include "RE_Render System.hpp"
#include "RE_Window.hpp"

namespace RE {

	typedef uint16_t REindex_t;
#define RE_VK_INDEX_BUFFER_SIZE RE_VK_RENDERABLE_RECTANGLES_COUNT * 6U
#define RE_VK_INDEX_BUFFER_BYTES RE_VK_INDEX_BUFFER_SIZE * sizeof(REindex_t)

	SubRenderer::SubRenderer() : ppSecondaryCommandBuffers(nullptr) {
		ppSecondaryCommandBuffers = new Vulkan_CommandBuffer*[u32SwapchainImageCount];
		if (!CATCH_SIGNAL_AND_RETURN(alloc_vk_command_buffers(pCommandPools[RE_VK_COMMAND_POOL_GRAPHICS_INDEX], VK_COMMAND_BUFFER_LEVEL_SECONDARY, u32SwapchainImageCount, ppSecondaryCommandBuffers), bool)) {
			RE_FATAL_ERROR("Failed to allocate secondary command buffers for subrenderer");
			DELETE_ARRAY_SAFELY(ppSecondaryCommandBuffers);
			return;
		}
	}

	SubRenderer::~SubRenderer() {
		if (is_subrenderer_valid()) {
			CATCH_SIGNAL(free_vk_command_buffers(u32SwapchainImageCount, ppSecondaryCommandBuffers));
			delete[] ppSecondaryCommandBuffers;
		}
	}

	bool SubRenderer::is_subrenderer_valid() const {
		return ppSecondaryCommandBuffers != nullptr;
	}



#define RE_VK_RENDER_PASS_ATTACHMENT_DESCRIPTION_COUNT 1U
#define RE_VK_RENDER_PASS_SUBPASS_DESCRIPTION_COUNT 1U
#define RE_VK_RENDER_PASS_SUBPASS_DEPENDENCY_COUNT 1U

	VkAttachmentDescription vk_colorAttachment[RE_VK_RENDER_PASS_ATTACHMENT_DESCRIPTION_COUNT] = {
		{
			.samples = VK_SAMPLE_COUNT_1_BIT,
			.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
			.storeOp = VK_ATTACHMENT_STORE_OP_STORE,
			.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
			.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
			.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
			.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR
		}
	};
	constexpr VkAttachmentReference vk_attachmentReference[] = {
		{
			.attachment = 0U,
			.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
		}
	};
	constexpr VkSubpassDescription vk_subpassDescription[RE_VK_RENDER_PASS_SUBPASS_DESCRIPTION_COUNT] = {
		{
			.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,
			.colorAttachmentCount = 1U,
			.pColorAttachments = &vk_attachmentReference[0]
		}
	};
	constexpr VkSubpassDependency vk_subpassDependency[RE_VK_RENDER_PASS_SUBPASS_DEPENDENCY_COUNT] = {
		{
			.srcSubpass = VK_SUBPASS_EXTERNAL,
			.dstSubpass = 0,
			.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
			.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
			.srcAccessMask = 0U,
			.dstAccessMask = 0U
		}
	};

	constexpr uint32_t u32IndexBufferQueueTypes[] = {RE_VK_QUEUE_GRAPHICS_INDEX, RE_VK_QUEUE_TRANSFER_INDEX};
	Camera *pActiveCamera = nullptr;
	Renderer *Renderer::pInstance = nullptr;

	static uint32_t setup_renderpass_objects() {
		vk_colorAttachment[0].format = vk_eSwapchainImageFormat;
		return RE_VK_RENDER_PASS_ATTACHMENT_DESCRIPTION_COUNT;
	}

	Renderer::Renderer() 
#if (RE_VK_FRAMES_IN_FLIGHT == 2)
	: renderPass(setup_renderpass_objects(), vk_colorAttachment, RE_VK_RENDER_PASS_SUBPASS_DESCRIPTION_COUNT, vk_subpassDescription, RE_VK_RENDER_PASS_SUBPASS_DEPENDENCY_COUNT, vk_subpassDependency), 
	ppPrimaryCommandBuffer(nullptr), 
	renderFence{Vulkan_Fence(VK_FENCE_CREATE_SIGNALED_BIT), Vulkan_Fence(VK_FENCE_CREATE_SIGNALED_BIT)}, 
	ppSwapchainImageFences(new const Vulkan_Fence*[u32SwapchainImageCount] {}), 
	u8CurrentFrameInFlight(0U), 
	gameObjectRenderer(&renderPass), 
	bValid(false), 
	rectangleIndexBuffer(RE_VK_INDEX_BUFFER_BYTES, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, u32IndexBufferQueueTypes, sizeof(u32IndexBufferQueueTypes) / sizeof(u32IndexBufferQueueTypes[0]), VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT), 
	ppFramebuffers(nullptr) 
#else
# error Update the array initializations above!
#endif
	{
		if (pInstance) {
			RE_FATAL_ERROR("Another instance of Renderer has been constructed.");
			return;
		}
		pInstance = this;
		if (!gameObjectRenderer.is_valid()) {
			return;
		}
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

		CATCH_SIGNAL(create_framebuffers());
		CATCH_SIGNAL(create_command_buffers());
		calculate_render_area();
		vk_cameraViewportArea.minDepth = 0.0f;
		vk_cameraViewportArea.maxDepth = 1.0f;
		vk_cameraScissorArea.offset.x = 0;
		vk_cameraScissorArea.offset.y = 0;

		CATCH_SIGNAL(indexBufferTransferFence.wait_for_fence());
		bValid = true;
	}

	Renderer::~Renderer() {
		if (pInstance != this)
			return;
		pInstance = nullptr;
		DELETE_ARRAY_SAFELY(ppSwapchainImageFences);
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
		CATCH_SIGNAL(wait_for_all_fences());
		for (uint32_t u32FramebufferDestroyIndex = 0U; u32FramebufferDestroyIndex < u32SwapchainImageCount; u32FramebufferDestroyIndex++)
			CATCH_SIGNAL_DETAILED(delete ppFramebuffers[u32FramebufferDestroyIndex], append_to_string("Framebuffer Index: ", u32FramebufferDestroyIndex).c_str());
		DELETE_ARRAY_SAFELY(ppFramebuffers);
	}

	void Renderer::create_command_buffers() {
		ppPrimaryCommandBuffer = new Vulkan_CommandBuffer*[u32SwapchainImageCount];
		if (!CATCH_SIGNAL_AND_RETURN(alloc_vk_command_buffers(pCommandPools[RE_VK_COMMAND_POOL_GRAPHICS_INDEX], VK_COMMAND_BUFFER_LEVEL_PRIMARY, u32SwapchainImageCount, ppPrimaryCommandBuffer), bool)) {
			DELETE_ARRAY_SAFELY(ppPrimaryCommandBuffer);
			RE_FATAL_ERROR("Failed to allocate primary command buffers");
			return;
		}
	}
	
	void Renderer::destroy_command_buffers() {
		if (ppPrimaryCommandBuffer) {
			CATCH_SIGNAL(free_vk_command_buffers(u32SwapchainImageCount, ppPrimaryCommandBuffer));
			DELETE_ARRAY_SAFELY(ppPrimaryCommandBuffer);
		}
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
		if (pActiveCamera)
			CATCH_SIGNAL(pActiveCamera->update());
		CATCH_SIGNAL(renderFence[u8CurrentFrameInFlight].wait_for_fence());
		uint32_t u32NextSwapchainImageIndex;
		if (!CATCH_SIGNAL_AND_RETURN(RenderSystem::pInstance->get_next_swapchain_image(&semaphoreAcquireSwapchainImage[u8CurrentFrameInFlight], &u32NextSwapchainImageIndex), bool))
			return;
		if (ppSwapchainImageFences[u32NextSwapchainImageIndex])
			ppSwapchainImageFences[u32NextSwapchainImageIndex]->wait_for_fence();
		ppSwapchainImageFences[u32NextSwapchainImageIndex] = &renderFence[u8CurrentFrameInFlight];
		CATCH_SIGNAL(gameObjectRenderer.render(u32NextSwapchainImageIndex, u8CurrentFrameInFlight));
		CATCH_SIGNAL_DETAILED(record_command_buffer(u32NextSwapchainImageIndex), append_to_string("Primary Command Buffer Index: ", u32NextSwapchainImageIndex).c_str());

		// Submission to queue
		constexpr uint32_t u32SemaphoresToWaitForBeforeRenderingCount = 2U;
		VkSemaphore vk_semaphoresToWaitForBeforeRendering[u32SemaphoresToWaitForBeforeRenderingCount] = {semaphoreAcquireSwapchainImage[u8CurrentFrameInFlight].get_semaphore(), gameObjectRenderer.semaphoreWaitForVertexBufferTransfer[u8CurrentFrameInFlight].get_semaphore()};
		VkPipelineStageFlags vk_ePipelinesStagesToWaitFor[u32SemaphoresToWaitForBeforeRenderingCount] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT};
		constexpr uint32_t u32CommandBufferForRenderingCount = 1U;
		VkCommandBuffer commandBuffersForRendering[u32CommandBufferForRenderingCount] = {ppPrimaryCommandBuffer[u32NextSwapchainImageIndex]->get_command_buffer()};
		constexpr uint32_t u32SemaphoresToSignalAfterRenderingCount = 1U;
		VkSemaphore vk_semaphoresToSignalAfterRendering[u32SemaphoresToSignalAfterRenderingCount] = {semaphoreRenderFinished[u8CurrentFrameInFlight].get_semaphore()};
		VkFence vk_fenceToSignal = renderFence[u8CurrentFrameInFlight].get_fence();
		CATCH_SIGNAL(renderFence[u8CurrentFrameInFlight].reset_fence());
		CATCH_SIGNAL(pDeviceQueues[RE_VK_QUEUE_GRAPHICS_INDEX]->submit_to_queue(u32SemaphoresToWaitForBeforeRenderingCount, (VkSemaphore*) &vk_semaphoresToWaitForBeforeRendering, (VkPipelineStageFlags*) &vk_ePipelinesStagesToWaitFor, u32CommandBufferForRenderingCount, (VkCommandBuffer*) commandBuffersForRendering, u32SemaphoresToSignalAfterRenderingCount, (VkSemaphore*) &vk_semaphoresToSignalAfterRendering, vk_fenceToSignal));
		CATCH_SIGNAL(pDeviceQueues[RE_VK_QUEUE_PRESENT_INDEX]->submit_to_present_queue(u32SemaphoresToSignalAfterRenderingCount, (VkSemaphore*) &vk_semaphoresToSignalAfterRendering, 1U, &vk_hSwapchain, &u32NextSwapchainImageIndex));
		u8CurrentFrameInFlight = (u8CurrentFrameInFlight + 1) % RE_VK_FRAMES_IN_FLIGHT;
	}

	void Renderer::swapchain_recreated() {
		delete[] ppSwapchainImageFences;
		ppSwapchainImageFences = new const Vulkan_Fence*[u32SwapchainImageCount] {};
		CATCH_SIGNAL(destroy_command_buffers());
		CATCH_SIGNAL(destroy_framebuffers());
		CATCH_SIGNAL(create_framebuffers());
		CATCH_SIGNAL(create_command_buffers());
		calculate_render_area();
	}

	void Renderer::calculate_render_area() {
		if (pActiveCamera) {
			const float fCameraScale = std::min(vk_swapchainResolution.width / pActiveCamera->scale[0], vk_swapchainResolution.height / pActiveCamera->scale[1]);
			vk_cameraViewportArea.width = std::round(pActiveCamera->scale[0] * fCameraScale);
			vk_cameraViewportArea.height = std::round(pActiveCamera->scale[1] * fCameraScale);
			vk_cameraViewportArea.x = std::round((vk_swapchainResolution.width - vk_cameraViewportArea.width) / 2.0f);
			vk_cameraViewportArea.y = std::round((vk_swapchainResolution.height - vk_cameraViewportArea.height) / 2.0f);
		} else {
			vk_cameraViewportArea.width = vk_swapchainResolution.width;
			vk_cameraViewportArea.height = vk_swapchainResolution.height;
			vk_cameraViewportArea.x = 0.0f;
			vk_cameraViewportArea.y = 0.0f;
		}
		vk_cameraScissorArea.extent.width = vk_cameraViewportArea.width;
		vk_cameraScissorArea.extent.height = vk_cameraViewportArea.height;
		vk_cameraScissorArea.offset.x = vk_cameraViewportArea.x;
		vk_cameraScissorArea.offset.y = vk_cameraViewportArea.y;
	}

	void Renderer::wait_for_all_fences() const {
		for (uint8_t u8FenceWaitForIndex = 0U; u8FenceWaitForIndex < RE_VK_FRAMES_IN_FLIGHT; u8FenceWaitForIndex++)
			CATCH_SIGNAL_DETAILED(renderFence[u8FenceWaitForIndex].wait_for_fence(), append_to_string("Fence to wait for-index: ", u8FenceWaitForIndex).c_str());
	}

	bool Renderer::is_valid() const {
		return bValid;
	}

}
