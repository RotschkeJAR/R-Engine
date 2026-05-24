#include "RE_Window.hpp"
#include "RE_Window_Wayland.hpp"
#include "RE_Renderer.hpp"

#ifdef RE_OS_LINUX

#include <dlfcn.h>
#include <linux/input-event-codes.h>

namespace RE {

#define REQUIRED_VERSION_WL_COMPOSITOR static_cast<uint32_t>(std::max(WL_COMPOSITOR_CREATE_SURFACE_SINCE_VERSION, WL_COMPOSITOR_CREATE_REGION_SINCE_VERSION))
#define REQUIRED_VERSION_XDG_WM_BASE static_cast<uint32_t>(std::max(XDG_WM_BASE_PING_SINCE_VERSION, std::max(XDG_WM_BASE_DESTROY_SINCE_VERSION, std::max(XDG_WM_BASE_GET_XDG_SURFACE_SINCE_VERSION, XDG_WM_BASE_PONG_SINCE_VERSION))))
#define REQUIRED_VERSION_WL_SEAT static_cast<uint32_t>(std::max(WL_SEAT_CAPABILITIES_SINCE_VERSION, std::max(WL_SEAT_GET_POINTER_SINCE_VERSION, WL_SEAT_GET_KEYBOARD_SINCE_VERSION)))
#define REQUIRED_VERSION_WL_OUTPUT static_cast<uint32_t>(std::max(WL_OUTPUT_GEOMETRY_SINCE_VERSION, WL_OUTPUT_MODE_SINCE_VERSION))

	template <class WaylandObject>
	struct GlobalWaylandObject final {
		WaylandObject waylandObject;
		uint32_t u32Name,
			u32Version;
	};
	typedef GlobalWaylandObject<wl_compositor*> WlCompositor;
	typedef GlobalWaylandObject<xdg_wm_base*> XdgWmBase;
	typedef GlobalWaylandObject<wl_seat*> WlSeat;
	typedef GlobalWaylandObject<wl_output*> WlOutput;

	wl_display *wl_pDisplay;
	static wl_registry *wl_pRegistry;
	static WlCompositor wlCompositor = {};
	static XdgWmBase xdgWmBase = {};
	static WlSeat wlSeat = {};
	wl_surface *wl_pSurface;
	static xdg_surface *xdg_pSurface;
	static xdg_toplevel *xdg_pToplevel;
	static std::vector<WlOutput> waylandMonitors;
	static wl_pointer *wl_pPointer = nullptr;
	static wl_keyboard *wl_pKeyboard = nullptr;

	static void destroy_wayland_pointer() {
		PRINT_DEBUG("Destroying pointer ", wl_pPointer);
		if (wl_pointer_get_version(wl_pPointer) >= static_cast<uint32_t>(WL_POINTER_RELEASE_SINCE_VERSION))
			wl_pointer_release(wl_pPointer);
		else
			wl_pointer_destroy(wl_pPointer);
		wl_pPointer = nullptr;
	}

	static void destroy_wayland_keyboard() {
		PRINT_DEBUG("Destroying keyboard ", wl_pKeyboard);
		if (wl_keyboard_get_version(wl_pKeyboard) >= static_cast<uint32_t>(WL_KEYBOARD_RELEASE_SINCE_VERSION))
			wl_keyboard_release(wl_pKeyboard);
		else
			wl_keyboard_destroy(wl_pKeyboard);
		wl_pKeyboard = nullptr;
	}

