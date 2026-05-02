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
				if (create_descriptor_sets()) {
					if (create_swapchain()) {
						if (setup_presentation()) {
							if (create_graphics_pipelines()) {
								if (create_compute_pipelines()) {
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
		destroy_render_tasks();
	}

	void render() {
		PRINT_DEBUG("Invoking render-procedure");
		if (bSwapchainDirty)
			if (!recreate_swapchain())
				return;
		if (acquire_next_swapchain_image()) {

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
