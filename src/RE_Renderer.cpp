#include "RE_Renderer_Internal.hpp"
#include "RE_Window.hpp"
#include "RE_Main.hpp"

namespace RE {

	Color backgroundClearColor(0.2f, 0.2f, 0.2f, 1.0f);

	float fSampleShadingRate = 0.2f;

	uint8_t u8CurrentFrameInFlightIndex = 0;

	bool init_renderer() {
		if (create_render_tasks()) {
			if (create_renderer_buffers()) {
				if (create_renderer_images()) {
					if (create_swapchain()) {
						if (setup_presentation()) {
							if (init_renderer_sprite_layouts()) {
								if (create_descriptor_sets()) {
									if (create_renderer_pipelines()) {
										if (init_renderer_textures()) {
											if (init_renderer_meshes()) {
												if (init_renderer_command_buffers()) {
													for (VulkanTask &rRenderTask : aRenderTasks)
														for (uint8_t u8FunctionIndex = 0; u8FunctionIndex < 3; u8FunctionIndex++)
															rRenderTask.record(u8FunctionIndex, 0, nullptr);
												#ifdef RE_OS_LINUX
													pIndirectDrawWindowTitle->vertexCount = 4;
													pIndirectDrawWindowTitle->firstVertex = 0;
													pIndirectDrawWindowTitle->firstInstance = 0;
													for (pIndirectDrawWindowTitle->instanceCount = 0; pIndirectDrawWindowTitle->instanceCount < 256; pIndirectDrawWindowTitle->instanceCount++) {
														const uint32_t u32CharCode = static_cast<uint32_t>(pacWindowTitle[pIndirectDrawWindowTitle->instanceCount]);
														pWindowFrameUniformData->au32TitleChars[pIndirectDrawWindowTitle->instanceCount] = u32CharCode;
														if (u32CharCode == 0)
															break;
													}
												#endif
													return true;
												}
												destroy_renderer_meshes();
											}
											destroy_renderer_textures();
										}
										destroy_renderer_pipelines();
									}
									destroy_descriptor_sets();
								}
								destroy_renderer_sprite_layout();
							}
							destroy_presentation();
						}
						destroy_swapchain();
					}
					destroy_renderer_images();
				}
				destroy_renderer_buffers();
			}
			destroy_render_tasks();
		}
		return false;
	}

	void destroy_renderer() {
		destroy_renderer_command_buffers();
		destroy_renderer_meshes();
		destroy_renderer_sprite_layout();
		destroy_renderer_textures();
		destroy_renderer_pipelines();
		destroy_descriptor_sets();
		destroy_presentation();
		destroy_swapchain();
		destroy_renderer_images();
		destroy_renderer_buffers();
		destroy_render_tasks();
	}