	static void wayland_registry_add_callback(void *const pData, wl_registry *const wl_pRegistry, const uint32_t u32Name, const char *const pacInterface, const uint32_t u32Version) {
		if (are_string_contents_equal(pacInterface, wl_compositor_interface.name) && !wlCompositor.waylandObject) {
			if (u32Version >= REQUIRED_VERSION_WL_COMPOSITOR) {
				PRINT_DEBUG("Binding the compositor");
				wlCompositor.waylandObject = reinterpret_cast<wl_compositor*>(
						wl_registry_bind(
							wl_pRegistry,
							u32Name,
							&wl_compositor_interface,
							REQUIRED_VERSION_WL_COMPOSITOR));
				wlCompositor.u32Name = u32Name;
				wlCompositor.u32Version = u32Version;
			} else
				RE_ERROR("The version of compositor is too low (", u32Version, " < ", REQUIRED_VERSION_WL_COMPOSITOR, ")");
		} else if (are_string_contents_equal(pacInterface, xdg_wm_base_interface.name) && !xdgWmBase.waylandObject) {
			if (u32Version >= REQUIRED_VERSION_XDG_WM_BASE) {
				PRINT_DEBUG("Binding the XDG wm base");
				xdgWmBase.waylandObject = reinterpret_cast<xdg_wm_base*>(
						wl_registry_bind(
							wl_pRegistry,
							u32Name,
							&xdg_wm_base_interface,
							REQUIRED_VERSION_XDG_WM_BASE));
				xdgWmBase.u32Name = u32Name;
				xdgWmBase.u32Version = u32Version;
			} else
				RE_ERROR("The version of XDG wm base is too low (", u32Version, " < ", REQUIRED_VERSION_XDG_WM_BASE, ")");
		} else if (are_string_contents_equal(pacInterface, wl_seat_interface.name) && !wlSeat.waylandObject) {
			if (u32Version >= REQUIRED_VERSION_WL_SEAT) {
				PRINT_DEBUG("Binding the seat");
				wlSeat.waylandObject = reinterpret_cast<wl_seat*>(
						wl_registry_bind(
							wl_pRegistry,
							u32Name,
							&wl_seat_interface,
							REQUIRED_VERSION_WL_SEAT));
				wlSeat.u32Name = u32Name;
				wlSeat.u32Version = u32Version;
			} else
				RE_ERROR("The version of seat is too low (", u32Version, " < ", REQUIRED_VERSION_WL_SEAT, ")");
		} else if (are_string_contents_equal(pacInterface, wl_output_interface.name)) {
			if (u32Version >= REQUIRED_VERSION_WL_OUTPUT) {
				PRINT_DEBUG("Binding a monitor");
				waylandMonitors.emplace_back(
						reinterpret_cast<wl_output*>(
							wl_registry_bind(
								wl_pRegistry,
								u32Name,
								&wl_output_interface,
								REQUIRED_VERSION_WL_OUTPUT)),
						u32Name,
						u32Version);
			} else
				RE_ERROR("The version of output is too low (", u32Version, " < ", REQUIRED_VERSION_WL_OUTPUT, ")");
		}
	}

	static void wayland_registry_remove_callback(void *const pData, wl_registry *const wl_pRegistry, const uint32_t u32Name) {
		for (auto it = waylandMonitors.begin(); it != waylandMonitors.end(); it++) {
			if (it->u32Name != u32Name)
				continue;
			PRINT_DEBUG("Removing the Wayland output object ", it->waylandObject);
			if (it->u32Version >= static_cast<uint32_t>(WL_OUTPUT_RELEASE_SINCE_VERSION))
				wl_output_release(it->waylandObject);
			else
				wl_output_destroy(it->waylandObject);
			waylandMonitors.erase(it);
			return;
		}
		if (wlCompositor.u32Name == u32Name)
			RE_ABORT("Lost the Wayland compositor");
		else if (xdgWmBase.u32Name == u32Name)
			RE_ABORT("Lost the XDG wm base");
		else if (wlSeat.u32Name == u32Name)
			RE_ABORT("Lost the Wayland seat");
	}

	static constexpr wl_registry_listener wl_registryListener = {
		.global = wayland_registry_add_callback,
		.global_remove = wayland_registry_remove_callback
	};

	static void xdg_wm_base_ping(void *const pData, xdg_wm_base *const xdg_pWmBase, const uint32_t u32Serial) {
		xdg_wm_base_pong(xdg_pWmBase, u32Serial);
	}

	static constexpr xdg_wm_base_listener xdg_wmBasePinger = {
		.ping = xdg_wm_base_ping
	};

	static void xdg_surface_configure_callback(void *const pData, xdg_surface *const xdg_pSurface, const uint32_t u32Serial) {
		xdg_surface_ack_configure(xdg_pSurface, u32Serial);
		wl_display_flush(wl_pDisplay);
	}

