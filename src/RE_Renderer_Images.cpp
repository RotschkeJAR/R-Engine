#include "RE_Renderer_Images_Internal.hpp"
#include "RE_Main.hpp"
#include "RE_Window.hpp"

namespace RE {

	ScreenPercentageSettings screenPercentageSettings;
	Vector2u renderImageSize,
		maxRenderImageSize;
	VkFormat vk_eRenderTargetFormat;
	static bool bDynamicScreenPercentage = false;

	void get_queues_for_swapchain_images(std::vector<uint32_t> &rRenderTaskQueueIndices) {
		constexpr uint32_t au32FunctionsToLookup[] = {
			RENDER_TASK_SUBINDEX_RENDERING,
			RENDER_TASK_SUBINDEX_IMAGE_BLIT
		};
		constexpr uint32_t u32FunctionsToLookupCount = sizeof(au32FunctionsToLookup) / sizeof(au32FunctionsToLookup[0]);
		rRenderTaskQueueIndices.reserve(u32FunctionsToLookupCount);
		for (uint32_t u32FunctionToLookupIndex = 0; u32FunctionToLookupIndex < u32FunctionsToLookupCount; u32FunctionToLookupIndex++) {
			const uint32_t u32QueueFamilyIndex = queueFamilyIndices[aRenderTasks[0].logical_queue_index_for_function(au32FunctionsToLookup[u32FunctionToLookupIndex])];
			if (std::find(rRenderTaskQueueIndices.begin(), rRenderTaskQueueIndices.end(), u32QueueFamilyIndex) == rRenderTaskQueueIndices.end())
				rRenderTaskQueueIndices.push_back(u32QueueFamilyIndex);
		}
		const uint32_t u32PresentIndex = aRenderTasks[0].logical_queue_index_for_presentation();
		if (std::find(rRenderTaskQueueIndices.begin(), rRenderTaskQueueIndices.end(), u32PresentIndex) == rRenderTaskQueueIndices.end())
			rRenderTaskQueueIndices.push_back(u32PresentIndex);
	}

	bool create_renderer_images() {
		if (create_window_button_image()) {
			if (alloc_memory_for_renderer_images()) {
				if (create_window_button_image_views())
					return true;
				free_memory_for_renderer_images();
			}
			destroy_window_button_image();
		}
		return false;
	}

	bool create_swapchain_related_images() {
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
					if (alloc_memory_for_swapchain_related_images()) {
						if (create_depth_stencil_image_views()) {
							if (bSkipCreatingSinglesampledImage || create_singlesampled_image_views()) {
								if (bSkipCreatingRenderTargetImage || create_render_target_image_views())
									return true;
								destroy_singlesampled_image_views();
							}
							destroy_depth_stencil_image_views();
						}
						free_memory_for_swapchain_related_images();
					}
					destroy_render_target_image();
				}
				destroy_singlesampled_image();
			}
			destroy_depth_stencil_image();
		}
		return false;
	}

	void destroy_renderer_images() {
		destroy_window_button_image_views();
		destroy_window_button_image();
		free_memory_for_renderer_images();
	}

	void destroy_swapchain_related_images() {
		destroy_render_target_image_views();
		destroy_render_target_image();
		destroy_singlesampled_image_views();
		destroy_singlesampled_image();
		destroy_depth_stencil_image_views();
		destroy_depth_stencil_image();
		free_memory_for_swapchain_related_images();
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
			destroy_swapchain_related_images();
			create_swapchain_related_images();
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
			destroy_swapchain_related_images();
			create_swapchain_related_images();
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
			destroy_swapchain_related_images();
			create_swapchain_related_images();
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
