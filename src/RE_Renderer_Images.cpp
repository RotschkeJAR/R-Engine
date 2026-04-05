#include "RE_Renderer_Images_Internal.hpp"
#include "RE_Main.hpp"
#include "RE_Window.hpp"

namespace RE {

	ScreenPercentageSettings screenPercentageSettings;
	Vector2u renderImageSize,
		maxRenderImageSize;
	VkFormat vk_eRenderTargetFormat;
	static bool bDynamicScreenPercentage = false;

	static void get_queues_for_render_images(std::vector<uint32_t> &rRenderTaskQueueIndices) {
		constexpr uint32_t au32FunctionsToLookup[] = {
			RENDER_TASK_SUBINDEX_RENDERING,
			RENDER_TASK_SUBINDEX_IMAGE_BLIT
		};
		constexpr uint32_t u32FunctionsToLookupCount = sizeof(au32FunctionsToLookup) / sizeof(au32FunctionsToLookup[0]);
		rRenderTaskQueueIndices.reserve(u32FunctionsToLookupCount);
		for (uint32_t u32FunctionToLookupIndex = 0; u32FunctionToLookupIndex < u32FunctionsToLookupCount; u32FunctionToLookupIndex++) {
			const uint32_t u32QueueFamilyIndex = queueFamilyIndices[renderTasks[0].logical_queue_index_for_function(au32FunctionsToLookup[u32FunctionToLookupIndex])];
			if (std::find(rRenderTaskQueueIndices.begin(), rRenderTaskQueueIndices.end(), u32QueueFamilyIndex) == rRenderTaskQueueIndices.end())
				rRenderTaskQueueIndices.push_back(u32QueueFamilyIndex);
		}
	}

	void get_queues_for_swapchain_images(std::vector<uint32_t> &rRenderTaskQueueIndices) {
		get_queues_for_render_images(rRenderTaskQueueIndices);
		const uint32_t u32PresentIndex = renderTasks[0].logical_queue_index_for_presentation();
		if (std::find(rRenderTaskQueueIndices.begin(), rRenderTaskQueueIndices.end(), u32PresentIndex) == rRenderTaskQueueIndices.end())
			rRenderTaskQueueIndices.push_back(u32PresentIndex);
	}

	bool create_images_renderer() {
		PRINT_DEBUG("Calculating size of renderable images");
		switch (screenPercentageSettings.eMode) {
			case RE_SCREEN_PERCENTAGE_MODE_NORMAL:
				renderImageSize[0] = vk_swapchainResolution.width;
				renderImageSize[1] = vk_swapchainResolution.height;
				break;
			case RE_SCREEN_PERCENTAGE_MODE_SCALED:
				renderImageSize[0] = static_cast<uint32_t>(std::round(vk_swapchainResolution.width * screenPercentageSettings.f32Scale));
				renderImageSize[1] = static_cast<uint32_t>(std::round(vk_swapchainResolution.height * screenPercentageSettings.f32Scale));
				break;
			case RE_SCREEN_PERCENTAGE_MODE_CONST_SIZE:
				if (screenPercentageSettings.constSize.any_of([](const decltype(screenPercentageSettings.constSize)::type size) {return size <= 0;}))
					screenPercentageSettings.constSize = largestMonitorSize;
				renderImageSize = screenPercentageSettings.constSize;
				break;
			[[unlikely]] default:
				RE_ABORT("Unknown screen percentage mode selected ", std::hex, screenPercentageSettings.eMode, " in the settings");
		}
		if (maxRenderImageSize.any_of([](const decltype(maxRenderImageSize)::type size) {return size <= 0;}))
			maxRenderImageSize = largestMonitorSize;
		if (bDynamicScreenPercentage && screenPercentageSettings.eMode != RE_SCREEN_PERCENTAGE_MODE_CONST_SIZE) {
			renderImageSize[0] = std::clamp<uint32_t>(renderImageSize[0], 1, maxRenderImageSize[0]);
			renderImageSize[1] = std::clamp<uint32_t>(renderImageSize[1], 1, maxRenderImageSize[1]);
			return true;
		}
		const bool bBlittingRequired = renderImageSize[0] != vk_swapchainResolution.width || renderImageSize[1] != vk_swapchainResolution.height,
			bResolvingRequired = vk_eMsaaCount != VK_SAMPLE_COUNT_1_BIT,
			bSkipCreatingSinglesampledImage = !(bBlittingRequired && bResolvingRequired),
			bSkipCreatingRenderTargetImage = bBlittingRequired || bResolvingRequired;
		if (create_depth_stencil_image()) {
			if (bSkipCreatingSinglesampledImage || create_singlesampled_image()) {
				if (bSkipCreatingRenderTargetImage || create_render_target_image()) {
					if (alloc_memory_for_images_renderer()) {
						if (create_depth_stencil_image_views()) {
							if (bSkipCreatingSinglesampledImage || create_singlesampled_image_views()) {
								if (bSkipCreatingRenderTargetImage || create_render_target_image_views())
									return true;
								destroy_singlesampled_image_views();
							}
							destroy_depth_stencil_image_views();
						}
						free_memory_for_images_renderer();
					}
					destroy_render_target_image();
				}
				destroy_singlesampled_image();
			}
			destroy_depth_stencil_image();
		}
		return false;
	}