	static constexpr xdg_surface_listener xdg_surfaceListener = {
		.configure = xdg_surface_configure_callback
	};

	static void xdg_toplevel_configure_callback(void *const pData, xdg_toplevel *const xdg_pToplevel, int32_t i32Width, int32_t i32Height, wl_array *const wl_pStates) {
		if (i32Width <= 0)
			i32Width = 600;
		if (i32Height <= 0)
			i32Height = 400;
		window_resize_event(static_cast<uint32_t>(i32Width), static_cast<uint32_t>(i32Height));
		PRINT_DEBUG("Updating regions on Wayland surface ", wl_pSurface);
		wl_region *const wl_pRegion = wl_compositor_create_region(wlCompositor.waylandObject);
		wl_region_add(wl_pRegion, 0, 0, i32Width, i32Height);
		wl_surface_set_opaque_region(wl_pSurface, wl_pRegion);
		wl_region_destroy(wl_pRegion);
	}

	static void xdg_toplevel_close_callback(void *const pData, xdg_toplevel *const xdg_pToplevel) {
		set_bits<uint8_t>(u8WindowFlagBits, true, WINDOW_CLOSE_FLAG_BIT);
	}

	static constexpr xdg_toplevel_listener xdg_toplevelListener = {
		.configure = xdg_toplevel_configure_callback,
		.close = xdg_toplevel_close_callback,
		.configure_bounds = nullptr,
		.wm_capabilities = nullptr
	};

	static void wayland_pointer_enter_callback(void *const pData, wl_pointer *const wl_pPointer, const uint32_t u32Serial, wl_surface *const wl_pSurface, const wl_fixed_t wl_x, const wl_fixed_t wl_y) {
	}

	static void wayland_pointer_leave_callback(void *const pData, wl_pointer *const wl_pPointer, const uint32_t u32Serial, wl_surface *const wl_pSurface) {
	}

	static void wayland_pointer_motion_callback(void *const pData, wl_pointer *const wl_pPointer, const uint32_t u32Time, const wl_fixed_t wl_x, const wl_fixed_t wl_y) {
		cursor_event(wl_fixed_to_int(wl_x), wl_fixed_to_int(wl_y));
	}

	static void wayland_pointer_button_callback(void *const pData, wl_pointer *const wl_pPointer, const uint32_t u32Serial, const uint32_t u32Time, const uint32_t u32Button, const uint32_t u32State) {
		if (u32State == WL_POINTER_BUTTON_STATE_PRESSED && u32Button == BTN_LEFT)
			xdg_toplevel_move(xdg_pToplevel, wlSeat.waylandObject, u32Serial);
	}

	static void wayland_keyboard_keymap_callback(void *const pData, wl_keyboard *const wl_pKeyboard, const uint32_t u32Format, const int32_t i32FileDescriptor, const uint32_t u32Size) {
		PRINT_LN("Keyboard format: ", u32Format);
	}

	static void wayland_keyboard_enter_callback(void *const pData, wl_keyboard *const wl_pKeyboard, const uint32_t u32Serial, struct wl_surface *const wl_pSurface, struct wl_array *const wl_pKeys) {
	}

	static void wayland_keyboard_leave_callback(void *const pData, wl_keyboard *const wl_pKeyboard, const uint32_t u32Serial, wl_surface *const wl_pSurface) {
	}

	static void wayland_keyboard_key_callback(void *const pData, wl_keyboard *const wl_pKeyboard, const uint32_t u32Serial, const uint32_t u32Time, const uint32_t u32Key, const uint32_t u32State) {
		input_event(RE_INPUT_UNKNOWN, u32Key, static_cast<bool>(u32State), true);
	}

	static void wayland_keyboard_modifiers_callback(void *const pData, wl_keyboard *const wl_pKeyboard, const uint32_t u32Serial, const uint32_t u32ModsDepressed, const uint32_t u32ModsLatched, const uint32_t u32ModsLocked, const uint32_t u32Group) {
	}

	static void wayland_keyboard_repeat_info_callback(void *const pData, wl_keyboard *const wl_pKeyboard, const int32_t i32Rate, const int32_t i32Delay) {
	}

