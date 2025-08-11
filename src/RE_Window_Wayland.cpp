#include "RE_Window.hpp"

#ifdef RE_OS_LINUX

namespace RE {

	wl_display *wl_pDisplay = nullptr;
	wl_registry *wl_pRegistry = nullptr;
	wl_compositor *wl_pCompositor = nullptr;
	wl_surface *wl_pSurface = nullptr;
	xdg_wm_base *xdg_pWindowBase = nullptr;
	xdg_surface *xdg_pSurface = nullptr;
	xdg_toplevel *xdg_pToplevel = nullptr;

	static void commit_wayland_surface() {
		CATCH_SIGNAL(wl_surface_commit(wl_pSurface));
		CATCH_SIGNAL(wl_display_flush(wl_pDisplay));
	}

	static void window_base_ping_handler(void *const pData, xdg_wm_base *const wl_pBase, const uint32_t u32Serial) {
		CATCH_SIGNAL(xdg_wm_base_pong(wl_pBase, u32Serial));
	}

	void registry_handle_global(void *const pData, wl_registry *const wl_pRegistry, const uint32_t u32Name, const char * const pacInterface, const uint32_t u32Version) {
		if (are_string_contents_equal(pacInterface, wl_compositor_interface.name))
			CATCH_SIGNAL(wl_pCompositor = static_cast<wl_compositor*>(wl_registry_bind(wl_pRegistry, u32Name, &wl_compositor_interface, u32Version)));
		else if (are_string_contents_equal(pacInterface, xdg_wm_base_interface.name))
			CATCH_SIGNAL(xdg_pWindowBase = static_cast<xdg_wm_base*>(wl_registry_bind(wl_pRegistry, u32Name, &xdg_wm_base_interface, u32Version)));
	}

	static void registry_handle_global_remove(void *const pData, wl_registry *const wl_pRegistry, const uint32_t u32Name) {
	}

	static void wayland_configure_window_event(void *const pData, xdg_surface *const xdg_pSurface, const uint32_t u32Serial) {
		CATCH_SIGNAL(xdg_surface_ack_configure(xdg_pSurface, u32Serial));
	}

	static void register_next_ready_frame_callback();

	static void wayland_frame_ready_callback(void *const pData, wl_callback *const wl_pCallback, const uint32_t u32Serial) {
		CATCH_SIGNAL(wl_callback_destroy(wl_pCallback));
		set_bits<uint8_t>(u8WindowFlagBits, true, WINDOW_WAYLAND_SHOULD_RENDER_FRAME_BIT);
		PRINT_LN("called frame ready");
		CATCH_SIGNAL(register_next_ready_frame_callback());
	}

	static void register_next_ready_frame_callback() {
		wl_callback *const wl_pFrameReadyCallback = CATCH_SIGNAL_AND_RETURN(wl_surface_frame(wl_pSurface), wl_callback*);
		const wl_callback_listener wl_frameReadyCallback = {
			.done = wayland_frame_ready_callback
		};
		CATCH_SIGNAL(wl_callback_add_listener(wl_pFrameReadyCallback, &wl_frameReadyCallback, nullptr));
		CATCH_SIGNAL(commit_wayland_surface());
	}

