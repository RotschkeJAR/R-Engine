#include "RE_Renderer_Internal.hpp"
#include "RE_Window.hpp"
#include "RE_Main.hpp"
#include "RE_Settings.hpp"

namespace RE {

	Color backgroundClearColor(0.2f, 0.2f, 0.2f, 1.0f);

	unsigned uCurrentFrameInFlightIndex = 0;

	bool init_renderer() {
		vk_eSwapchainImageFormat = surfaceFormatsAvailable[u32IndexToSelectedSurfaceFormat].format;
		if (create_render_tasks()) {
			if (create_renderer_buffers()) {
				if (create_renderer_images()) {
					if (create_render_pass()) {
						if (create_swapchain()) {
							if (setup_presentation()) {
								if (init_renderer_sprite_layouts()) {
									if (create_descriptor_sets()) {
										if (create_renderer_pipelines()) {
											if (init_renderer_textures()) {
												if (init_renderer_meshes()) {
													if (init_window_frame()) {
														init_camera_matrices();
														for (VulkanTask &rRenderTask : aRenderTasks)
															for (unsigned uFunctionIndex = 0; uFunctionIndex < aRenderTasks[0].function_count(); uFunctionIndex++)
																rRenderTask.record(uFunctionIndex, 0, nullptr);
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
						destroy_render_pass();
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
		destroy_window_frame();
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
		if ((mSettingsFlags & SETTINGS_FLAG_MENU_OPEN_BIT)) {
			[[maybe_unused]] bool bDiscardCode = present_empty();
		} else {
			render_procedure();
		}
		goto INCREASE_INDICES_RENDERER;
		
	INCREASE_INDICES_RENDERER:
		uCurrentFrameInFlightIndex = (uCurrentFrameInFlightIndex + 1U) % RE_VK_FRAMES_IN_FLIGHT;
		u32CurrentSwapchainSemaphoreIndex = (u32CurrentSwapchainSemaphoreIndex + 1U) % u32SwapchainImageCount;
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
		backgroundClearColor.set_alpha(1.0f);
	}

	void set_background_color(float fRed, float fGreen, float fBlue) {
		backgroundClearColor.set_red(fRed);
		backgroundClearColor.set_green(fGreen);
		backgroundClearColor.set_blue(fBlue);
	}

	Color get_background_color() {
		return backgroundClearColor;
	}

}