	void destroy_images_renderer() {
		PRINT_DEBUG("Destroying render image resources");
		destroy_render_target_image_views();
		destroy_render_target_image();
		destroy_singlesampled_image_views();
		destroy_singlesampled_image();
		destroy_depth_stencil_image_views();
		destroy_depth_stencil_image();
		free_memory_for_images_renderer();
	}

	bool record_cmd_blitting_render_image() {
		if (renderTasks[u8CurrentFrameInFlightIndex].record(RENDER_TASK_SUBINDEX_IMAGE_BLIT, VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT, [&](const VkCommandBuffer vk_hCommandBuffer, const uint8_t u8PreviousLogicalQueue, const uint8_t u8CurrentLogicalQueue, const uint8_t u8NextLogicalQueue) {
				VkImageMemoryBarrier2 vk_aImageLayoutTransferRegion[2];
				vk_aImageLayoutTransferRegion[1].sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2;
				vk_aImageLayoutTransferRegion[1].pNext = nullptr;
				vk_aImageLayoutTransferRegion[1].srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
				vk_aImageLayoutTransferRegion[1].dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
				vk_aImageLayoutTransferRegion[1].image = swapchainImages[u32SwapchainImageIndex];
				vk_aImageLayoutTransferRegion[1].subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
				vk_aImageLayoutTransferRegion[1].subresourceRange.baseMipLevel = 0;
				vk_aImageLayoutTransferRegion[1].subresourceRange.levelCount = 1;
				vk_aImageLayoutTransferRegion[1].subresourceRange.baseArrayLayer = 0;
				vk_aImageLayoutTransferRegion[1].subresourceRange.layerCount = 1;
				VkDependencyInfo vk_imageLayoutTransferInfo;
				vk_imageLayoutTransferInfo.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO;
				vk_imageLayoutTransferInfo.pNext = nullptr;
				vk_imageLayoutTransferInfo.dependencyFlags = 0;
				vk_imageLayoutTransferInfo.memoryBarrierCount = 0;
				vk_imageLayoutTransferInfo.pMemoryBarriers = nullptr;
				vk_imageLayoutTransferInfo.bufferMemoryBarrierCount = 0;
				vk_imageLayoutTransferInfo.pBufferMemoryBarriers = nullptr;
				if (screenPercentageSettings.eMode != RE_SCREEN_PERCENTAGE_MODE_NORMAL) {
					PRINT_DEBUG("Transferring render image's layout from optimal for rendering to transfer source and swapchain image's to transfer destination");
					vk_aImageLayoutTransferRegion[0].sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2;
					vk_aImageLayoutTransferRegion[0].pNext = nullptr;
					vk_aImageLayoutTransferRegion[0].srcStageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT;
					vk_aImageLayoutTransferRegion[0].srcAccessMask = VK_ACCESS_2_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT;
					vk_aImageLayoutTransferRegion[0].dstStageMask = VK_PIPELINE_STAGE_2_TRANSFER_BIT;
					vk_aImageLayoutTransferRegion[0].dstAccessMask = VK_ACCESS_2_TRANSFER_READ_BIT;
					vk_aImageLayoutTransferRegion[0].oldLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
					vk_aImageLayoutTransferRegion[0].newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
					vk_aImageLayoutTransferRegion[0].srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
					vk_aImageLayoutTransferRegion[0].dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
					vk_aImageLayoutTransferRegion[0].image = vk_eMsaaCount != VK_SAMPLE_COUNT_1_BIT ? vk_hSinglesampledImage : vk_hRenderTargetImage;
					vk_aImageLayoutTransferRegion[0].subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
					vk_aImageLayoutTransferRegion[0].subresourceRange.baseMipLevel = 0;
					vk_aImageLayoutTransferRegion[0].subresourceRange.levelCount = 1;
					vk_aImageLayoutTransferRegion[0].subresourceRange.baseArrayLayer = u8CurrentFrameInFlightIndex;
					vk_aImageLayoutTransferRegion[0].subresourceRange.layerCount = 1;
					vk_aImageLayoutTransferRegion[1].srcStageMask = VK_PIPELINE_STAGE_2_NONE;
					vk_aImageLayoutTransferRegion[1].srcAccessMask = VK_ACCESS_2_NONE;
					vk_aImageLayoutTransferRegion[1].dstStageMask = VK_PIPELINE_STAGE_2_TRANSFER_BIT;
					vk_aImageLayoutTransferRegion[1].dstAccessMask = VK_ACCESS_2_TRANSFER_WRITE_BIT;
					vk_aImageLayoutTransferRegion[1].oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
					vk_aImageLayoutTransferRegion[1].newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
					vk_imageLayoutTransferInfo.imageMemoryBarrierCount = 2;
					vk_imageLayoutTransferInfo.pImageMemoryBarriers = vk_aImageLayoutTransferRegion;
					vkCmdPipelineBarrier2(vk_hCommandBuffer, &vk_imageLayoutTransferInfo);
					PRINT_DEBUG("Blitting rendered image");
					const VkImageBlit2 vk_blitRegion = {
						.sType = VK_STRUCTURE_TYPE_IMAGE_BLIT_2,
						.srcSubresource = {
							.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
							.mipLevel = 0,
							.baseArrayLayer = u8CurrentFrameInFlightIndex,
							.layerCount = 1
						},
						.srcOffsets = {
							{}, 
							{
								.x = static_cast<int32_t>(renderImageSize[0]),
								.y = static_cast<int32_t>(renderImageSize[1]),
								.z = 1
							}
						},
						.dstSubresource = {
							.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
							.mipLevel = 0,
							.baseArrayLayer = 0,
							.layerCount = 1
						},
						.dstOffsets = {
							{},
							{
								.x = static_cast<int32_t>(vk_swapchainResolution.width),
								.y = static_cast<int32_t>(vk_swapchainResolution.height),
								.z = 1
							}
						}
					};
					VkBlitImageInfo2 vk_blitInfo = {
						.sType = VK_STRUCTURE_TYPE_BLIT_IMAGE_INFO_2,
						.srcImage = vk_aImageLayoutTransferRegion[0].image,
						.srcImageLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
						.dstImage = swapchainImages[u32SwapchainImageIndex],
						.dstImageLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
						.regionCount = 1,
						.pRegions = &vk_blitRegion
					};
					switch (screenPercentageSettings.eScalingFilter) {
						case RE_TEXTURE_FILTER_NEAREST:
							vk_blitInfo.filter = VK_FILTER_NEAREST;
							break;
						case RE_TEXTURE_FILTER_LINEAR:
							vk_blitInfo.filter = VK_FILTER_LINEAR;
							break;
					}
					vkCmdBlitImage2(vk_hCommandBuffer, &vk_blitInfo);
					vk_aImageLayoutTransferRegion[1].srcStageMask = VK_PIPELINE_STAGE_2_TRANSFER_BIT;
					vk_aImageLayoutTransferRegion[1].srcAccessMask = VK_ACCESS_2_TRANSFER_WRITE_BIT;
					vk_aImageLayoutTransferRegion[1].oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
				} else {
					vk_aImageLayoutTransferRegion[1].srcStageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT;
					vk_aImageLayoutTransferRegion[1].srcAccessMask = VK_ACCESS_2_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT;
					vk_aImageLayoutTransferRegion[1].oldLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
				}
				PRINT_DEBUG("Transferring swapchain image's layout to presentable source");
				vk_aImageLayoutTransferRegion[1].dstStageMask = VK_PIPELINE_STAGE_2_NONE;
				vk_aImageLayoutTransferRegion[1].dstAccessMask = VK_ACCESS_2_NONE;
				vk_aImageLayoutTransferRegion[1].newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
				vk_imageLayoutTransferInfo.imageMemoryBarrierCount = 1;
				vk_imageLayoutTransferInfo.pImageMemoryBarriers = &vk_aImageLayoutTransferRegion[1];
				vkCmdPipelineBarrier2(vk_hCommandBuffer, &vk_imageLayoutTransferInfo);
		})) {
			return true;
		} else
			RE_FATAL_ERROR("Failed recording command buffer for transfering rendered image to swapchain image");
		return false;
	}

