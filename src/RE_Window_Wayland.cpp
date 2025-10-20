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
		PRINT_DEBUG("Commiting Wayland surface ", wl_pSurface, " and flushing to server ", wl_pDisplay);
		wl_surface_commit(wl_pSurface);
		wl_display_flush(wl_pDisplay);
	}

	static void callback_xdg_wm_base_ping(void *const pData, xdg_wm_base *const xdg_pBase, const uint32_t u32Serial) {
		PRINT_DEBUG("Received PING-message for XDG base ", xdg_pBase, ". Responding with PONG");
		xdg_wm_base_pong(xdg_pBase, u32Serial);
	}

	static void callback_wayland_registry_handler(void *const pData, wl_registry *const wl_pRegistryParam, const uint32_t u32Name, const char * const pacInterface, const uint32_t u32Version) {
		PRINT_DEBUG("Callback for Wayland registry ", wl_pRegistryParam, " received and checking resource out. Its integral name is ", std::hex, u32Name, ", interface is called \"", pacInterface, "\" and its version is ", u32Version);
		if (are_string_contents_equal(pacInterface, wl_compositor_interface.name)) {
			PRINT_DEBUG("Got data for Wayland compositor and binding it to registry");
			wl_pCompositor = static_cast<wl_compositor*>(wl_registry_bind(wl_pRegistryParam, u32Name, &wl_compositor_interface, u32Version));
		} else if (are_string_contents_equal(pacInterface, wl_seat_interface.name)) {
			PRINT_DEBUG("Got data for Wayland seat and binding it to registry");
			wl_pSeat = static_cast<wl_seat*>(wl_registry_bind(wl_pRegistryParam, u32Name, &wl_seat_interface, u32Version));
		} else if (are_string_contents_equal(pacInterface, xdg_wm_base_interface.name)) {
			PRINT_DEBUG("Got data for XDG window base and binding it to registry");
			xdg_pWindowBase = static_cast<xdg_wm_base*>(wl_registry_bind(wl_pRegistryParam, u32Name, &xdg_wm_base_interface, u32Version));
		}
	}
	static void callback_wayland_registry_remove(void *const pData, wl_registry *const wl_pRegistryParam, const uint32_t u32Name) {
		PRINT_DEBUG("Callback for removing Wayland object from registry ", wl_pRegistryParam, ". Its integral name is ", std::hex, u32Name);
	}

	static void callback_wayland_mouse_enter(void *const pData, wl_pointer *const wl_pMouseParam, const uint32_t u32Serial, wl_surface *const wl_pSurfaceParam, const wl_fixed_t wl_cursorX, const wl_fixed_t wl_cursorY) {
		if (wl_pSurfaceParam != wl_pSurface)
			return;
		PRINT_DEBUG("Received message for mouse ", wl_pMouseParam, " entering Wayland surface ", wl_pSurfaceParam);
		xdg_toplevel_show_window_menu(xdg_pToplevel, wl_pSeat, u32Serial, 0, 0);
	}
	static void callback_wayland_mouse_leave(void *const pData, wl_pointer *const wl_pMouseParam, const uint32_t u32Serial, wl_surface *const wl_pSurfaceParam) {
		PRINT_DEBUG("Received message for mouse ", wl_pMouseParam, " leaving Wayland surface ", wl_pSurfaceParam);
	}
	static void callback_wayland_mouse_motion_input(void *const pData, wl_pointer *const wl_pMouseParam, const uint32_t u32Time, const wl_fixed_t wl_cursorX, const wl_fixed_t wl_cursorY) {
		PRINT_DEBUG("Mouse ", wl_pMouseParam, " has been moved");
	}
	static void callback_wayland_mouse_button_input(void *const pData, wl_pointer *const wl_pMouseParam, const uint32_t u32Serial, const uint32_t u32Time, const uint32_t u32Button, const uint32_t u32State) {
		PRINT_DEBUG("Received input-event for mouse button ", std::hex, u32Button, " of mouse ", wl_pMouseParam, ". Its new state is ", u32State, " and time parameter is ", u32Time);
	}
	static void callback_wayland_mouse_scroll_input(void *const pData, wl_pointer *const wl_pMouseParam, const uint32_t u32Time, const uint32_t u32Axis, const wl_fixed_t wl_value) {
		PRINT_DEBUG("Received scroll-event of mouse ", wl_pMouseParam, ". Its axis is ", std::hex, u32Axis, ", scroll-value is ", std::dec, wl_value, " and its time parameter is ", std::hex, u32Time);
	}
	static void callback_wayland_mouse_input_frame_finish(void *const pData, wl_pointer *const wl_pMouseParam) {
		PRINT_DEBUG("Frame finish-callback for mouse ", wl_pMouseParam, " has been called");
	}
	static void callback_wayland_mouse_scroll_stop_input(void *const pData, wl_pointer *const wl_pMouseParam, const uint32_t u32Time, const uint32_t u32Axis) {
		PRINT_DEBUG("Scrolling of mouse ", wl_pMouseParam, " has been stopped. Its axis is ", std::hex, u32Axis, " and time parameter is ", u32Time);
	}

	static void callback_wayland_surface_configure(void *const pData, xdg_surface *const xdg_pSurface, const uint32_t u32Serial) {
		PRINT_DEBUG("XDG surface ", xdg_pSurface, " has been configured. Acknowledging it without checking its status");
		xdg_surface_ack_configure(xdg_pSurface, u32Serial);
	}

	static void register_next_ready_frame_callback();
	static void callback_wayland_surface_frame_ready(void *const pData, wl_callback *const wl_pCallback, const uint32_t u32Serial) {
		PRINT_DEBUG("Wayland is ready for receiving next frame to display. Destroying old callback ", wl_pCallback, " and recreating it");
		wl_callback_destroy(wl_pCallback);
		set_bits<uint8_t>(u8WindowFlagBits, true, WINDOW_WAYLAND_SHOULD_RENDER_FRAME_BIT);
		register_next_ready_frame_callback();
	}
	static void register_next_ready_frame_callback() {
		PRINT_DEBUG("Registering callback for receiving readyness for next frame for Wayland surface ", wl_pSurface);
		wl_callback *const wl_pFrameReadyCallback = wl_surface_frame(wl_pSurface);
		const wl_callback_listener wl_frameReadyCallback = {
			.done = callback_wayland_surface_frame_ready
		};
		wl_callback_add_listener(wl_pFrameReadyCallback, &wl_frameReadyCallback, nullptr);
		commit_wayland_surface();
	}

	static void callback_xdg_toplevel_configure(void *const pData, xdg_toplevel *const xdg_pToplevelParam, const int32_t i32Width, const int32_t i32Height, wl_array *const wl_pArray) {
		PRINT_DEBUG("XDG toplevel ", xdg_pToplevelParam, " has been configured. Its size is ", i32Width, "x", i32Height, " and received array ", wl_pArray);
	}
	static void callback_xdg_toplevel_close(void *const pData, xdg_toplevel *const xdg_pToplevelParam) {
		PRINT_DEBUG("XDG toplevel ", xdg_pToplevelParam, " has been requested to close");
		set_bits<uint8_t>(u8WindowFlagBits, WINDOW_CLOSE_FLAG_BIT, true);
	}
	static void callback_xdg_toplevel_configure_bounds(void *const pData, xdg_toplevel *const xdg_pToplevelParam, const int32_t i32Width, const int32_t i32Height) {
		PRINT_DEBUG("The boundaries of XDG toplevel ", xdg_pToplevelParam, " have been configured. Now it's ", i32Width, "x", i32Height);
	}
	static void callback_xdg_toplevel_wm_capabilities(void *const pData, xdg_toplevel *const xdg_pToplevelParam, wl_array *const wl_pArray) {
		PRINT_DEBUG("Callback for WM-capabilities of XDG toplevel ", xdg_pToplevelParam, " has been called and received array ", wl_pArray);
	}

	bool wayland_create_window() {
		PRINT_DEBUG("Connecting to Wayland server");
		wl_pDisplay = wl_display_connect(nullptr);
		if (wl_pDisplay) {
			PRINT_DEBUG("Getting Wayland registry from display ", wl_pDisplay);
			wl_pRegistry = wl_display_get_registry(wl_pDisplay);
			PRINT_DEBUG("Adding callback for binding essential resources to Wayland registry ", wl_pRegistry);
			const wl_registry_listener wl_registryListeners = {
				.global = callback_wayland_registry_handler,
				.global_remove = callback_wayland_registry_remove
			};
			wl_registry_add_listener(wl_pRegistry, &wl_registryListeners, nullptr);
			PRINT_DEBUG("Triggering roundtrip in display ", wl_pDisplay);
			wl_display_roundtrip(wl_pDisplay);
			if (wl_pCompositor && wl_pSeat && xdg_pWindowBase) {
				PRINT_DEBUG("Querying mouse of Wayland seat ", wl_pSeat);
				wl_pMouse = wl_seat_get_pointer(wl_pSeat);
				PRINT_DEBUG("Adding callbacks for Wayland mouse ", wl_pMouse);
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
					.axis_value120 = nullptr
				};
				wl_pointer_add_listener(wl_pMouse, &wl_mouseInputListener, nullptr);
				PRINT_DEBUG("Adding callbacks for XDG WM-base ", xdg_pWindowBase);
				const xdg_wm_base_listener xdg_listener = {
					.ping = callback_xdg_wm_base_ping
				};
				xdg_wm_base_add_listener(xdg_pWindowBase, &xdg_listener, nullptr);
				PRINT_DEBUG("Creating Wayland surface in compositor ", wl_pCompositor);
				wl_pSurface = wl_compositor_create_surface(wl_pCompositor);
				if (wl_pSurface) {
					/*PRINT_DEBUG("Adding callbacks for Wayland surface ", wl_pSurface);
					const wl_surface_listener wl_surfaceListener = {
						.enter = ,
						.leave = ,
						.preferred_buffer_scale = ,
						.preferred_buffer_transform = 
					};
					wl_surface_add_listener(wl_pSurface, &wl_surfaceListener, nullptr);*/
					PRINT_DEBUG("Getting XDG surface of XDG WM-base ", xdg_pWindowBase);
					xdg_pSurface = xdg_wm_base_get_xdg_surface(xdg_pWindowBase, wl_pSurface);
					if (xdg_pSurface) {
						PRINT_DEBUG("Getting XDG toplevel of XDG surface ", xdg_pSurface);
						xdg_pToplevel = xdg_surface_get_toplevel(xdg_pSurface);
						if (xdg_pToplevel) {
							/*PRINT_DEBUG("Adding callbacks for XDG surface ", xdg_pSurface);
							const xdg_surface_listener xdg_surfaceListener = {
								.configure = callback_wayland_surface_configure
							};
							xdg_surface_add_listener(xdg_pSurface, &xdg_surfaceListener, nullptr);*/
							PRINT_DEBUG("Setting size of XDG surface ", wl_pSurface);
							windowSize[0] = 400;
							windowSize[1] = 300;
							xdg_surface_set_window_geometry(xdg_pSurface, 0, 0, windowSize[0], windowSize[1]);
							/*PRINT_DEBUG("Adding callbacks for XDG toplevel ", xdg_pToplevel);
							const xdg_toplevel_listener xdg_toplevelListener = {
								.configure = callback_xdg_toplevel_configure,
								.close = callback_xdg_toplevel_close,
								.configure_bounds = callback_xdg_toplevel_configure_bounds,
								.wm_capabilities = callback_xdg_toplevel_wm_capabilities
							};
							xdg_toplevel_add_listener(xdg_pToplevel, &xdg_toplevelListener, nullptr);*/
							PRINT_DEBUG("Setting title of XDG toplevel ", xdg_pToplevel);
							xdg_toplevel_set_title(xdg_pToplevel, pacWindowTitle);
							PRINT_DEBUG("Setting minimum allowed size of XDG toplevel ", xdg_pToplevel);
							xdg_toplevel_set_min_size(xdg_pToplevel, MIN_WINDOW_WIDTH, MIN_WINDOW_HEIGHT);
							register_next_ready_frame_callback();
							return true;
						} else
							RE_FATAL_ERROR("Failed to get toplevel of the XDG window for Wayland");
						PRINT_DEBUG("Destroying XDG surface ", xdg_pSurface, " due to failure creating or registering essentail resources");
						xdg_surface_destroy(xdg_pSurface);
						xdg_pSurface = nullptr;
					} else
						RE_FATAL_ERROR("Failed to create an XDG window for Wayland");
					PRINT_DEBUG("Destroying Wayland surface ", wl_pSurface, " due to failure creating or registering essentail resources");
					wl_surface_destroy(wl_pSurface);
					wl_pSurface = nullptr;
				} else
					RE_FATAL_ERROR("Failed to create a Wayland surface");
				PRINT_DEBUG("Destroying Wayland mouse ", wl_pMouse, " due to failure creating or registering essentail resources");
				wl_pointer_destroy(wl_pMouse);
				PRINT_DEBUG("Destroying XDG WM-base ", xdg_pWindowBase, " due to failure creating or registering essentail resources");
				xdg_wm_base_destroy(xdg_pWindowBase);
				PRINT_DEBUG("Destroying Wayland seat ", wl_pSeat, " due to failure creating or registering essentail resources");
				wl_seat_destroy(wl_pSeat);
				PRINT_DEBUG("Destroying XDG surface ", wl_pCompositor, " due to failure creating or registering essentail resources");
				wl_compositor_destroy(wl_pCompositor);
				wl_pMouse = nullptr;
				xdg_pWindowBase = nullptr;
				wl_pSeat = nullptr;
				wl_pCompositor = nullptr;
			} else
				RE_FATAL_ERROR("Failed to get essential Wayland objects at startup");
			PRINT_DEBUG("Destroying Wayland registry ", wl_pRegistry, " due to failure creating or registering essentail resources");
			wl_registry_destroy(wl_pRegistry);
			PRINT_DEBUG("Disconnecting Wayland display ", wl_pDisplay, " due to failure creating or registering essentail resources");
			wl_display_disconnect(wl_pDisplay);
			wl_pRegistry = nullptr;
			wl_pDisplay = nullptr;
		} else
			RE_FATAL_ERROR("Failed connecting to Wayland server");
		return false;
	}

	void wayland_destroy_window() {
		PRINT_DEBUG("Destroying XDG toplevel ", xdg_pToplevel);
		xdg_toplevel_destroy(xdg_pToplevel);
		PRINT_DEBUG("Destroying XDG surface ", xdg_pSurface);
		xdg_surface_destroy(xdg_pSurface);
		PRINT_DEBUG("Destroying Wayland surface ", wl_pSurface);
		wl_surface_destroy(wl_pSurface);
		PRINT_DEBUG("Destroying Wayland mouse ", wl_pMouse);
		wl_pointer_destroy(wl_pMouse);
		PRINT_DEBUG("Destroying XDG WM-base ", xdg_pWindowBase);
		xdg_wm_base_destroy(xdg_pWindowBase);
		PRINT_DEBUG("Destroying Wayland seat ", wl_pSeat);
		wl_seat_destroy(wl_pSeat);
		PRINT_DEBUG("Destroying XDG surface ", wl_pCompositor);
		wl_compositor_destroy(wl_pCompositor);
		PRINT_DEBUG("Destroying Wayland registry ", wl_pRegistry);
		wl_registry_destroy(wl_pRegistry);
		PRINT_DEBUG("Disconnecting Wayland display ", wl_pDisplay);
		wl_display_disconnect(wl_pDisplay);
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
		PRINT_DEBUG("Showing/Hiding Wayland surface ", wl_pSurface);
		commit_wayland_surface();
	}
	
	void wayland_update_window_title() {
		PRINT_DEBUG("Updating title of XDG toplevel ", xdg_pToplevel);
		xdg_toplevel_set_title(xdg_pToplevel, pacWindowTitle);
		commit_wayland_surface();
	}

	void wayland_window_proc() {
		PRINT_DEBUG("Calling procedure for Wayland display ", wl_pDisplay);
		int32_t i32Count = wl_display_dispatch_pending(wl_pDisplay);
		while (i32Count) {
			i32Count--;
			PRINT_DEBUG("Wayland event received. Count of remaining events in queue: ", i32Count);
		}
		if (are_bits_true<uint8_t>(u8WindowFlagBits, WINDOW_WAYLAND_SHOULD_RENDER_FRAME_BIT)) {
			PRINT_DEBUG("Marking whole window area being damaged/dirty and needing to render");
			wl_surface_damage(wl_pSurface, 0, 0, windowSize[0], windowSize[1]);
		}
	}

	void wayland_post_rendering_window_proc() {
		PRINT_DEBUG("Commiting Wayland surface to show new frame after being rendered");
		commit_wayland_surface();
		set_bits<uint8_t>(u8WindowFlagBits, false, WINDOW_WAYLAND_SHOULD_RENDER_FRAME_BIT);
	}

}

#endif /* RE_OS_LINUX */