	static constexpr wl_pointer_listener wl_pointerListener = {
		.enter = wayland_pointer_enter_callback,
		.leave = wayland_pointer_leave_callback,
		.motion = wayland_pointer_motion_callback,
		.button = wayland_pointer_button_callback,
		.axis = nullptr,
		.frame = nullptr,
		.axis_source = nullptr,
		.axis_stop = nullptr,
		.axis_discrete = nullptr,
		.axis_value120 = nullptr,
		.axis_relative_direction = nullptr
	};

	static constexpr wl_keyboard_listener wl_keyboardListener = {
		.keymap = wayland_keyboard_keymap_callback,
		.enter = wayland_keyboard_enter_callback,
		.leave = wayland_keyboard_leave_callback,
		.key = wayland_keyboard_key_callback,
		.modifiers = wayland_keyboard_modifiers_callback,
		.repeat_info = wayland_keyboard_repeat_info_callback
	};

	static void wayland_seat_capabilities_callback(void *const pData, wl_seat *wl_pSeat, const uint32_t m32Capabilities) {
		if ((m32Capabilities & WL_SEAT_CAPABILITY_POINTER)) {
			wl_pPointer = wl_seat_get_pointer(wl_pSeat);
			PRINT_DEBUG("Adding listener to the pointer ", wl_pPointer);
			if (wl_pointer_add_listener(wl_pPointer, &wl_pointerListener, nullptr) != 0) {
				RE_ERROR("Failed to add a listener to the Wayland pointer ", wl_pPointer);
				goto WAYLAND_POINTER_INIT_FAILURE;
			}
		} else {
		WAYLAND_POINTER_INIT_FAILURE:
			if (wl_pPointer)
				destroy_wayland_pointer();
			wl_pPointer = nullptr;
		}
		if ((m32Capabilities & WL_SEAT_CAPABILITY_KEYBOARD)) {
			wl_pKeyboard = wl_seat_get_keyboard(wl_pSeat);
			PRINT_DEBUG("Adding listener to the keyboard ", wl_pKeyboard);
			if (wl_keyboard_add_listener(wl_pKeyboard, &wl_keyboardListener, nullptr) != 0) {
				RE_ERROR("Failed to add a listener to the Wayland keyboard ", wl_pKeyboard);
				goto WAYLAND_KEYBOARD_INIT_FAILURE;
			}
		} else {
		WAYLAND_KEYBOARD_INIT_FAILURE:
			if (wl_pKeyboard)
				destroy_wayland_keyboard();
			wl_pKeyboard = nullptr;
		}
	}

	static constexpr wl_seat_listener wl_seatListener = {
		.capabilities = wayland_seat_capabilities_callback,
		.name = nullptr
	};

	static bool init_wayland_global_objects() {
		PRINT_DEBUG("Retrieving the registry from the Wayland connection ", wl_pDisplay);
		if ((wl_pRegistry = wl_display_get_registry(wl_pDisplay))) {
			PRINT_DEBUG("Adding a listener to the registry ", wl_pRegistry, " in Wayland");
			if (wl_registry_add_listener(wl_pRegistry, &wl_registryListener, nullptr) == 0) {
				PRINT_DEBUG("Sending request to the Wayland server to call listeners");
				wl_display_roundtrip(wl_pDisplay);
				if (wlCompositor.waylandObject && xdgWmBase.waylandObject && wlSeat.waylandObject && !waylandMonitors.empty()) {
					waylandMonitors.shrink_to_fit();
					return true;
				} else
					RE_ERROR("Failed to get essential Wayland global objects or to initialize them");
				for (WlOutput wlOutput : waylandMonitors) {
					if (wl_output_get_version(wlOutput.waylandObject) >= static_cast<uint32_t>(WL_OUTPUT_RELEASE_SINCE_VERSION))
						wl_output_release(wlOutput.waylandObject);
					else
						wl_output_destroy(wlOutput.waylandObject);
				}
				waylandMonitors.clear();
				if (wlSeat.waylandObject) {
					PRINT_DEBUG("Destroying seat ", wlSeat.waylandObject, " due to failure creating the Wayland window");
					if (wl_seat_get_version(wlSeat.waylandObject) >= static_cast<uint32_t>(WL_SEAT_RELEASE_SINCE_VERSION))
						wl_seat_release(wlSeat.waylandObject);
					else
						wl_seat_destroy(wlSeat.waylandObject);
					wlSeat.waylandObject = nullptr;
				}
				if (xdgWmBase.waylandObject) {
					PRINT_DEBUG("Destroying XDG wm base ", xdgWmBase.waylandObject, " due to failure creating the Wayland window");
					xdg_wm_base_destroy(xdgWmBase.waylandObject);
					xdgWmBase.waylandObject = nullptr;
				}
				if (wlCompositor.waylandObject) {
					PRINT_DEBUG("Destroying compositor ", wlCompositor.waylandObject, " due to failure creating the Wayland window");
					wl_compositor_destroy(wlCompositor.waylandObject);
					wlCompositor.waylandObject = nullptr;
				}
			} else
				RE_ERROR("Failed to add a listener to the Wayland registry ", wl_pRegistry);
			PRINT_DEBUG("Destroying registry ", wl_pRegistry, " due to failure connecting to the Wayland server properly");
			wl_registry_destroy(wl_pRegistry);
		} else
			RE_ERROR("Failed to retrieve a registry from the connection to the Wayland display ", wl_pDisplay);
		return false;
	}