	void set_screen_percentage_settings(ScreenPercentageSettings newSettings) {
		bool bNormalInstead = false;
		if (screenPercentageSettings.eMode == newSettings.eMode)
			switch (newSettings.eMode) {
				case RE_SCREEN_PERCENTAGE_MODE_NORMAL:
					return;
				case RE_SCREEN_PERCENTAGE_MODE_SCALED:
					if (newSettings.f32Scale == 1.0f) {
						RE_WARNING("Screen percentage had to be changed to 100%, however this has the same effect like the normal mode. Therefore the normal mode will be enabled instead");
						if (screenPercentageSettings.eMode == RE_SCREEN_PERCENTAGE_MODE_NORMAL)
							return;
						bNormalInstead = true;
					} else if (screenPercentageSettings.f32Scale == newSettings.f32Scale && screenPercentageSettings.eScalingFilter == newSettings.eScalingFilter)
						return;
					break;
				case RE_SCREEN_PERCENTAGE_MODE_CONST_SIZE:
					if (newSettings.constSize.any_of(0)) {
						RE_WARNING("The new constant size of the updated screen percentage settings has a member equal zero. The changes won't be applied");
						return;
					} else if (screenPercentageSettings.constSize == newSettings.constSize && screenPercentageSettings.eScalingFilter == newSettings.eScalingFilter)
						return;
					break;
				[[unlikely]] default:
					RE_ABORT("Unknown screen percentage mode, that was supposed to be updated to: ", std::hex, newSettings.eMode);
			}
		PRINT_DEBUG("Updating screen percentage settings");
		screenPercentageSettings = newSettings;
		if (bNormalInstead)
			screenPercentageSettings.eMode = RE_SCREEN_PERCENTAGE_MODE_NORMAL;
		if (!bSwapchainDirty && bRunning) {
			PRINT_DEBUG("Applying new screen percentage settings");
			wait_for_rendering_finished();
			destroy_images_renderer();
			create_images_renderer();
		}
	}

