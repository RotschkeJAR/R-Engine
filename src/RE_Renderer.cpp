#include "RE_Renderer_Internal.hpp"
#include "RE_Window.hpp"
#include "RE_Vulkan_Wrapper Classes.hpp"
#include "RE_Vulkan_Wrapper Functions.hpp"
#include "RE_RenderElement.hpp"
#include "RE_Main.hpp"
#include "RE_List_GameObject.hpp"

namespace RE {

	Color backgroundClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	float fSampleShadingRate = 0.2f;

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
								if (create_swapchain()) {
									if (setup_presentation()) {
										if (create_render_pipelines()) {
											PRINT_DEBUG("Waiting for pending transfer tasks to finish");
											wait_for_transfer(std::numeric_limits<uint64_t>::max());
											PRINT_DEBUG("Successfully initialized the renderer");
											return true;
										}
										destroy_presentation();
									}
									destroy_swapchain();
								}
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
		destroy_presentation();
		destroy_swapchain();
		destroy_render_pipelines();
		destroy_renderpass();
		destroy_descriptor_sets();
		destroy_render_buffers();
		destroy_render_elements();
		destroy_general_transfer_task();
		destroy_render_tasks();
	}

	void render() {
		PRINT_DEBUG("Invoking render-procedure");
		bool bSkipRendering;
		if (acquire_next_swapchain_image(bSkipRendering)) {
			if (bSkipRendering)
				return;
			PRINT_DEBUG("Waiting for pending transfers and frame-in-flight at index ", u8CurrentFrameInFlightIndex, " being finished rendering");
			wait_for_transfer(std::numeric_limits<uint64_t>::max());
			vkWaitForFences(vk_hDevice, 1, &renderFences[u8CurrentFrameInFlightIndex], VK_TRUE, std::numeric_limits<uint64_t>::max());
			vkResetFences(vk_hDevice, 1, &renderFences[u8CurrentFrameInFlightIndex]);
			calculate_camera_matrices();
			record_cmd_transfer_buffer();
			if (renderTasks[u8CurrentFrameInFlightIndex].record(RENDER_TASK_SUBINDEX_RENDERING, VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT, [&](const VkCommandBuffer vk_hCommandBuffer, const uint8_t u8PreviousLogicalQueue, const uint8_t u8CurrentLogicalQueue, const uint8_t u8NextLogicalQueue) {
					PRINT_DEBUG("Transferring image layout of renderable image to optimal layout for rendering");
					VkImageMemoryBarrier2 vk_renderImageLayoutTransferRegion = {
						.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2,
						.srcStageMask = VK_PIPELINE_STAGE_2_NONE,
						.srcAccessMask = VK_ACCESS_2_NONE,
						.dstStageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT,
						.dstAccessMask = VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_2_COLOR_ATTACHMENT_READ_BIT,
						.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED,
						.newLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
						.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
						.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
						.subresourceRange = {
							.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
							.baseMipLevel = 0,
							.levelCount = 1,
							.layerCount = 1
						}
					};
					if (vk_eMsaaCount != VK_SAMPLE_COUNT_1_BIT || screenPercentageSettings.eMode != RE_SCREEN_PERCENTAGE_MODE_NORMAL) {
						vk_renderImageLayoutTransferRegion.image = vk_hRenderImages;
						vk_renderImageLayoutTransferRegion.subresourceRange.baseArrayLayer = u8CurrentFrameInFlightIndex;
					} else {
						vk_renderImageLayoutTransferRegion.image = vk_pahSwapchainImages[u32SwapchainImageIndex];
						vk_renderImageLayoutTransferRegion.subresourceRange.baseArrayLayer = 0;
					}
					const VkDependencyInfo vk_renderImageLayoutTransferInfo = {
						.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO,
						.dependencyFlags = 0,
						.imageMemoryBarrierCount = 1,
						.pImageMemoryBarriers = &vk_renderImageLayoutTransferRegion
					};
					vkCmdPipelineBarrier2(vk_hCommandBuffer, &vk_renderImageLayoutTransferInfo);
					record_cmd_begin_renderpass(vk_hCommandBuffer);
					PRINT_DEBUG("Binding graphics pipeline ", vk_hGraphicsPipeline);
					vkCmdBindPipeline(vk_hCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, vk_hGraphicsPipeline);
					PRINT_DEBUG("Binding Vulkan descriptor sets");
					const VkDescriptorSet vk_ahDescSets[] = {cameraDescSets[u8CurrentFrameInFlightIndex], vk_hTextureDescSet};
					vkCmdBindDescriptorSets(vk_hCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, vk_hGraphicsPipelineLayout, 0, sizeof(vk_ahDescSets) / sizeof(vk_ahDescSets[0]), vk_ahDescSets, 0, nullptr);
					PRINT_DEBUG("Setting viewport and scissors");
					const VkViewport vk_viewport = {
						.x = 0.0f,
						.y = 0.0f,
						.width = static_cast<float>(vk_cameraProjectionOnscreen.extent.width),
						.height = static_cast<float>(vk_cameraProjectionOnscreen.extent.height),
						.minDepth = 0.0f,
						.maxDepth = 1.0f
					};
					vkCmdSetViewport(vk_hCommandBuffer, 0, 1, &vk_viewport);
					vkCmdSetScissor(vk_hCommandBuffer, 0, 1, &vk_cameraProjectionOnscreen);
					PRINT_DEBUG("Binding vertex, instance and index buffers");
					const VkBuffer vk_aVertexBuffers[] = {vk_hRectBuffer, vk_hRenderBuffer};
					const VkDeviceSize vk_aBufferOffsets[] = {RE_VK_RECT_BUFFER_VERTICES_OFFSET, sizeof(VkDrawIndexedIndirectCommand)},
						vk_aBufferSizes[] = {RE_VK_RECT_BUFFER_VERTICES_SIZE, gameObjectToRenderCount * sizeof(GameObjectInstanceData)};
					vkCmdBindVertexBuffers2(vk_hCommandBuffer, 0, 2, vk_aVertexBuffers, vk_aBufferOffsets, vk_aBufferSizes, nullptr);
					vkCmdBindIndexBuffer(vk_hCommandBuffer, vk_hRectBuffer, RE_VK_RECT_BUFFER_INDICES_OFFSET, VK_INDEX_TYPE_UINT16);
					PRINT_DEBUG("Drawing indirectly");
					vkCmdDrawIndexedIndirect(vk_hCommandBuffer, vk_hRenderBuffer, 0, 1, sizeof(VkDrawIndexedIndirectCommand));
					record_cmd_end_renderpass(vk_hCommandBuffer);
			})) {
				record_cmd_blitting_render_image();
				PRINT_DEBUG("Submitting render task");
				const VkSemaphoreSubmitInfo vk_waitSwapchainImageAcquireSemaphore = {
					.sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO,
					.semaphore = swapchainSemaphores[u32CurrentSwapchainSemaphoreIndex * RE_VK_SEMAPHORES_PER_SWAPCHAIN_IMAGE],
					.stageMask = VK_PIPELINE_STAGE_2_NONE
				};
				const VkSemaphoreSubmitInfo vk_signalSwapchainImagePresentSemaphore = {
					.sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO,
					.semaphore = swapchainSemaphores[u32CurrentSwapchainSemaphoreIndex * RE_VK_SEMAPHORES_PER_SWAPCHAIN_IMAGE + 1]
				};
				const VkPipelineStageFlags2 vk_aeRenderWaitStages[] = {VK_PIPELINE_STAGE_2_TRANSFER_BIT, VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT};
				if (renderTasks[u8CurrentFrameInFlightIndex].submit(1, &vk_waitSwapchainImageAcquireSemaphore, vk_aeRenderWaitStages, 1, &vk_signalSwapchainImagePresentSemaphore, renderFences[u8CurrentFrameInFlightIndex])) {
					if (present_swapchain_image()) {
						u8CurrentFrameInFlightIndex = (u8CurrentFrameInFlightIndex + 1) % RE_VK_FRAMES_IN_FLIGHT;
						u32CurrentSwapchainSemaphoreIndex = (u32CurrentSwapchainSemaphoreIndex + 1) % u32SwapchainImageCount;
					} else
						RE_FATAL_ERROR("Failed to submit presentation task to the GPU");
				} else
					RE_FATAL_ERROR("Failed to submit render task to the GPU");
			} else
				RE_FATAL_ERROR("Failed to record commands for rendering");
		} else
			RE_FATAL_ERROR("Failed acquiring index of next swapchain image");
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

	void set_background_color(const Color &rColor) {
		backgroundClearColor = rColor;
	}

	void set_background_color(const float fRed, const float fGreen, const float fBlue, const float fAlpha) {
		backgroundClearColor.set_red(fRed);
		backgroundClearColor.set_green(fGreen);
		backgroundClearColor.set_blue(fBlue);
		backgroundClearColor.set_alpha(fAlpha);
	}

	[[nodiscard]]
	Color get_background_color() {
		return backgroundClearColor;
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
			WAIT_FOR_IDLE_VULKAN_DEVICE();
			recreate_render_pipelines();
		}
	}

	[[nodiscard]]
	float get_sample_shading_rate() {
		return fSampleShadingRate;
	}

}
