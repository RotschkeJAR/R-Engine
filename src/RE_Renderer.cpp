#include "RE_Renderer_Internal.hpp"
#include "RE_Window.hpp"
#include "RE_Vulkan_Wrapper Classes.hpp"
#include "RE_Vulkan_Wrapper Functions.hpp"
#include "RE_RenderElement.hpp"
#include "RE_RenderPipelines.hpp"
#include "RE_Main.hpp"
#include "RE_List_GameObject.hpp"

namespace RE {

#define RE_VK_SEMAPHORES_PER_SWAPCHAIN_IMAGE 2
#define RE_VK_SWAPCHAIN_SEMAPHORE_COUNT (u32SwapchainImageCount * RE_VK_SEMAPHORES_PER_SWAPCHAIN_IMAGE)

	const Camera *pActiveCamera = nullptr;
	
	VkPipelineLayout vk_hWorldPipelineLayout;

	VkQueue vk_hPresentQueue;

	std::vector<VkSemaphore> swapchainSemaphores;
	uint32_t u32CurrentSwapchainSemaphoreIndex = 0;

	float fSampleShadingRate = 0.2f;
	bool bRenderPipelinesDirty = false;

	uint8_t u8CurrentFrameInFlightIndex = 0;

	bool init_renderer() {
		PRINT_DEBUG("Initializing renderer");
		if (create_render_tasks()) {
			if (init_general_transfer_task()) {
				PRINT_DEBUG("Creating temporary staging Vulkan buffer for rendering rectangle");
				Vulkan_Buffer stagingRectBuffer;
				if (init_render_elements(stagingRectBuffer)) {
					if (create_render_buffers()) {
						if (create_descriptor_sets()) {
							if (create_renderpass()) {
								PRINT_DEBUG("Creating Vulkan pipeline layout");
								const VkDescriptorSetLayout vk_ahWorldPipelineLayoutDescriptors[] = {
									vk_hCameraDescLayout,
									vk_hTextureDescLayout
								};
								const VkPipelineLayoutCreateInfo vk_worldPipelineLayoutCreateInfo = {
									.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
									.setLayoutCount = sizeof(vk_ahWorldPipelineLayoutDescriptors) / sizeof(vk_ahWorldPipelineLayoutDescriptors[0]),
									.pSetLayouts = vk_ahWorldPipelineLayoutDescriptors
								};
								if (vkCreatePipelineLayout(vk_hDevice, &vk_worldPipelineLayoutCreateInfo, nullptr, &vk_hWorldPipelineLayout) == VK_SUCCESS) {
									if (create_render_pipelines()) {
										if (create_swapchain()) {
											const VkSemaphoreTypeCreateInfo vk_swapchainSemaphoreTypeCreateInfo = {
												.sType = VK_STRUCTURE_TYPE_SEMAPHORE_TYPE_CREATE_INFO,
												.semaphoreType = VK_SEMAPHORE_TYPE_BINARY
											};
											const VkSemaphoreCreateInfo vk_swapchainSemaphoreCreateInfo = {
												.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
												.pNext = &vk_swapchainSemaphoreTypeCreateInfo
											};
											swapchainSemaphores.reserve(RE_VK_SWAPCHAIN_SEMAPHORE_COUNT);
											while (swapchainSemaphores.size() < RE_VK_SWAPCHAIN_SEMAPHORE_COUNT) {
												VkSemaphore vk_hCreatedSemaphore;
												if (vkCreateSemaphore(vk_hDevice, &vk_swapchainSemaphoreCreateInfo, nullptr, &vk_hCreatedSemaphore) != VK_SUCCESS) {
													RE_FATAL_ERROR("Failed creating a Vulkan semaphore for synchronizing presentation operations");
													break;
												}
												swapchainSemaphores.push_back(vk_hCreatedSemaphore);
											}
											if (swapchainSemaphores.size() == RE_VK_SWAPCHAIN_SEMAPHORE_COUNT) {
												vk_hPresentQueue = vk_pahQueues[renderTasks[0].get_logical_queue_index_for_presentation()];
												PRINT_DEBUG("Waiting for pending transfer tasks to finish");
												wait_for_transfer(std::numeric_limits<uint64_t>::max());
												PRINT_DEBUG("Successfully initialized the renderer");
												return true;
											} else {
												for (const VkSemaphore vk_hSwapchainSemaphore : swapchainSemaphores)
													vkDestroySemaphore(vk_hDevice, vk_hSwapchainSemaphore, nullptr);
												swapchainSemaphores.clear();
												swapchainSemaphores.shrink_to_fit();
											}
											destroy_swapchain();
										}
										destroy_render_pipelines();
									}
									PRINT_DEBUG("Destroying Vulkan pipeline layout due to failure initializing the whole renderer");
									vkDestroyPipelineLayout(vk_hDevice, vk_hWorldPipelineLayout, nullptr);
								} else
									RE_FATAL_ERROR("Failed creating a Vulkan graphics pipeline for rendering objects in world");
								destroy_renderpass();
							}
							destroy_descriptor_sets();
						}
						destroy_render_buffers();
					}
					PRINT_DEBUG("Waiting for pending transfer task to finish before destroying further due to failure creating essential resources");
					wait_for_transfer(std::numeric_limits<uint64_t>::max());
					destroy_render_elements();
				}
				destroy_general_transfer_task();
			}
			destroy_render_tasks();
		}
		PRINT_DEBUG("Destroyed renderer after failure initializing it");
		return false;
	}
	
