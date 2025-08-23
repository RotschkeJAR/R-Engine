#include "RE_Window.hpp"

#ifdef RE_OS_LINUX

namespace RE {

	wl_display *wl_pDisplay = nullptr;
	wl_registry *wl_pRegistry = nullptr;
	wl_compositor *wl_pCompositor = nullptr;
	wl_seat *wl_pSeat = nullptr;
	xdg_wm_base *xdg_pWindowBase = nullptr;
	wl_pointer *wl_pMouse = nullptr;
	wl_surface *wl_pSurface = nullptr;
	xdg_surface *xdg_pSurface = nullptr;
	xdg_toplevel *xdg_pToplevel = nullptr;

	static void commit_wayland_surface() {
		PUSH_TO_CALLSTACKTRACE(wl_surface_commit(wl_pSurface));
		PUSH_TO_CALLSTACKTRACE(wl_display_flush(wl_pDisplay));
	}

	static void callback_xdg_wm_base_ping(void *const pData, xdg_wm_base *const xdg_pBase, const uint32_t u32Serial) {
		PUSH_TO_CALLSTACKTRACE(xdg_wm_base_pong(xdg_pBase, u32Serial));
	}

	static void callback_wayland_registry_handler(void *const pData, wl_registry *const wl_pRegistryParam, const uint32_t u32Name, const char * const pacInterface, const uint32_t u32Version) {
		if (are_string_contents_equal(pacInterface, wl_compositor_interface.name))
			wl_pCompositor = PUSH_TO_CALLSTACKTRACE_AND_RETURN(static_cast<wl_compositor*>(wl_registry_bind(wl_pRegistryParam, u32Name, &wl_compositor_interface, u32Version)), wl_compositor*);
		else if (are_string_contents_equal(pacInterface, wl_seat_interface.name))
			wl_pSeat = PUSH_TO_CALLSTACKTRACE_AND_RETURN(static_cast<wl_seat*>(wl_registry_bind(wl_pRegistryParam, u32Name, &wl_seat_interface, u32Version)), wl_seat*);
		else if (are_string_contents_equal(pacInterface, xdg_wm_base_interface.name))
			xdg_pWindowBase = PUSH_TO_CALLSTACKTRACE_AND_RETURN(static_cast<xdg_wm_base*>(wl_registry_bind(wl_pRegistryParam, u32Name, &xdg_wm_base_interface, u32Version)), xdg_wm_base*);
	}
	static void callback_wayland_registry_remove(void *const pData, wl_registry *const wl_pRegistryParam, const uint32_t u32Name) {}

	static void callback_wayland_mouse_enter(void *const pData, wl_pointer *const wl_pMouseParam, const uint32_t u32Serial, wl_surface *const wl_pSurfaceParam, const wl_fixed_t wl_cursorX, const wl_fixed_t wl_cursorY) {
		if (wl_pSurfaceParam != wl_pSurface)
			return;
		PUSH_TO_CALLSTACKTRACE(xdg_toplevel_show_window_menu(xdg_pToplevel, wl_pSeat, u32Serial, 0, 0));
	}
	static void callback_wayland_mouse_leave(void *const pData, wl_pointer *const wl_pMouseParam, const uint32_t u32Serial, wl_surface *const wl_pSurfaceParam) {}
	static void callback_wayland_mouse_motion_input(void *const pData, wl_pointer *const wl_pMouseParam, const uint32_t u32Time, const wl_fixed_t wl_cursorX, const wl_fixed_t wl_cursorY) {}
	static void callback_wayland_mouse_button_input(void *const pData, wl_pointer *const wl_pMouseParam, const uint32_t u32Serial, const uint32_t u32Time, const uint32_t u32Button, const uint32_t u32State) {}
	static void callback_wayland_mouse_scroll_input(void *const pData, wl_pointer *const wl_pMouseParam, const uint32_t u32Time, const uint32_t u32Axis, const wl_fixed_t wl_value) {}
	static void callback_wayland_mouse_input_frame_finish(void *const pData, wl_pointer *const wl_pMouseParam) {}
	static void callback_wayland_mouse_scroll_stop_input(void *const pData, wl_pointer *const wl_pMouseParam, const uint32_t u32Time, const uint32_t u32Axis) {}

	static void callback_wayland_surface_configure(void *const pData, xdg_surface *const xdg_pSurface, const uint32_t u32Serial) {
		PUSH_TO_CALLSTACKTRACE(xdg_surface_ack_configure(xdg_pSurface, u32Serial));
	}