	static void destroy_wayland_global_objects() {
		for (WlOutput wlOutput : waylandMonitors) {
			PRINT_DEBUG("Destroying output ", wlOutput.waylandObject);
			if (wl_output_get_version(wlOutput.waylandObject) >= static_cast<uint32_t>(WL_OUTPUT_RELEASE_SINCE_VERSION))
				wl_output_release(wlOutput.waylandObject);
			else
				wl_output_destroy(wlOutput.waylandObject);
		}
		waylandMonitors.clear();
		if (wlSeat.waylandObject) {
			PRINT_DEBUG("Destroying seat ", wlSeat.waylandObject);
			if (wl_seat_get_version(wlSeat.waylandObject) >= static_cast<uint32_t>(WL_SEAT_RELEASE_SINCE_VERSION))
				wl_seat_release(wlSeat.waylandObject);
			else
				wl_seat_destroy(wlSeat.waylandObject);
			wlSeat.waylandObject = nullptr;
		}
		if (xdgWmBase.waylandObject) {
			PRINT_DEBUG("Destroying XDG wm base ", xdgWmBase.waylandObject);
			xdg_wm_base_destroy(xdgWmBase.waylandObject);
			xdgWmBase.waylandObject = nullptr;
		}
		if (wlCompositor.waylandObject) {
			PRINT_DEBUG("Destroying compositor ", wlCompositor.waylandObject);
			wl_compositor_destroy(wlCompositor.waylandObject);
			wlCompositor.waylandObject = nullptr;
		}
		wl_registry_destroy(wl_pRegistry);
	}

