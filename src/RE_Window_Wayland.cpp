#include "RE_Window.hpp"
#include "RE_Window_Wayland.hpp"
#include "RE_Renderer.hpp"

#ifdef RE_OS_LINUX

namespace RE {

#define REQUIRED_VERSION_WL_COMPOSITOR WL_COMPOSITOR_CREATE_SURFACE_SINCE_VERSION
#define REQUIRED_VERSION_XDG_WM_BASE std::max(XDG_WM_BASE_PING_SINCE_VERSION, std::max(XDG_WM_BASE_DESTROY_SINCE_VERSION, std::max(XDG_WM_BASE_GET_XDG_SURFACE_SINCE_VERSION, XDG_WM_BASE_PONG_SINCE_VERSION)))
#define REQUIRED_VERSION_WL_SEAT std::max(WL_SEAT_CAPABILITIES_SINCE_VERSION, std::max(WL_SEAT_GET_POINTER_SINCE_VERSION, std::max(WL_SEAT_GET_KEYBOARD_SINCE_VERSION, WL_SEAT_RELEASE_SINCE_VERSION)))

	wl_display *wl_pDisplay;
	static wl_registry *wl_pRegistry;
	static wl_compositor *wl_pCompositor = nullptr;
	static xdg_wm_base *xdg_pWmBase = nullptr;
	static wl_seat *wl_pSeat = nullptr;
	wl_surface *wl_pSurface;
	static xdg_surface *xdg_pSurface;
	static xdg_toplevel *xdg_pToplevel;

	static void xdg_wm_base_ping(void *const pData, xdg_wm_base *const xdg_pWmBase, const uint32_t u32Serial) {
		xdg_wm_base_pong(xdg_pWmBase, u32Serial);
	}

	static void wayland_registry_add_callback(void *const pData, wl_registry *const wl_pRegistry, const uint32_t u32Name, const char *const pacInterface, const uint32_t u32Version) {
		if (are_string_contents_equal(pacInterface, wl_compositor_interface.name)) {
			if (u32Version >= REQUIRED_VERSION_WL_COMPOSITOR) {
				PRINT_DEBUG("Binding the compositor");
				wl_pCompositor = reinterpret_cast<wl_compositor*>(
						wl_registry_bind(
							wl_pRegistry,
							u32Name,
							&wl_compositor_interface,
							REQUIRED_VERSION_WL_COMPOSITOR));
			} else
				RE_ERROR("The version of compositor is too low (", u32Version, " < ", REQUIRED_VERSION_WL_COMPOSITOR, ")");
		} else if (are_string_contents_equal(pacInterface, xdg_wm_base_interface.name)) {
			if (u32Version >= static_cast<uint32_t>(REQUIRED_VERSION_XDG_WM_BASE)) {
				PRINT_DEBUG("Binding the XDG wm base");
				xdg_pWmBase = reinterpret_cast<xdg_wm_base*>(
						wl_registry_bind(wl_pRegistry,
							u32Name,
							&xdg_wm_base_interface,
							REQUIRED_VERSION_XDG_WM_BASE));
			} else
				RE_ERROR("The version of XDG wm base is too low (", u32Version, " < ", REQUIRED_VERSION_XDG_WM_BASE, ")");
		} else if (are_string_contents_equal(pacInterface, wl_seat_interface.name)) {
			if (u32Version >= static_cast<uint32_t>(REQUIRED_VERSION_WL_SEAT)) {
				PRINT_DEBUG("Binding the seat");
				wl_pSeat = reinterpret_cast<wl_seat*>(
						wl_registry_bind(wl_pRegistry,
							u32Name,
							&wl_seat_interface,
							REQUIRED_VERSION_WL_SEAT));
			} else
				RE_ERROR("The version of seat is too low (", u32Version, " < ", REQUIRED_VERSION_WL_SEAT, ")");
		}
	}

	static void wayland_registry_remove_callback(void *const pData, wl_registry *const wl_pRegistry, const uint32_t u32Name) {
	}