	static void register_next_ready_frame_callback();
	static void callback_wayland_surface_frame_ready(void *const pData, wl_callback *const wl_pCallback, const uint32_t u32Serial) {
		PUSH_TO_CALLSTACKTRACE(wl_callback_destroy(wl_pCallback));
		set_bits<uint8_t>(u8WindowFlagBits, true, WINDOW_WAYLAND_SHOULD_RENDER_FRAME_BIT);
		RE_NOTE("called callback frame ready");
		PUSH_TO_CALLSTACKTRACE(register_next_ready_frame_callback());
	}
	static void register_next_ready_frame_callback() {
		wl_callback *const wl_pFrameReadyCallback = PUSH_TO_CALLSTACKTRACE_AND_RETURN(wl_surface_frame(wl_pSurface), wl_callback*);
		const wl_callback_listener wl_frameReadyCallback = {
			.done = callback_wayland_surface_frame_ready
		};
		RE_NOTE("setting callback up");
		PUSH_TO_CALLSTACKTRACE(wl_callback_add_listener(wl_pFrameReadyCallback, &wl_frameReadyCallback, nullptr));
		PUSH_TO_CALLSTACKTRACE(commit_wayland_surface());
	}

	static void callback_xdg_toplevel_configure(void *const pData, xdg_toplevel *const xdg_pToplevelParam, const int32_t i32Width, const int32_t i32Height, wl_array *const wl_pArray) {}
	static void callback_xdg_toplevel_close(void *const pData, xdg_toplevel *const xdg_pToplevelParam) {
		set_bits<uint8_t>(u8WindowFlagBits, WINDOW_CLOSE_FLAG_BIT, true);
	}
	static void callback_xdg_toplevel_configure_bounds(void *const pData, xdg_toplevel *const xdg_pToplevelParam, const int32_t i32Width, const int32_t i32Height) {}
	static void callback_xdg_toplevel_wm_capabilities(void *const pData, xdg_toplevel *const xdg_pToplevelParam, wl_array *const wl_pArray) {}

	bool wayland_create_window() {
		wl_pDisplay = wl_display_connect(nullptr);
		if (wl_pDisplay) {
			wl_pRegistry = PUSH_TO_CALLSTACKTRACE_AND_RETURN(wl_display_get_registry(wl_pDisplay), wl_registry*);
			const wl_registry_listener wl_registryListeners = {
				.global = callback_wayland_registry_handler,
				.global_remove = callback_wayland_registry_remove
			};
			PUSH_TO_CALLSTACKTRACE(wl_registry_add_listener(wl_pRegistry, &wl_registryListeners, nullptr));
			PUSH_TO_CALLSTACKTRACE(wl_display_roundtrip(wl_pDisplay));
			if (wl_pCompositor && wl_pSeat && xdg_pWindowBase) {
				wl_pMouse = PUSH_TO_CALLSTACKTRACE_AND_RETURN(wl_seat_get_pointer(wl_pSeat), wl_pointer*);
				const wl_pointer_listener wl_mouseInputListener = {
					.enter = callback_wayland_mouse_enter,
					.leave = callback_wayland_mouse_leave,
					.motion = callback_wayland_mouse_motion_input,
					.button = callback_wayland_mouse_button_input,
					.axis = callback_wayland_mouse_scroll_input,
					.frame = callback_wayland_mouse_input_frame_finish,
					.axis_source = nullptr,
					.axis_stop = callback_wayland_mouse_scroll_stop_input,
					.axis_discrete = nullptr,
					.axis_value120 = nullptr,
					.axis_relative_direction = nullptr
				};
				PUSH_TO_CALLSTACKTRACE(wl_pointer_add_listener(wl_pMouse, &wl_mouseInputListener, nullptr));
				const xdg_wm_base_listener xdg_listener = {
					.ping = callback_xdg_wm_base_ping
				};
				PUSH_TO_CALLSTACKTRACE(xdg_wm_base_add_listener(xdg_pWindowBase, &xdg_listener, nullptr));
				wl_pSurface = PUSH_TO_CALLSTACKTRACE_AND_RETURN(wl_compositor_create_surface(wl_pCompositor), wl_surface*);
				if (wl_pSurface) {
					/*const wl_surface_listener wl_surfaceListener = {
						.enter = ,
						.leave = ,
						.preferred_buffer_scale = ,
						.preferred_buffer_transform = 
					};
					PUSH_TO_CALLSTACKTRACE(wl_surface_add_listener(wl_pSurface, &wl_surfaceListener, nullptr));*/
					xdg_pSurface = PUSH_TO_CALLSTACKTRACE_AND_RETURN(xdg_wm_base_get_xdg_surface(xdg_pWindowBase, wl_pSurface), xdg_surface*);
					if (xdg_pSurface) {
						xdg_pToplevel = PUSH_TO_CALLSTACKTRACE_AND_RETURN(xdg_surface_get_toplevel(xdg_pSurface), xdg_toplevel*);
						if (xdg_pToplevel) {
							/*const xdg_surface_listener xdg_surfaceListener = {
								.configure = callback_wayland_surface_configure
							};
							PUSH_TO_CALLSTACKTRACE(xdg_surface_add_listener(xdg_pSurface, &xdg_surfaceListener, nullptr));*/
							windowSize[0] = 400;
							windowSize[1] = 300;
							PUSH_TO_CALLSTACKTRACE(xdg_surface_set_window_geometry(xdg_pSurface, 0, 0, windowSize[0], windowSize[1]));
							/*const xdg_toplevel_listener xdg_toplevelListener = {
								.configure = callback_xdg_toplevel_configure,
								.close = callback_xdg_toplevel_close,
								.configure_bounds = callback_xdg_toplevel_configure_bounds,
								.wm_capabilities = callback_xdg_toplevel_wm_capabilities
							};
							PUSH_TO_CALLSTACKTRACE(xdg_toplevel_add_listener(xdg_pToplevel, &xdg_toplevelListener, nullptr));*/
							PUSH_TO_CALLSTACKTRACE(xdg_toplevel_set_title(xdg_pToplevel, pacWindowTitle));
							PUSH_TO_CALLSTACKTRACE(xdg_toplevel_set_min_size(xdg_pToplevel, MIN_WINDOW_WIDTH, MIN_WINDOW_HEIGHT));
							PUSH_TO_CALLSTACKTRACE(register_next_ready_frame_callback());
							return true;
						} else
							RE_FATAL_ERROR("Failed to get toplevel of the XDG window for Wayland");
						PUSH_TO_CALLSTACKTRACE(xdg_surface_destroy(xdg_pSurface));
						xdg_pSurface = nullptr;
					} else
						RE_FATAL_ERROR("Failed to create an XDG window for Wayland");
					PUSH_TO_CALLSTACKTRACE(wl_surface_destroy(wl_pSurface));
					wl_pSurface = nullptr;
				} else
					RE_FATAL_ERROR("Failed to create a Wayland surface");
				PUSH_TO_CALLSTACKTRACE(wl_pointer_destroy(wl_pMouse));
				PUSH_TO_CALLSTACKTRACE(xdg_wm_base_destroy(xdg_pWindowBase));
				PUSH_TO_CALLSTACKTRACE(wl_seat_destroy(wl_pSeat));
				PUSH_TO_CALLSTACKTRACE(wl_compositor_destroy(wl_pCompositor));
				wl_pMouse = nullptr;
				xdg_pWindowBase = nullptr;
				wl_pSeat = nullptr;
				wl_pCompositor = nullptr;
			} else
				RE_FATAL_ERROR("Failed to get essential Wayland objects at startup");
			PUSH_TO_CALLSTACKTRACE(wl_registry_destroy(wl_pRegistry));
			PUSH_TO_CALLSTACKTRACE(wl_display_disconnect(wl_pDisplay));
			wl_pRegistry = nullptr;
			wl_pDisplay = nullptr;
		} else
			RE_FATAL_ERROR("Failed connecting to Wayland server");
		return false;
	}

