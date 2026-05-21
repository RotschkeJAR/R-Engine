#include "RE_Renderer_Internal.hpp"
#include "RE_Window.hpp"
#include "RE_Main.hpp"

#include "RE_Renderer_Buffers.hpp"

namespace RE {

	Color backgroundClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	float f32SampleShadingRate = 0.2f;

	uint8_t u8CurrentFrameInFlightIndex = 0;

	bool init_renderer() {
		PRINT_DEBUG("Initializing renderer");
		if (create_render_tasks()) {
			if (create_renderer_buffers()) {
				if (create_swapchain()) {
					if (setup_presentation()) {
						if (create_descriptor_sets()) {
							if (create_renderer_pipelines()) {
								if (init_renderer_textures()) {
									if (init_renderer_sprite_layouts()) {
										if (init_renderer_meshes()) {
											for (VulkanTask &rRenderTask : aRenderTasks)
												for (uint8_t u8FunctionIndex = 0; u8FunctionIndex < 3; u8FunctionIndex++)
													rRenderTask.record(u8FunctionIndex, 0, nullptr);
											PRINT_DEBUG("Renderer successfully initialized");
											return true;
										}
										destroy_renderer_sprite_layout();
									}
									destroy_renderer_textures();
								}
								destroy_renderer_pipelines();
							}
							destroy_descriptor_sets();
						}
						destroy_presentation();
					}
					destroy_swapchain();
				}
				destroy_renderer_buffers();
			}
			destroy_render_tasks();
		}
		PRINT_DEBUG("Destroyed renderer after failure initializing it");
		return false;
	}

	void destroy_renderer() {
		PRINT_DEBUG("Destroying renderer");
		destroy_renderer_meshes();
		destroy_renderer_sprite_layout();
		destroy_renderer_textures();
		destroy_renderer_pipelines();
		destroy_descriptor_sets();
		destroy_presentation();
		destroy_swapchain();
		destroy_renderer_buffers();
		destroy_render_tasks();
	}

	void render() {
		PRINT_DEBUG("Invoking render-procedure");
		if (bSwapchainDirty)
			if (!recreate_swapchain())
				return;
		if (!acquire_next_swapchain_image())
			return;
		vkWaitForFences(vk_hDevice, 1, &vk_ahRenderFences[u8CurrentFrameInFlightIndex], VK_TRUE, std::numeric_limits<uint64_t>::max());
		aRenderTasks[u8CurrentFrameInFlightIndex].record(
				RENDER_TASK_SUBINDEX_IMAGE_BLIT,
				VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
				[](const VkCommandBuffer vk_hCommandBuffer, const uint8_t u8PreviousLogicalQueue, const uint8_t u8CurrentLogicalQueue, const uint8_t u8NextLogicalQueue) {
					constexpr VkImageSubresourceRange vk_imageRange = {
						.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
						.baseMipLevel = 0,
						.levelCount = 1,
						.baseArrayLayer = 0,
						.layerCount = 1
					};
					const VkImageMemoryBarrier vk_swapchainImage1Info = {
						.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
						.pNext = nullptr,
						.srcAccessMask = VK_ACCESS_NONE,
						.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT,
						.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED,
						.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
						.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
						.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
						.image = swapchainImages[u32SwapchainImageIndex],
						.subresourceRange = vk_imageRange
					};
					vkCmdPipelineBarrier(vk_hCommandBuffer, VK_PIPELINE_STAGE_NONE, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, nullptr, 0, nullptr, 1, &vk_swapchainImage1Info);
					const VkClearColorValue vk_clearImage = {
						.float32 = {backgroundClearColor.get_red(), backgroundClearColor.get_green(), backgroundClearColor.get_blue(), 1.0f}
					};
					vkCmdClearColorImage(vk_hCommandBuffer, swapchainImages[u32SwapchainImageIndex], VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, &vk_clearImage, 1, &vk_imageRange);
					const VkImageMemoryBarrier vk_swapchainImage2Info = {
						.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
						.pNext = nullptr,
						.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT,
						.dstAccessMask = VK_ACCESS_MEMORY_READ_BIT,
						.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
						.newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
						.srcQueueFamilyIndex = queueFamilyIndices[u8CurrentLogicalQueue],
						.dstQueueFamilyIndex = queueFamilyIndices[u8NextLogicalQueue],
						.image = swapchainImages[u32SwapchainImageIndex],
						.subresourceRange = vk_imageRange
					};
					vkCmdPipelineBarrier(vk_hCommandBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, 0, 0, nullptr, 0, nullptr, 1, &vk_swapchainImage2Info);
				});
		PRINT_DEBUG("Submitting rendering task at frame-in-flight index ", u8CurrentFrameInFlightIndex);
		vkResetFences(vk_hDevice, 1, &vk_ahRenderFences[u8CurrentFrameInFlightIndex]);
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
		u8CurrentFrameInFlightIndex = (u8CurrentFrameInFlightIndex + 1) % RE_VK_FRAMES_IN_FLIGHT;
		u32CurrentSwapchainSemaphoreIndex = (u32CurrentSwapchainSemaphoreIndex + 1) % u32SwapchainImageCount;
	}

	bool swapchain_created_renderer() {
		PRINT_DEBUG("Creating swapchain-related Vulkan objects in Renderer");
		return create_renderer_images();
	}

	void swapchain_destroyed_renderer() {
		PRINT_DEBUG("Destroying swapchain-related Vulkan objects in Renderer");
		destroy_renderer_images();
	}

	void set_background_color(const Color &rColor) {
		backgroundClearColor = rColor;
	}

	void set_background_color(const float fRed, const float fGreen, const float fBlue) {
		backgroundClearColor.set_red(fRed);
		backgroundClearColor.set_green(fGreen);
		backgroundClearColor.set_blue(fBlue);
	}

	[[nodiscard]]
	Color get_background_color() {
		return backgroundClearColor;
	}

	[[nodiscard]]
	bool is_sample_shading_enabled() {
		return f32SampleShadingRate != 0.0f;
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
			wait_for_rendering_finished();
			recreate_graphics_pipelines();
		}
	}

	[[nodiscard]]
	float get_sample_shading_rate() {
		return f32SampleShadingRate;
	}

}