	static void xdg_surface_configure_callback(void *const pData, xdg_surface *const xdg_pSurface, const uint32_t u32Serial) {
		xdg_surface_ack_configure(xdg_pSurface, u32Serial);
		wl_display_flush(wl_pDisplay);
		std::this_thread::sleep_for(std::chrono::duration<int, std::milli>(10));
	}

	static void xdg_toplevel_configure_callback(void *const pData, xdg_toplevel *const xdg_pToplevel, const int32_t i32Width, const int32_t i32Height, wl_array *const wl_pStates) {
		if (i32Width <= 0 || i32Height <= 0)
			return;
		window_resize_event(static_cast<uint32_t>(i32Width), static_cast<uint32_t>(i32Height));
	}

	static void xdg_toplevel_close_callback(void *const pData, xdg_toplevel *const xdg_pToplevel) {
		set_bits<uint8_t>(u8WindowFlagBits, true, WINDOW_CLOSE_FLAG_BIT);
	}

	static constexpr wl_registry_listener wl_registryListener = {
		.global = wayland_registry_add_callback,
		.global_remove = wayland_registry_remove_callback
	};

	static constexpr xdg_wm_base_listener xdg_wmBasePinger = {
		.ping = xdg_wm_base_ping
	};

	static constexpr xdg_surface_listener xdg_surfaceListener = {
		.configure = xdg_surface_configure_callback
	};

	static constexpr xdg_toplevel_listener xdg_toplevelListener = {
		.configure = xdg_toplevel_configure_callback,
		.close = xdg_toplevel_close_callback,
		.configure_bounds = nullptr,
		.wm_capabilities = nullptr
	};