	void render() {
		if (bSwapchainDirty) {
			if (!recreate_swapchain())
				return;
		}
		constexpr VkPipelineStageFlags2 vk_aeInternalWaitStages[] = {
			VK_PIPELINE_STAGE_2_TRANSFER_BIT,
			VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT,
			VK_PIPELINE_STAGE_2_BOTTOM_OF_PIPE_BIT
		};
		const VkSemaphoreSubmitInfo vk_waitSwapchainSemaphoreInfo = {
			.sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO,
			.pNext = nullptr,
			.semaphore = swapchainSemaphores[u32CurrentSwapchainSemaphoreIndex * RE_VK_SEMAPHORES_PER_SWAPCHAIN_IMAGE],
			.stageMask = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
			.deviceIndex = 1
		},
		vk_signalSwapchainSemaphoreInfo = {
			.sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO,
			.pNext = nullptr,
			.semaphore = swapchainSemaphores[u32CurrentSwapchainSemaphoreIndex * RE_VK_SEMAPHORES_PER_SWAPCHAIN_IMAGE + 1],
			.stageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
			.deviceIndex = 1
		};
		vkWaitForFences(vk_hDevice, 1, &vk_ahRenderFences[u8CurrentFrameInFlightIndex], VK_TRUE, std::numeric_limits<uint64_t>::max());
		if (!should_render()) {
		#ifdef RE_OS_LINUX
			switch (eLinuxWindowType) {
				case LINUX_WINDOW_TYPE_X11:
					break;
				case LINUX_WINDOW_TYPE_WAYLAND:
					if (!acquire_next_swapchain_image())
						break;
					vkResetFences(vk_hDevice, 1, &vk_ahRenderFences[u8CurrentFrameInFlightIndex]);
					aRenderTasks[u8CurrentFrameInFlightIndex].record(
							RENDER_TASK_SUBINDEX_IMAGE_BLIT,
							VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
							[&](VkCommandBuffer vk_hCommandBuffer, uint8_t u8PreviousLogicalQueue, uint8_t u8CurrentLogicalQueue, uint8_t u8NextLogicalQueue) {
								const VkImageMemoryBarrier vk_swapchainImageLayoutTransferBarrier = {
									.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
									.pNext = nullptr,
									.srcAccessMask = VK_ACCESS_NONE,
									.dstAccessMask = VK_ACCESS_NONE,
									.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED,
									.newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
									.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
									.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
									.image = swapchainImages[u32CurrentSwapchainImageIndex],
									.subresourceRange = {
										.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
										.baseMipLevel = 0,
										.levelCount = 1,
										.baseArrayLayer = 0,
										.layerCount = 1
									}
								};
								vkCmdPipelineBarrier(
										vk_hCommandBuffer,
										VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
										VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
										0,
										0,
										nullptr,
										0,
										nullptr,
										1,
										&vk_swapchainImageLayoutTransferBarrier);
							});
					aRenderTasks[u8CurrentFrameInFlightIndex].submit(
							1,
							&vk_waitSwapchainSemaphoreInfo,
							vk_aeInternalWaitStages,
							1,
							&vk_signalSwapchainSemaphoreInfo,
							vk_ahRenderFences[u8CurrentFrameInFlightIndex]);
					present_swapchain_image();
					u8CurrentFrameInFlightIndex = (u8CurrentFrameInFlightIndex + 1) % RE_VK_FRAMES_IN_FLIGHT;
					u32CurrentSwapchainSemaphoreIndex = (u32CurrentSwapchainSemaphoreIndex + 1) % u32SwapchainImageCount;
					goto INCREASE_INDICES_RENDERER;
			}
		#endif
			return;
		}
		if (!acquire_next_swapchain_image())
			return;
		if (!render_procedure()) {
			RE_FATAL_ERROR("Failed to run the rendering procedure");
			return;
		}
		PRINT_DEBUG("Submitting rendering task at frame-in-flight index ", u8CurrentFrameInFlightIndex);
		vkResetFences(vk_hDevice, 1, &vk_ahRenderFences[u8CurrentFrameInFlightIndex]);
		if (!aRenderTasks[u8CurrentFrameInFlightIndex].submit(1,
				&vk_waitSwapchainSemaphoreInfo,
				vk_aeInternalWaitStages,
				1,
				&vk_signalSwapchainSemaphoreInfo,
				vk_ahRenderFences[u8CurrentFrameInFlightIndex])) {
			RE_FATAL_ERROR("Failed submitting a rendering task");
			return;
		}
		present_swapchain_image();
	INCREASE_INDICES_RENDERER:
		u8CurrentFrameInFlightIndex = (u8CurrentFrameInFlightIndex + 1) % RE_VK_FRAMES_IN_FLIGHT;
		u32CurrentSwapchainSemaphoreIndex = (u32CurrentSwapchainSemaphoreIndex + 1) % u32SwapchainImageCount;
	}

	bool swapchain_created_renderer() {
		return create_swapchain_related_images();
	}

	void swapchain_destroyed_renderer() {
		destroy_swapchain_related_images();
	}

	void set_background_color(const Color &rColor) {
		backgroundClearColor = rColor;
	}

	void set_background_color(float fRed, float fGreen, float fBlue) {
		backgroundClearColor.set_red(fRed);
		backgroundClearColor.set_green(fGreen);
		backgroundClearColor.set_blue(fBlue);
	}

	Color get_background_color() {
		return backgroundClearColor;
	}

	bool is_sample_shading_enabled() {
		return fSampleShadingRate != 0.0f;
	}

	void set_sample_shading_rate(const float f32NewSampleShadingRate) {
		if (fSampleShadingRate == f32NewSampleShadingRate)
			return;
		if (f32NewSampleShadingRate < 0.0f || f32NewSampleShadingRate > 1.0f) {
			RE_ERROR("Sample shading rate should be in range between 0.0 and 1.0, but was ", f32NewSampleShadingRate, ". Request to change it has been discarded");
			return;
		} else {
			PRINT_DEBUG("Setting sample shading rate to ", f32NewSampleShadingRate);
			fSampleShadingRate = f32NewSampleShadingRate;
			wait_for_rendering_finished();
			recreate_graphics_pipelines();
		}
	}

	float get_sample_shading_rate() {
		return fSampleShadingRate;
	}

}