	void wayland_destroy_window() {
		PUSH_TO_CALLSTACKTRACE(xdg_toplevel_destroy(xdg_pToplevel));
		PUSH_TO_CALLSTACKTRACE(xdg_surface_destroy(xdg_pSurface));
		PUSH_TO_CALLSTACKTRACE(wl_surface_destroy(wl_pSurface));
		PUSH_TO_CALLSTACKTRACE(wl_pointer_destroy(wl_pMouse));
		PUSH_TO_CALLSTACKTRACE(xdg_wm_base_destroy(xdg_pWindowBase));
		PUSH_TO_CALLSTACKTRACE(wl_seat_destroy(wl_pSeat));
		PUSH_TO_CALLSTACKTRACE(wl_compositor_destroy(wl_pCompositor));
		PUSH_TO_CALLSTACKTRACE(wl_registry_destroy(wl_pRegistry));
		PUSH_TO_CALLSTACKTRACE(wl_display_disconnect(wl_pDisplay));
		xdg_pToplevel = nullptr;
		xdg_pSurface = nullptr;
		wl_pSurface = nullptr;
		wl_pMouse = nullptr;
		xdg_pWindowBase = nullptr;
		wl_pSeat = nullptr;
		wl_pCompositor = nullptr;
		wl_pRegistry = nullptr;
		wl_pDisplay = nullptr;
	}
	
	void wayland_show_window() {
		PUSH_TO_CALLSTACKTRACE(commit_wayland_surface());
	}
	
	void wayland_update_window_title() {
		PUSH_TO_CALLSTACKTRACE(xdg_toplevel_set_title(xdg_pToplevel, pacWindowTitle));
		PUSH_TO_CALLSTACKTRACE(commit_wayland_surface());
	}

	void wayland_window_proc() {
		int32_t i32Count;
		while ((i32Count = PUSH_TO_CALLSTACKTRACE_AND_RETURN(wl_display_dispatch_pending(wl_pDisplay), int32_t)) > 0) {
			PRINT_LN(i32Count);
		}
		if (are_bits_true<uint8_t>(u8WindowFlagBits, WINDOW_WAYLAND_SHOULD_RENDER_FRAME_BIT))
			PUSH_TO_CALLSTACKTRACE(wl_surface_damage(wl_pSurface, 0, 0, windowSize[0], windowSize[1]));
	}

	void wayland_post_rendering_window_proc() {
		PUSH_TO_CALLSTACKTRACE(commit_wayland_surface());
		set_bits<uint8_t>(u8WindowFlagBits, false, WINDOW_WAYLAND_SHOULD_RENDER_FRAME_BIT);
	}

}

#endif /* RE_OS_LINUX */