	static bool init_wayland_window() {
		PRINT_DEBUG("Adding a listener the XDG wm base ", xdgWmBase.waylandObject);
		if (xdg_wm_base_add_listener(xdgWmBase.waylandObject, &xdg_wmBasePinger, nullptr) == 0) {
			PRINT_DEBUG("Creating surface from the compositor ", wlCompositor.waylandObject, " in Wayland");
			if ((wl_pSurface = wl_compositor_create_surface(wlCompositor.waylandObject))) {
				PRINT_DEBUG("Retrieving XDG surface from XDG wm base ", xdgWmBase.waylandObject, " and Wayland surface ", wl_pSurface);
				if ((xdg_pSurface = xdg_wm_base_get_xdg_surface(xdgWmBase.waylandObject, wl_pSurface))) {
					PRINT_DEBUG("Adding a listener to the XDG surface");
					if (xdg_surface_add_listener(xdg_pSurface, &xdg_surfaceListener, nullptr) == 0) {
						PRINT_DEBUG("Retrieving XDG toplevel from surface ", xdg_pSurface);
						if ((xdg_pToplevel = xdg_surface_get_toplevel(xdg_pSurface))) {
							PRINT_DEBUG("Adding listener to the XDG toplevel ", xdg_pToplevel);
							if (xdg_toplevel_add_listener(xdg_pToplevel, &xdg_toplevelListener, nullptr) == 0)
								return true;
							else
								RE_ERROR("Failed to add a listener to the XDG toplevel ", xdg_pToplevel);
							PRINT_DEBUG("Destroying XDG toplevel ", xdg_pToplevel, " due to failure creating the window");
							xdg_toplevel_destroy(xdg_pToplevel);
						} else
							RE_ERROR("Failed to retrieve toplevel of XDG surface ", xdg_pSurface);
					} else
						RE_ERROR("Failed to add a listener to the XDG surface ", xdg_pSurface);
					PRINT_DEBUG("Destroying XDG surface ", xdg_pToplevel, " due to failure creating the window");
					xdg_surface_destroy(xdg_pSurface);
				} else
					RE_ERROR("Failed to retrieve surface of XDG wm base ", xdgWmBase.waylandObject, " and Wayland surface ", wl_pSurface);
				PRINT_DEBUG("Destroying surface ", wl_pSurface, " due to failure creating the window");
				wl_surface_destroy(wl_pSurface);
			} else
				RE_ERROR("Failed to create a Wayland surface with compositor ", wlCompositor.waylandObject);
		} else
			RE_ERROR("Failed to add a listener to the XDG wm base ", xdgWmBase.waylandObject);
		return false;
	}

	static void destroy_wayland_window() {
		PRINT_DEBUG("Destroying window-related resources");
		xdg_toplevel_destroy(xdg_pToplevel);
		xdg_surface_destroy(xdg_pSurface);
		wl_surface_destroy(wl_pSurface);
	}

	static bool init_wayland_input() {
		PRINT_DEBUG("Adding listener to the seat ", wlSeat.waylandObject);
		if (wl_seat_add_listener(wlSeat.waylandObject, &wl_seatListener, nullptr) == 0) {
			PRINT_DEBUG("Received pointer ", wl_pPointer, " and keyboard ", wl_pKeyboard);
			return true;
		} else
			RE_ERROR("Failed to add a listener to the Wayland seat ", wlSeat.waylandObject);
		return false;
	}

	static void destroy_wayland_input() {
		PRINT_DEBUG("Destroying input-related resources");
		if (wl_pKeyboard)
			destroy_wayland_keyboard();
		if (wl_pPointer)
			destroy_wayland_pointer();
	}

	bool wayland_create_window() {
		PRINT_DEBUG("Connecting to the Wayland server");
		if ((wl_pDisplay = wl_display_connect(nullptr))) {
			if (init_wayland_global_objects()) {
				if (init_wayland_window()) {
					if (init_wayland_input()) {
						PRINT_DEBUG("Setting title of the XDG toplevel ", xdg_pToplevel);
						xdg_toplevel_set_title(xdg_pToplevel, pacWindowTitle);
						xdg_toplevel_set_app_id(xdg_pToplevel, pacWindowTitle);
						wl_surface_commit(wl_pSurface);
						wl_display_flush(wl_pDisplay);
						/*xdg_toplevel_set_min_size(xdg_pToplevel, MIN_WINDOW_WIDTH, MIN_WINDOW_HEIGHT);
						if (!IS_FULLSCREEN())
							xdg_toplevel_set_max_size(xdg_pToplevel, largestMonitorSize[0] + MAX_WINDOW_WIDTH_RELATIVE_TO_MONITOR, largestMonitorSize[1] + MAX_WINDOW_HEIGHT_RELATIVE_TO_MONITOR);*/
						return true;
						destroy_wayland_input();
					}
					destroy_wayland_window();
				}
				destroy_wayland_global_objects();
			}
			PRINT_DEBUG("Disconnecting from the Wayland server due to failure creating the window");
			wl_display_disconnect(wl_pDisplay);
		} else
			RE_ERROR("Failed to connect to the Wayland server");
		return false;
	}

	void wayland_destroy_window() {
		PRINT_DEBUG("Destroying the Wayland window resources");
		destroy_wayland_input();
		destroy_wayland_window();
		destroy_wayland_global_objects();
		wl_display_disconnect(wl_pDisplay);
	}
	
	void wayland_show_window() {
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
