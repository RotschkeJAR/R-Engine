#include "RE_Renderer_Internal.hpp"
#include "RE_Window.hpp"
#include "RE_Main.hpp"
#include "RE_Settings.hpp"

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
									if (create_render_pass()) {
										if (create_renderer_pipelines()) {
											if (init_renderer_textures()) {
												if (init_renderer_meshes()) {
													for (VulkanTask &rRenderTask : aRenderTasks)
														for (unsigned uFunctionIndex = 0; uFunctionIndex < aRenderTasks[0].function_count(); uFunctionIndex++)
															rRenderTask.record(uFunctionIndex, 0, nullptr);
													return true;
												}
												destroy_renderer_textures();
											}
											destroy_renderer_pipelines();
										}
										destroy_render_pass();
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
		destroy_renderer_meshes();
		destroy_renderer_sprite_layout();
		destroy_renderer_textures();
		destroy_renderer_pipelines();
		destroy_descriptor_sets();
		destroy_presentation();
		destroy_swapchain();
		destroy_render_pass();
		destroy_renderer_images();
		destroy_renderer_buffers();
		destroy_render_tasks();
	}

	void render() {
		if (bSwapchainDirty) {
			if (!recreate_swapchain())
				return;
		}
		if (!should_render()) {
		#ifdef RE_OS_LINUX
			switch (eLinuxWindowType) {
				case LINUX_WINDOW_TYPE_X11:
					break;
				case LINUX_WINDOW_TYPE_WAYLAND:
					present_empty();
					goto INCREASE_INDICES_RENDERER;
			}
		#endif
			return;
		}
		if ((mSettingsFlags & SETTINGS_FLAG_MENU_OPEN_BIT))
			render_settings_gui();
		else
			render_procedure();
		goto INCREASE_INDICES_RENDERER;
		
	INCREASE_INDICES_RENDERER:
		u8CurrentFrameInFlightIndex = (u8CurrentFrameInFlightIndex + 1) % RE_VK_FRAMES_IN_FLIGHT;
		u32CurrentSwapchainSemaphoreIndex = (u32CurrentSwapchainSemaphoreIndex + 1) % u32SwapchainImageCount;
	}

	bool swapchain_created_renderer() {
		if (create_swapchain_related_images()) {
			if (create_renderer_framebuffers()) {
				return true;
			}
			destroy_swapchain_related_images();
		}
		return false;
	}

	void swapchain_destroyed_renderer() {
		destroy_renderer_framebuffers();
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

	void set_sample_shading_rate(const float fNewSampleShadingRate) {
		if (fSampleShadingRate == fNewSampleShadingRate)
			return;
		if (fNewSampleShadingRate < 0.0f || fNewSampleShadingRate > 1.0f) {
			RE_ERROR("Sample shading rate should be in range between 0.0 and 1.0, but was ", fNewSampleShadingRate, ". Request to change it has been discarded");
			return;
		} else {
			PRINT_DEBUG("Setting sample shading rate to ", fNewSampleShadingRate);
			fSampleShadingRate = fNewSampleShadingRate;
			wait_for_rendering_finished();
			recreate_graphics_pipelines();
		}
	}

	float get_sample_shading_rate() {
		return fSampleShadingRate;
	}

}