	void destroy_renderer() {
		PRINT_DEBUG("Destroying renderer");
		for (const VkSemaphore vk_hSwapchainSemaphore : swapchainSemaphores)
			vkDestroySemaphore(vk_hDevice, vk_hSwapchainSemaphore, nullptr);
		swapchainSemaphores.clear();
		swapchainSemaphores.shrink_to_fit();
		destroy_swapchain();
		destroy_render_pipelines();
		vkDestroyPipelineLayout(vk_hDevice, vk_hWorldPipelineLayout, nullptr);
		destroy_renderpass();
		destroy_descriptor_sets();
		destroy_render_buffers();
		destroy_render_elements();
		destroy_general_transfer_task();
		destroy_render_tasks();
	}

	void render() {
		PRINT_DEBUG("Invoking render-procedure");
		if (bRenderPipelinesDirty) {
			recreate_render_pipelines();
			bRenderPipelinesDirty = false;
		}
		PRINT_DEBUG("Acquiring next Vulkan swapchain image");
		const VkAcquireNextImageInfoKHR vk_acquireInfo = {
			.sType = VK_STRUCTURE_TYPE_ACQUIRE_NEXT_IMAGE_INFO_KHR,
			.swapchain = vk_hSwapchain,
			.timeout = std::numeric_limits<uint64_t>::max(),
			.semaphore = swapchainSemaphores[u32CurrentSwapchainSemaphoreIndex * RE_VK_SEMAPHORES_PER_SWAPCHAIN_IMAGE],
			.deviceMask = 1
		};
		uint32_t u32SwapchainImageIndex;
		vkAcquireNextImage2KHR(vk_hDevice, &vk_acquireInfo, &u32SwapchainImageIndex);
		PRINT_DEBUG("Waiting for pending transfers and frame-in-flight at index ", u8CurrentFrameInFlightIndex, " being finished rendering");
		wait_for_transfer(std::numeric_limits<uint64_t>::max());
		vkWaitForFences(vk_hDevice, 1, &renderFences[u8CurrentFrameInFlightIndex], VK_TRUE, std::numeric_limits<uint64_t>::max());
		vkResetFences(vk_hDevice, 1, &renderFences[u8CurrentFrameInFlightIndex]);
		record_command_buffer_blitting_render_image(u32SwapchainImageIndex);
		const VkSemaphoreSubmitInfo vk_waitSwapchainImageAcquireSemaphore = {
			.sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO,
			.semaphore = vk_acquireInfo.semaphore,
			.stageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT
		};
		const VkSemaphoreSubmitInfo vk_signalSwapchainImagePresentSemaphore = {
			.sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO,
			.semaphore = swapchainSemaphores[u32CurrentSwapchainSemaphoreIndex * RE_VK_SEMAPHORES_PER_SWAPCHAIN_IMAGE + 1]
		};
		const VkPipelineStageFlags2 vk_aeRenderWaitStages[] = {VK_PIPELINE_STAGE_2_TRANSFER_BIT, VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT};
		renderTasks[u8CurrentFrameInFlightIndex].submit(1, &vk_waitSwapchainImageAcquireSemaphore, vk_aeRenderWaitStages, 1, &vk_signalSwapchainImagePresentSemaphore, renderFences[u8CurrentFrameInFlightIndex]);
		PRINT_DEBUG("Submitting swapchain image at index ", u32SwapchainImageIndex, " to presentation");
		const VkPresentInfoKHR vk_presentInfo = {
			.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
			.waitSemaphoreCount = 1,
			.pWaitSemaphores = &vk_signalSwapchainImagePresentSemaphore.semaphore,
			.swapchainCount = 1,
			.pSwapchains = &vk_hSwapchain,
			.pImageIndices = &u32SwapchainImageIndex
		};
		vkQueuePresentKHR(vk_hPresentQueue, &vk_presentInfo);
		u8CurrentFrameInFlightIndex = (u8CurrentFrameInFlightIndex + 1) % RE_VK_FRAMES_IN_FLIGHT;
		u32CurrentSwapchainSemaphoreIndex = (u32CurrentSwapchainSemaphoreIndex + 1) % u32SwapchainImageCount;
	}

	bool swapchain_created_renderer() {
		PRINT_DEBUG("Creating swapchain-related Vulkan objects in Renderer");
		return create_render_image_resources();
	}

	void swapchain_destroyed_renderer() {
		PRINT_DEBUG("Destroying swapchain-related Vulkan objects in Renderer");
		destroy_render_image_resources();
	}

	void attach_camera(const Camera *const pCamera) {
		pActiveCamera = pCamera;
	}

	[[nodiscard]]
	bool is_sample_shading_enabled() {
		return fSampleShadingRate > 0.0f && fSampleShadingRate <= 1.0f;
	}

	void set_sample_shading_rate(const float fNewSampleShadingRate) {
		if (fSampleShadingRate == fNewSampleShadingRate)
			return;
		if (fNewSampleShadingRate < 0.0f || fNewSampleShadingRate > 1.0f) {
			RE_ERROR("Sample shading rate should be in range between 0 and 1, but was ", fNewSampleShadingRate, ". Request to change it has been discarded");
			return;
		} else {
			PRINT_DEBUG("Setting sample shading rate to ", fNewSampleShadingRate);
			fSampleShadingRate = fNewSampleShadingRate;
			bRenderPipelinesDirty = bRunning;
		}
	}

	[[nodiscard]]
	float get_sample_shading_rate() {
		return fSampleShadingRate;
	}

}
