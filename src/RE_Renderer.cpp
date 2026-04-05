#include "RE_Renderer_Internal.hpp"
#include "RE_Window.hpp"
#include "RE_RenderElement.hpp"
#include "RE_Main.hpp"

#include "RE_Renderer_Buffers.hpp"

namespace RE {

	Color backgroundClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	float f32SampleShadingRate = 0.2f;

	uint8_t u8CurrentFrameInFlightIndex = 0;

	bool init_renderer() {
		PRINT_DEBUG("Initializing renderer");
		if (create_render_tasks()) {
			PRINT_DEBUG("Creating temporary staging Vulkan buffer for rendering rectangle");
			const uint8_t a2u8LogicalQueueIndices[2] = {RE_VK_LOGICAL_QUEUE_IGNORED, renderTasks[0].logical_queue_index_for_function(RENDER_TASK_SUBINDEX_RENDERING)};
			constexpr VkQueueFlagBits vk_a2eQueueTypes[2] = {VK_QUEUE_TRANSFER_BIT, VK_QUEUE_GRAPHICS_BIT};
			constexpr uint32_t a2u32SeparationIds[2] = {0, 1};
			const VulkanTask_Queues occupiedTransferQueues = {
				.pau8LogicalQueueIndices = a2u8LogicalQueueIndices,
				.vk_paeQueueTypes = vk_a2eQueueTypes,
				.pau32StrictSeparationIds = a2u32SeparationIds,
				.u32FunctionsCount = 2
			};
			VulkanTask rectBufferTransferTask(occupiedTransferQueues, false, false, true);
			Vulkan_Fence rectBufferTransferFence(0);
			Vulkan_Buffer stagingRectBuffer;
			if (init_render_elements(stagingRectBuffer, &rectBufferTransferTask, rectBufferTransferFence)) {
				if (create_renderer_buffers()) {
					if (create_descriptor_sets()) {
						if (create_swapchain()) {
							if (setup_presentation()) {
								if (create_graphics_pipelines()) {
									if (create_compute_pipelines()) {
										rectBufferTransferFence.wait_for();
										PRINT_DEBUG("Successfully initialized the renderer");
										return true;
									}
									destroy_graphics_pipelines();
								}
								destroy_presentation();
							}
							destroy_swapchain();
						}
						destroy_descriptor_sets();
					}
					destroy_renderer_buffers();
				}
				PRINT_DEBUG("Waiting for pending transfer task to finish before destroying further due to failure creating essential resources");
				rectBufferTransferFence.wait_for();
				destroy_render_elements();
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
		destroy_compute_pipelines();
		destroy_graphics_pipelines();
		destroy_descriptor_sets();
		destroy_renderer_buffers();
		destroy_render_elements();
		destroy_render_tasks();
	}

	void render() {
		PRINT_DEBUG("Invoking render-procedure");
		if (bSwapchainDirty)
			if (!recreate_swapchain())
				return;
		if (acquire_next_swapchain_image()) {
			PRINT_DEBUG("Waiting for pending frame-in-flight at index ", u8CurrentFrameInFlightIndex, " being finished rendering");
			vkWaitForFences(vk_hDevice, 1, &renderFences[u8CurrentFrameInFlightIndex], VK_TRUE, std::numeric_limits<uint64_t>::max());
			vkResetFences(vk_hDevice, 1, &renderFences[u8CurrentFrameInFlightIndex]);
			calculate_camera_matrices();
			record_cmd_transfer_buffer();
			const size_t gameObjectCounter = amount_of_game_objects();
			if (renderTasks[u8CurrentFrameInFlightIndex].record(RENDER_TASK_SUBINDEX_PROCESSING, VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT, [&](const VkCommandBuffer vk_hCommandBuffer, const uint8_t u8PreviousLogicalQueue, const uint8_t u8CurrentLogicalQueue, const uint8_t u8NextLogicalQueue) {
					vkCmdFillBuffer(vk_hCommandBuffer, aSortableDepthBuffers[u8CurrentFrameInFlightIndex].get(), 0, VK_WHOLE_SIZE, 0);
					PRINT_DEBUG("Binding compute pipeline ", vk_hComputePipelinePreprocessing);
					vkCmdBindPipeline(vk_hCommandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, vk_hComputePipelinePreprocessing);
					PRINT_DEBUG("Binding Vulkan descriptor sets for preprocessing");
					const VkDescriptorSet vk_ahComputeDescSets[] = {
						vk_ahRawGameObjectBufferDescSets[u8CurrentFrameInFlightIndex],
						vk_ahRenderContentDescSets[u8CurrentFrameInFlightIndex]
					};
					vkCmdBindDescriptorSets(vk_hCommandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, vk_hComputePipelineLayoutProcessing, 0, sizeof(vk_ahComputeDescSets) / sizeof(vk_ahComputeDescSets[0]), vk_ahComputeDescSets, 0, nullptr);
					PRINT_DEBUG("Dispatching compute shaders");
					vkCmdDispatch(vk_hCommandBuffer, gameObjectCounter, 1, 1);
					const VkBufferMemoryBarrier2 vk_sortableDepthBufferInfo = {
						.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER_2,
						.srcStageMask = VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT,
						.srcAccessMask = VK_ACCESS_2_SHADER_STORAGE_READ_BIT | VK_ACCESS_2_SHADER_STORAGE_WRITE_BIT,
						.dstStageMask = VK_PIPELINE_STAGE_2_VERTEX_SHADER_BIT,
						.dstAccessMask = VK_ACCESS_2_SHADER_READ_BIT,
						.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
						.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
						.buffer = aSortableDepthBuffers[u8CurrentFrameInFlightIndex].get(),
						.offset = 0,
						.size = VK_WHOLE_SIZE
					};
					const VkDependencyInfo vk_sortableDepthBufferDependency = {
						.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO,
						.pNext = nullptr,
						.dependencyFlags = 0,
						.memoryBarrierCount = 0,
						.bufferMemoryBarrierCount = 1,
						.pBufferMemoryBarriers = &vk_sortableDepthBufferInfo,
						.imageMemoryBarrierCount = 0
					};
					vkCmdPipelineBarrier2(vk_hCommandBuffer, &vk_sortableDepthBufferDependency);
			})) {
				if (renderTasks[u8CurrentFrameInFlightIndex].record(RENDER_TASK_SUBINDEX_RENDERING, VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT, [&](const VkCommandBuffer vk_hCommandBuffer, const uint8_t u8PreviousLogicalQueue, const uint8_t u8CurrentLogicalQueue, const uint8_t u8NextLogicalQueue) {
						PRINT_DEBUG("Transferring image layout of renderable image to optimal layout for rendering");
						VkImageMemoryBarrier2 vk_a2RenderImageLayoutTransferRegions[2];
						vk_a2RenderImageLayoutTransferRegions[0].sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2;
						vk_a2RenderImageLayoutTransferRegions[0].pNext = nullptr;
						vk_a2RenderImageLayoutTransferRegions[0].srcStageMask = VK_PIPELINE_STAGE_2_NONE;
						vk_a2RenderImageLayoutTransferRegions[0].srcAccessMask = VK_ACCESS_2_NONE;
						vk_a2RenderImageLayoutTransferRegions[0].dstStageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT;
						vk_a2RenderImageLayoutTransferRegions[0].dstAccessMask = VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_2_COLOR_ATTACHMENT_READ_BIT;
						vk_a2RenderImageLayoutTransferRegions[0].oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
						vk_a2RenderImageLayoutTransferRegions[0].newLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
						vk_a2RenderImageLayoutTransferRegions[0].srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
						vk_a2RenderImageLayoutTransferRegions[0].dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
						vk_a2RenderImageLayoutTransferRegions[0].subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
						vk_a2RenderImageLayoutTransferRegions[0].subresourceRange.baseMipLevel = 0;
						vk_a2RenderImageLayoutTransferRegions[0].subresourceRange.levelCount = 1;
						vk_a2RenderImageLayoutTransferRegions[0].subresourceRange.layerCount = 1;
						VkDependencyInfo vk_renderImageLayoutTransferInfo = {
							.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO,
							.dependencyFlags = 0,
							.imageMemoryBarrierCount = 1,
							.pImageMemoryBarriers = vk_a2RenderImageLayoutTransferRegions
						};
						const bool bBlittingRequired = screenPercentageSettings.eMode != RE_SCREEN_PERCENTAGE_MODE_NORMAL,
							bResolvingRequired = vk_eMsaaCount != VK_SAMPLE_COUNT_1_BIT;
						if (bResolvingRequired || bBlittingRequired) {
							vk_a2RenderImageLayoutTransferRegions[0].image = vk_hRenderTargetImage;
							vk_a2RenderImageLayoutTransferRegions[0].subresourceRange.baseArrayLayer = u8CurrentFrameInFlightIndex;
							if (bResolvingRequired) {
								vk_a2RenderImageLayoutTransferRegions[1].sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2;
								vk_a2RenderImageLayoutTransferRegions[1].pNext = nullptr;
								vk_a2RenderImageLayoutTransferRegions[1].srcStageMask = VK_PIPELINE_STAGE_2_NONE;
								vk_a2RenderImageLayoutTransferRegions[1].srcAccessMask = VK_ACCESS_2_NONE;
								vk_a2RenderImageLayoutTransferRegions[1].dstStageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT;
								vk_a2RenderImageLayoutTransferRegions[1].dstAccessMask = VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT;
								vk_a2RenderImageLayoutTransferRegions[1].oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
								vk_a2RenderImageLayoutTransferRegions[1].newLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
								vk_a2RenderImageLayoutTransferRegions[1].srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
								vk_a2RenderImageLayoutTransferRegions[1].dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
								vk_a2RenderImageLayoutTransferRegions[1].image = bBlittingRequired ? vk_hSinglesampledImage : swapchainImages[u32SwapchainImageIndex];
								vk_a2RenderImageLayoutTransferRegions[1].subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
								vk_a2RenderImageLayoutTransferRegions[1].subresourceRange.baseMipLevel = 0;
								vk_a2RenderImageLayoutTransferRegions[1].subresourceRange.levelCount = 1;
								vk_a2RenderImageLayoutTransferRegions[1].subresourceRange.baseArrayLayer = u8CurrentFrameInFlightIndex;
								vk_a2RenderImageLayoutTransferRegions[1].subresourceRange.layerCount = 1;
								vk_renderImageLayoutTransferInfo.imageMemoryBarrierCount = 2;
							}
						} else {
							vk_a2RenderImageLayoutTransferRegions[0].image = swapchainImages[u32SwapchainImageIndex];
							vk_a2RenderImageLayoutTransferRegions[0].subresourceRange.baseArrayLayer = 0;
						}
						vkCmdPipelineBarrier2(vk_hCommandBuffer, &vk_renderImageLayoutTransferInfo);
						record_cmd_begin_dynamic_rendering(vk_hCommandBuffer);
						PRINT_DEBUG("Binding graphics pipeline ", vk_hGraphicsPipeline3D);
						vkCmdBindPipeline(vk_hCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, vk_hGraphicsPipeline3D);
						const VkViewport vk_viewport = {
							.x = static_cast<float>(vk_cameraProjectionOnscreen.offset.x),
							.y = static_cast<float>(vk_cameraProjectionOnscreen.offset.y),
							.width = static_cast<float>(vk_cameraProjectionOnscreen.extent.width),
							.height = static_cast<float>(vk_cameraProjectionOnscreen.extent.height),
							.minDepth = 0.0f,
							.maxDepth = 1.0f
						};
						vkCmdSetViewport(vk_hCommandBuffer, 0, 1, &vk_viewport);
						vkCmdSetScissor(vk_hCommandBuffer, 0, 1, &vk_cameraProjectionOnscreen);
						PRINT_DEBUG("Binding Vulkan descriptor sets for rendering");
						const VkDescriptorSet vk_ahGraphicsDescSets[] = {
							vk_ahRenderContentDescSets[u8CurrentFrameInFlightIndex],
							vk_hTextureDescSet
						};
						vkCmdBindDescriptorSets(vk_hCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, vk_hGraphicsPipelineLayout, 0, sizeof(vk_ahGraphicsDescSets) / sizeof(vk_ahGraphicsDescSets[0]), vk_ahGraphicsDescSets, 0, nullptr);
						PRINT_DEBUG("Binding vertex, instance and index buffers");
						constexpr uint32_t u32VertexBufferBindingCount = 1;
						const VkBuffer vk_aVertexBuffers[u32VertexBufferBindingCount] = {
							vk_hRectBuffer
						};
						const VkDeviceSize vk_aBufferOffsets[u32VertexBufferBindingCount] = {
							RE_VK_RECT_BUFFER_VERTICES_OFFSET
						},
						vk_aBufferSizes[u32VertexBufferBindingCount] = {
							RE_VK_RECT_BUFFER_VERTICES_SIZE
						};
						vkCmdBindVertexBuffers2(vk_hCommandBuffer, 0, u32VertexBufferBindingCount, vk_aVertexBuffers, vk_aBufferOffsets, vk_aBufferSizes, nullptr);
						vkCmdBindIndexBuffer(vk_hCommandBuffer, vk_hRectBuffer, RE_VK_RECT_BUFFER_INDICES_OFFSET, VK_INDEX_TYPE_UINT16);
						PRINT_DEBUG("Drawing indirectly");
						vkCmdDrawIndexed(vk_hCommandBuffer, 6, 1000, 0, 0, 0);
						record_cmd_end_dynamic_rendering(vk_hCommandBuffer);
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
				RE_FATAL_ERROR("Failed to record commands for computing");
		} else
			RE_FATAL_ERROR("Failed acquiring index of next swapchain image");
	}

	bool swapchain_created_renderer() {
		PRINT_DEBUG("Creating swapchain-related Vulkan objects in Renderer");
		return create_images_renderer();
	}

	void swapchain_destroyed_renderer() {
		PRINT_DEBUG("Destroying swapchain-related Vulkan objects in Renderer");
		destroy_images_renderer();
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
		return f32SampleShadingRate > 0.0f && f32SampleShadingRate <= 1.0f;
	}

	void set_sample_shading_rate(const float f32NewSampleShadingRate) {
		if (f32SampleShadingRate == f32NewSampleShadingRate)
			return;
		if (f32NewSampleShadingRate < 0.0f || f32NewSampleShadingRate > 1.0f) {
			RE_ERROR("Sample shading rate should be in range between 0.0 and 1.0, but was ", f32NewSampleShadingRate, ". Request to change it has been discarded");
			return;
		} else {
			PRINT_DEBUG("Setting sample shading rate to ", f32NewSampleShadingRate);
			f32SampleShadingRate = f32NewSampleShadingRate;
			vkDeviceWaitIdle(vk_hDevice);
			recreate_graphics_pipelines();
		}
	}

	[[nodiscard]]
	float get_sample_shading_rate() {
		return f32SampleShadingRate;
	}

}