	[[nodiscard]]
	ScreenPercentageSettings get_screen_percentage_settings() {
		return screenPercentageSettings;
	}

	void enable_dynamic_screen_percentage(const bool bEnable) {
		if (bDynamicScreenPercentage == bEnable)
			return;
		PRINT_DEBUG("Updating dynamic screen scaling settings (enabled: ", bEnable, ")");
		bDynamicScreenPercentage = bEnable;
		if (!bSwapchainDirty && bRunning) {
			PRINT_DEBUG("Applying new dynamic screen scaling settings");
			wait_for_rendering_finished();
			destroy_images_renderer();
			create_images_renderer();
		}
	}

	[[nodiscard]]
	bool is_dynamic_screen_percentage_enabled() {
		return bDynamicScreenPercentage;
	}

	void set_maximum_size_for_dynamic_screen_percentage(Vector2u newSize) {
		if (maxRenderImageSize == newSize)
			return;
		PRINT_DEBUG("Updating maximum size of dynamic screen percentage to ", newSize);
		maxRenderImageSize = newSize;
		if (!bSwapchainDirty && bRunning) {
			PRINT_DEBUG("Applying new maximum size of the dynamic screen percentage");
			wait_for_rendering_finished();
			destroy_images_renderer();
			create_images_renderer();
		}
	}

	void set_maximum_size_for_dynamic_screen_percentage(const uint32_t u32MaxWidth, const uint32_t u32MaxHeight) {
		set_maximum_size_for_dynamic_screen_percentage(Vector2u(u32MaxWidth, u32MaxHeight));
	}

	[[nodiscard]]
	Vector2u get_maximum_size_for_dynamic_screen_scaling() {
		return maxRenderImageSize;
	}

	[[nodiscard]]
	uint32_t get_maximum_width_for_dynamic_screen_scaling() {
		return maxRenderImageSize[0];
	}

	[[nodiscard]]
	uint32_t get_maximum_height_for_dynamic_screen_scaling() {
		return maxRenderImageSize[1];
	}

}