	bool wayland_create_window() {
		wl_pDisplay = wl_display_connect(nullptr);
		if (wl_pDisplay) {
			wl_pRegistry = CATCH_SIGNAL_AND_RETURN(wl_display_get_registry(wl_pDisplay), wl_registry*);
			const wl_registry_listener wl_registryListeners = {
				.global = registry_handle_global,
				.global_remove = registry_handle_global_remove
			};
			CATCH_SIGNAL(wl_registry_add_listener(wl_pRegistry, &wl_registryListeners, nullptr));
			CATCH_SIGNAL(wl_display_roundtrip(wl_pDisplay));
			if (wl_pCompositor) {
				if (xdg_pWindowBase) {
					const xdg_wm_base_listener xdg_listener = {
						.ping = window_base_ping_handler
					};
					CATCH_SIGNAL(xdg_wm_base_add_listener(xdg_pWindowBase, &xdg_listener, nullptr));
					CATCH_SIGNAL(wl_pSurface = wl_compositor_create_surface(wl_pCompositor));
					if (wl_pSurface) {
						CATCH_SIGNAL(xdg_pSurface = xdg_wm_base_get_xdg_surface(xdg_pWindowBase, wl_pSurface));
						if (xdg_pSurface) {
							CATCH_SIGNAL(xdg_pToplevel = xdg_surface_get_toplevel(xdg_pSurface));
							if (xdg_pToplevel) {
								const xdg_surface_listener xdg_surfaceListener = {
									.configure = wayland_configure_window_event
								};
								CATCH_SIGNAL(xdg_surface_add_listener(xdg_pSurface, &xdg_surfaceListener, nullptr));
								CATCH_SIGNAL(xdg_toplevel_set_title(xdg_pToplevel, pacWindowTitle));
								CATCH_SIGNAL(xdg_surface_set_window_geometry(xdg_pSurface, 0, 0, windowSize[0], windowSize[1]));
								CATCH_SIGNAL(xdg_toplevel_set_min_size(xdg_pToplevel, MIN_WINDOW_WIDTH, MIN_WINDOW_HEIGHT));
								CATCH_SIGNAL(register_next_ready_frame_callback());
								return true;
							} else
								RE_FATAL_ERROR("Failed to get toplevel of the XDG window for Wayland");
							CATCH_SIGNAL(xdg_surface_destroy(xdg_pSurface));
							xdg_pSurface = nullptr;
						} else
							RE_FATAL_ERROR("Failed to create an XDG window for Wayland");
						CATCH_SIGNAL(wl_surface_destroy(wl_pSurface));
						wl_pSurface = nullptr;
					} else
						RE_FATAL_ERROR("Failed to create a Wayland surface");
					CATCH_SIGNAL(xdg_wm_base_destroy(xdg_pWindowBase));
					xdg_pWindowBase = nullptr;
				} else
					RE_FATAL_ERROR("Failed to get Wayland window base");
				CATCH_SIGNAL(wl_compositor_destroy(wl_pCompositor));
				wl_pCompositor = nullptr;
			} else
				RE_FATAL_ERROR("Failed to get Wayland compositor");
			CATCH_SIGNAL(wl_registry_destroy(wl_pRegistry));
			CATCH_SIGNAL(wl_display_disconnect(wl_pDisplay));
			wl_pRegistry = nullptr;
			wl_pDisplay = nullptr;
		} else
			RE_FATAL_ERROR("Failed connecting to Wayland server");
		return false;
	}

	void wayland_destroy_window() {
		CATCH_SIGNAL(xdg_toplevel_destroy(xdg_pToplevel));
		CATCH_SIGNAL(xdg_surface_destroy(xdg_pSurface));
		CATCH_SIGNAL(wl_surface_destroy(wl_pSurface));
		CATCH_SIGNAL(xdg_wm_base_destroy(xdg_pWindowBase));
		CATCH_SIGNAL(wl_compositor_destroy(wl_pCompositor));
		CATCH_SIGNAL(wl_registry_destroy(wl_pRegistry));
		CATCH_SIGNAL(wl_display_disconnect(wl_pDisplay));
		xdg_pToplevel = nullptr;
		xdg_pSurface = nullptr;
		wl_pSurface = nullptr;
		xdg_pWindowBase = nullptr;
		wl_pCompositor = nullptr;
		wl_pRegistry = nullptr;
		wl_pDisplay = nullptr;
	}
	
	void wayland_show_window() {
		CATCH_SIGNAL(commit_wayland_surface());
	}
	
	void wayland_update_window_title() {
		CATCH_SIGNAL(xdg_toplevel_set_title(xdg_pToplevel, pacWindowTitle));
		CATCH_SIGNAL(commit_wayland_surface());
	}

	void wayland_window_proc() {
		//while (CATCH_SIGNAL_AND_RETURN(wl_display_dispatch(wl_pDisplay), int32_t) > 0) {
		//}
	}

	void wayland_post_rendering_window_proc() {
		CATCH_SIGNAL(commit_wayland_surface());
		set_bits<uint8_t>(u8WindowFlagBits, false, WINDOW_WAYLAND_SHOULD_RENDER_FRAME_BIT);
	}

}

#endif /* RE_OS_LINUX */