	bool wayland_create_window() {
		PRINT_DEBUG("Connecting to the Wayland server");
		if ((wl_pDisplay = wl_display_connect(nullptr))) {
			PRINT_DEBUG("Retrieving the registry from the Wayland connection ", wl_pDisplay);
			if ((wl_pRegistry = wl_display_get_registry(wl_pDisplay))) {
				PRINT_DEBUG("Adding a listener to the registry ", wl_pRegistry, " in Wayland");
				if (wl_registry_add_listener(wl_pRegistry, &wl_registryListener, nullptr) == 0) {
					PRINT_DEBUG("Sending request to the Wayland server to call listeners");
					wl_display_roundtrip(wl_pDisplay);
					if (wl_pCompositor && xdg_pWmBase && wl_pSeat) {
						PRINT_DEBUG("Adding a listener the XDG wm base ", xdg_pWmBase);
						if (xdg_wm_base_add_listener(xdg_pWmBase, &xdg_wmBasePinger, nullptr) == 0) {
							PRINT_DEBUG("Creating surface from the compositor ", wl_pCompositor, " in Wayland");
							if ((wl_pSurface = wl_compositor_create_surface(wl_pCompositor))) {
								PRINT_DEBUG("Retrieving XDG surface from XDG wm base ", xdg_pWmBase, " and Wayland surface ", wl_pSurface);
								if ((xdg_pSurface = xdg_wm_base_get_xdg_surface(xdg_pWmBase, wl_pSurface))) {
									PRINT_DEBUG("Adding a listener to the XDG surface");
									if (xdg_surface_add_listener(xdg_pSurface, &xdg_surfaceListener, nullptr) == 0) {
										PRINT_DEBUG("Retrieving XDG toplevel from surface ", xdg_pSurface);
										if ((xdg_pToplevel = xdg_surface_get_toplevel(xdg_pSurface))) {
											PRINT_DEBUG("Adding listener to the XDG toplevel ", xdg_pToplevel);
											if (xdg_toplevel_add_listener(xdg_pToplevel, &xdg_toplevelListener, nullptr) == 0) {
												PRINT_DEBUG("Setting title of the XDG toplevel ", xdg_pToplevel);
												xdg_toplevel_set_min_size(xdg_pToplevel, MIN_WINDOW_WIDTH, MIN_WINDOW_HEIGHT);
												xdg_toplevel_set_title(xdg_pToplevel, pacWindowTitle);
												xdg_toplevel_set_app_id(xdg_pToplevel, pacWindowTitle);
												wl_surface_commit(wl_pSurface);
												wl_display_flush(wl_pDisplay);
												return true;
											}
											PRINT_DEBUG("Destroying XDG toplevel ", xdg_pToplevel, " due to failure creating the Wayland window");
											xdg_toplevel_destroy(xdg_pToplevel);
										} else
											RE_ERROR("Failed to retrieve toplevel of XDG surface ", xdg_pSurface);
									} else
										RE_ERROR("Failed to add a listener to the XDG surface ", xdg_pSurface);
									PRINT_DEBUG("Destroying XDG surface ", xdg_pToplevel, " due to failure creating the Wayland window");
									xdg_surface_destroy(xdg_pSurface);
								} else
									RE_ERROR("Failed to retrieve surface of XDG wm base ", xdg_pWmBase, " and Wayland surface ", wl_pSurface);
								PRINT_DEBUG("Destroying surface ", wl_pSurface, " due to failure creating the Wayland window");
								wl_surface_destroy(wl_pSurface);
							} else
								RE_ERROR("Failed to create a Wayland surface with compositor ", wl_pCompositor);
						} else
							RE_ERROR("Failed to add a listener to the XDG wm base ", xdg_pWmBase);
					} else
						RE_ERROR("Failed to get essential Wayland ressources or to initialize them");
					if (wl_pSeat) {
						PRINT_DEBUG("Destroying seat ", wl_pSeat, " due to failure creating the Wayland window");
						wl_seat_destroy(wl_pSeat);
						wl_pSeat = nullptr;
					}
					if (xdg_pWmBase) {
						PRINT_DEBUG("Destroying XDG wm base ", xdg_pWmBase, " due to failure creating the Wayland window");
						xdg_wm_base_destroy(xdg_pWmBase);
						xdg_pWmBase = nullptr;
					}
					if (wl_pCompositor) {
						PRINT_DEBUG("Destroying compositor ", wl_pCompositor, " due to failure creating the Wayland window");
						wl_compositor_destroy(wl_pCompositor);
						wl_pCompositor = nullptr;
					}
				} else
					RE_ERROR("Failed to add a listener to the Wayland registry ", wl_pRegistry);
				PRINT_DEBUG("Destroying registry ", wl_pRegistry, " due to failure creating the Wayland window");
				wl_registry_destroy(wl_pRegistry);
			} else
				RE_ERROR("Failed to retrieve a registry from the connection to the Wayland display ", wl_pDisplay);
			PRINT_DEBUG("Disconnecting from the Wayland server due to failure creating the window");
			wl_display_disconnect(wl_pDisplay);
		} else
			RE_ERROR("Failed to connect to the Wayland server");
		return false;
	}

	void wayland_destroy_window() {
		PRINT_DEBUG("Destroying the Wayland window ressources");
		xdg_toplevel_destroy(xdg_pToplevel);
		xdg_surface_destroy(xdg_pSurface);
		wl_surface_destroy(wl_pSurface);
		wl_seat_destroy(wl_pSeat);
		xdg_wm_base_destroy(xdg_pWmBase);
		wl_compositor_destroy(wl_pCompositor);
		wl_registry_destroy(wl_pRegistry);
		wl_display_disconnect(wl_pDisplay);
		wl_pSeat = nullptr;
		xdg_pWmBase = nullptr;
		wl_pCompositor = nullptr;
	}
	
	void wayland_show_window() {
	}

	void wayland_update_fullscreen() {
		if (is_fullscreen()) {
		} else {
		}
	}
	
	void wayland_update_window_title() {
		PRINT_DEBUG("Updating title and app-id of the wayland window");
		xdg_toplevel_set_title(xdg_pToplevel, pacWindowTitle);
		xdg_toplevel_set_app_id(xdg_pToplevel, pacWindowTitle);
		wl_display_flush(wl_pDisplay);
	}

	void wayland_window_proc() {
		PRINT_DEBUG("Calling Wayland window procedure");
		wl_display_dispatch_pending(wl_pDisplay);
		wl_display_flush(wl_pDisplay);
	}

}

#endif /* RE_OS_LINUX */
