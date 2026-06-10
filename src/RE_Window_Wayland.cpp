#include "RE_Window.hpp"
#include "RE_Window_Wayland.hpp"
#include "RE_Renderer.hpp"

#ifdef RE_OS_LINUX

#include <dlfcn.h>
#include <linux/input-event-codes.h>
#include <sys/mman.h>

namespace RE {

#define REQUIRED_VERSION_WL_COMPOSITOR static_cast<uint32_t>(std::max(WL_COMPOSITOR_CREATE_SURFACE_SINCE_VERSION, WL_COMPOSITOR_CREATE_REGION_SINCE_VERSION))
#define REQUIRED_VERSION_XDG_WM_BASE static_cast<uint32_t>(std::max(XDG_WM_BASE_PING_SINCE_VERSION, std::max(XDG_WM_BASE_DESTROY_SINCE_VERSION, std::max(XDG_WM_BASE_GET_XDG_SURFACE_SINCE_VERSION, XDG_WM_BASE_PONG_SINCE_VERSION))))
#define REQUIRED_VERSION_WL_SEAT static_cast<uint32_t>(std::max(WL_SEAT_CAPABILITIES_SINCE_VERSION, std::max(WL_SEAT_GET_POINTER_SINCE_VERSION, WL_SEAT_GET_KEYBOARD_SINCE_VERSION)))
#define REQUIRED_VERSION_WL_OUTPUT static_cast<uint32_t>(std::max(WL_OUTPUT_GEOMETRY_SINCE_VERSION, WL_OUTPUT_MODE_SINCE_VERSION))

#define KEYCODE_TO_XKB_OFFSET 8

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

	struct WaylandMonitorInfo final {
		WlOutput wlOutput;
		Vector2u size;
	};

	struct WaylandKeyboardInfo final {
		wl_keyboard *wl_pKeyboard;
		xkb_keymap *xkb_pKeymap;
		xkb_state *xkb_pState;
	};

	wl_display *wl_pDisplay;
	static wl_registry *wl_pRegistry;
	static WlCompositor wlCompositor = {};
	static XdgWmBase xdgWmBase = {};
	static WlSeat wlSeat = {};
	wl_surface *wl_pSurface;
	static wl_output *wl_pCurrentOutput = nullptr;
	static xdg_surface *xdg_pSurface;
	static xdg_toplevel *xdg_pToplevel;
	static std::vector<WaylandMonitorInfo> waylandMonitors;
	static xkb_context *xkb_pContext;
	static wl_pointer *wl_pPointer = nullptr;
	static WaylandKeyboardInfo waylandKeyboard = {};
	static Vector2i actualCursorPosition,
		actualWindowSize;

	static bool is_cursor_within_content() {
		return (actualCursorPosition[0] >= WINDOW_WAYLAND_X_OFFSET && actualCursorPosition[0] < static_cast<int32_t>(actualWindowSize[0]) - WINDOW_WAYLAND_BORDER_TOTAL_SIZE)
				&& (actualCursorPosition[1] >= WINDOW_WAYLAND_Y_OFFSET && actualCursorPosition[1] < static_cast<int32_t>(actualWindowSize[1]) - WINDOW_WAYLAND_BORDER_TOTAL_SIZE - WINDOW_WAYLAND_BAR_SIZE);
	}

	static WindowArea get_wayland_window_area_cursor_is_in() {
		if (actualCursorPosition[0] < WINDOW_WAYLAND_BORDER_TOTAL_SIZE) {
			if (actualCursorPosition[1] < WINDOW_WAYLAND_BORDER_TOTAL_SIZE)
				return WINDOW_AREA_TOP_LEFT;
			else if (actualCursorPosition[1] >= actualWindowSize[1] - WINDOW_WAYLAND_BORDER_TOTAL_SIZE)
				return WINDOW_AREA_BOTTOM_LEFT;
			return WINDOW_AREA_LEFT;
		} else if (actualCursorPosition[0] >= actualWindowSize[0] - WINDOW_WAYLAND_BORDER_TOTAL_SIZE) {
			if (actualCursorPosition[1] < WINDOW_WAYLAND_BORDER_TOTAL_SIZE)
				return WINDOW_AREA_TOP_RIGHT;
			else if (actualCursorPosition[1] >= actualWindowSize[1] - WINDOW_WAYLAND_BORDER_TOTAL_SIZE)
				return WINDOW_AREA_BOTTOM_RIGHT;
			return WINDOW_AREA_RIGHT;
		} else if (actualCursorPosition[1] < WINDOW_WAYLAND_BORDER_TOTAL_SIZE)
			return WINDOW_AREA_TOP;
		else if (actualCursorPosition[1] >= actualWindowSize[1] - WINDOW_WAYLAND_BORDER_TOTAL_SIZE)
			return WINDOW_AREA_BOTTOM;
		else if (actualCursorPosition[1] < WINDOW_WAYLAND_BORDER_TOTAL_SIZE + WINDOW_WAYLAND_BAR_SIZE) {
			if (actualCursorPosition[0] >= actualWindowSize[0] - WINDOW_WAYLAND_BORDER_TOTAL_SIZE - WINDOW_WAYLAND_BUTTON_WIDTH)
				return WINDOW_AREA_BUTTON_CLOSE;
			else if (actualCursorPosition[0] >= actualWindowSize[0] - WINDOW_WAYLAND_BORDER_TOTAL_SIZE - WINDOW_WAYLAND_BUTTON_WIDTH * 2)
				return WINDOW_AREA_BUTTON_MAXIMIZE;
			else if (actualCursorPosition[0] >= actualWindowSize[0] - WINDOW_WAYLAND_BORDER_TOTAL_SIZE - WINDOW_WAYLAND_BUTTON_WIDTH * 3)
				return WINDOW_AREA_BUTTON_MINIMIZE;
			return WINDOW_AREA_BAR;
		} else
			return WINDOW_AREA_CONTENT;
	}

	static void destroy_wayland_pointer() {
		PRINT_DEBUG("Destroying pointer ", wl_pPointer);
		if (wl_pointer_get_version(wl_pPointer) >= static_cast<uint32_t>(WL_POINTER_RELEASE_SINCE_VERSION))
			wl_pointer_release(wl_pPointer);
		else
			wl_pointer_destroy(wl_pPointer);
		wl_pPointer = nullptr;
	}

	static void destroy_wayland_keyboard() {
		PRINT_DEBUG("Destroying keyboard ", waylandKeyboard.wl_pKeyboard);
		if (wl_keyboard_get_version(waylandKeyboard.wl_pKeyboard) >= static_cast<uint32_t>(WL_KEYBOARD_RELEASE_SINCE_VERSION))
			wl_keyboard_release(waylandKeyboard.wl_pKeyboard);
		else
			wl_keyboard_destroy(waylandKeyboard.wl_pKeyboard);
		waylandKeyboard.wl_pKeyboard = nullptr;
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
						WlOutput {
							reinterpret_cast<wl_output*>(
								wl_registry_bind(
									wl_pRegistry,
									u32Name,
									&wl_output_interface,
									REQUIRED_VERSION_WL_OUTPUT)),
							u32Name,
							u32Version
						},
						Vector2u {});
			} else
				RE_ERROR("The version of output is too low (", u32Version, " < ", REQUIRED_VERSION_WL_OUTPUT, ")");
		}
	}

	static void wayland_registry_remove_callback(void *const pData, wl_registry *const wl_pRegistry, const uint32_t u32Name) {
		for (auto it = waylandMonitors.begin(); it != waylandMonitors.end(); it++) {
			if (it->wlOutput.u32Name != u32Name)
				continue;
			PRINT_DEBUG("Removing the Wayland output object ", it->wlOutput.waylandObject);
			if (it->wlOutput.u32Version >= static_cast<uint32_t>(WL_OUTPUT_RELEASE_SINCE_VERSION))
				wl_output_release(it->wlOutput.waylandObject);
			else
				wl_output_destroy(it->wlOutput.waylandObject);
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

	static void wayland_output_geometry_callback(void *const pData, wl_output *const wl_pOutput, const int32_t i32X, const int32_t i32Y, const int32_t i32PhysicalWidth, const int32_t i32PhysicalHeight, const int32_t i32Subpixel, const char *const pacMake, const char *const pacModel, const int32_t i32Transform) {
	}

	static void wayland_output_mode_callback(void *const pData, wl_output *const wl_pOutput, const uint32_t u32Flags, const int32_t i32Width, const int32_t i32Height, const int32_t i32RefreshRate) {
		largestMonitorSize[0] = std::max<uint32_t>(largestMonitorSize[0], i32Width);
		largestMonitorSize[1] = std::max<uint32_t>(largestMonitorSize[1], i32Height);
		const auto it = std::find_if(
				waylandMonitors.begin(),
				waylandMonitors.end(),
				[&](const WaylandMonitorInfo &rMonitorInfo) {
					return rMonitorInfo.wlOutput.waylandObject == wl_pOutput;
				});
		it->size[0] = static_cast<uint32_t>(i32Width);
		it->size[1] = static_cast<uint32_t>(i32Height);
	}

	static constexpr wl_output_listener wl_outputListener = {
		.geometry = wayland_output_geometry_callback,
		.mode = wayland_output_mode_callback
	};

	static void xdg_wm_base_ping(void *const pData, xdg_wm_base *const xdg_pWmBase, const uint32_t u32Serial) {
		xdg_wm_base_pong(xdg_pWmBase, u32Serial);
	}

	static constexpr xdg_wm_base_listener xdg_wmBasePinger = {
		.ping = xdg_wm_base_ping
	};

	static void wayland_surface_enter_callback(void *const pData, wl_surface *const wl_pSurface, wl_output *const wl_pNewOutput) {
		if (wl_pCurrentOutput != wl_pNewOutput)
			wl_pCurrentOutput = wl_pNewOutput;
	}

	static void wayland_surface_leave_callback(void *const pData, wl_surface *const wl_pSurface, wl_output *const wl_pOldOutput) {
		if (wl_pCurrentOutput == wl_pOldOutput)
			wl_pCurrentOutput = nullptr;
	}

	static constexpr wl_surface_listener wl_surfaceListener {
		.enter = wayland_surface_enter_callback,
		.leave = wayland_surface_leave_callback,
		.preferred_buffer_scale = nullptr,
		.preferred_buffer_transform = nullptr
	};

	static void xdg_surface_configure_callback(void *const pData, xdg_surface *const xdg_pSurface, const uint32_t u32Serial) {
		xdg_surface_ack_configure(xdg_pSurface, u32Serial);
		wl_display_flush(wl_pDisplay);
	}

	static constexpr xdg_surface_listener xdg_surfaceListener = {
		.configure = xdg_surface_configure_callback
	};

	static void xdg_toplevel_configure_callback(void *const pData, xdg_toplevel *const xdg_pToplevel, int32_t i32Width, int32_t i32Height, wl_array *const wl_pStates) {
		if (i32Width <= 0 && actualWindowSize[0] == 0)
			i32Width = largestMonitorSize[0] / 5 * 3 + WINDOW_WAYLAND_EXTRA_WIDTH;
		if (i32Height <= 0 && actualWindowSize[1] == 0)
			i32Height = largestMonitorSize[1] / 5 * 3 + WINDOW_WAYLAND_EXTRA_HEIGHT;
		actualWindowSize[0] = i32Width;
		actualWindowSize[1] = i32Height;
		window_resize_event(static_cast<uint32_t>(i32Width - WINDOW_WAYLAND_EXTRA_WIDTH), static_cast<uint32_t>(i32Height - WINDOW_WAYLAND_EXTRA_HEIGHT));
		PRINT_DEBUG("Updating regions on Wayland surface ", wl_pSurface);
		wl_region *const wl_pRegion = wl_compositor_create_region(wlCompositor.waylandObject);
		wl_region_add(wl_pRegion, WINDOW_WAYLAND_SHADOW_SIZE, WINDOW_WAYLAND_SHADOW_SIZE, actualWindowSize[0] - WINDOW_WAYLAND_SHADOW_SIZE * 2, actualWindowSize[1] - WINDOW_WAYLAND_SHADOW_SIZE * 2);
		wl_surface_set_opaque_region(wl_pSurface, wl_pRegion);
		wl_region_destroy(wl_pRegion);

		bool bMaximized = false,
			bFullscreen = false;
		for (xdg_toplevel_state *xdg_pToplevelState = static_cast<xdg_toplevel_state*>(wl_pStates->data); xdg_pToplevelState != (static_cast<xdg_toplevel_state*>(wl_pStates->data) + wl_pStates->size / sizeof(xdg_toplevel_state)); xdg_pToplevelState++) {
			switch (*xdg_pToplevelState) {
				case XDG_TOPLEVEL_STATE_MAXIMIZED:
					bMaximized = true;
					break;
				case XDG_TOPLEVEL_STATE_FULLSCREEN:
					bFullscreen = true;
					break;
				default:
					break;
			}
		}
		set_bits<decltype(u8WindowFlagBits)>(u8WindowFlagBits, bMaximized, WINDOW_MAXIMIZED_BIT);
		set_bits<decltype(u8WindowFlagBits)>(u8WindowFlagBits, bFullscreen, WINDOW_FULLSCREEN_BIT);
	}

	static void xdg_toplevel_close_callback(void *const pData, xdg_toplevel *const xdg_pToplevel) {
		set_bits<decltype(u8WindowFlagBits)>(u8WindowFlagBits, true, WINDOW_CLOSE_FLAG_BIT);
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
		actualCursorPosition[0] = wl_fixed_to_int(wl_x);
		actualCursorPosition[1] = wl_fixed_to_int(wl_y);
		if (pCursorShaderData)
			for (uint8_t u8DimensionIndex = 0; u8DimensionIndex < actualCursorPosition.dimensions(); u8DimensionIndex++)
				pCursorShaderData->a2u32Position[u8DimensionIndex] = actualCursorPosition[u8DimensionIndex];
		if (is_cursor_within_content())
			cursor_event(actualCursorPosition[0] - WINDOW_WAYLAND_X_OFFSET, actualCursorPosition[1] - WINDOW_WAYLAND_Y_OFFSET);
	}

	static void wayland_pointer_button_callback(void *const pData, wl_pointer *const wl_pPointer, const uint32_t u32Serial, const uint32_t u32Time, const uint32_t u32Button, const uint32_t u32State) {
		const WindowArea eWindowAreaOfCursor = get_wayland_window_area_cursor_is_in();
		if (eWindowAreaOfCursor != WINDOW_AREA_CONTENT) {
			if (u32State != WL_POINTER_BUTTON_STATE_PRESSED || u32Button != BTN_LEFT)
				return;
			switch (eWindowAreaOfCursor) {
				case WINDOW_AREA_TOP_LEFT:
					xdg_toplevel_resize(xdg_pToplevel, wlSeat.waylandObject, u32Serial, XDG_TOPLEVEL_RESIZE_EDGE_TOP_LEFT);
					break;
				case WINDOW_AREA_TOP_RIGHT:
					xdg_toplevel_resize(xdg_pToplevel, wlSeat.waylandObject, u32Serial, XDG_TOPLEVEL_RESIZE_EDGE_TOP_RIGHT);
					break;
				case WINDOW_AREA_BOTTOM_LEFT:
					xdg_toplevel_resize(xdg_pToplevel, wlSeat.waylandObject, u32Serial, XDG_TOPLEVEL_RESIZE_EDGE_BOTTOM_LEFT);
					break;
				case WINDOW_AREA_BOTTOM_RIGHT:
					xdg_toplevel_resize(xdg_pToplevel, wlSeat.waylandObject, u32Serial, XDG_TOPLEVEL_RESIZE_EDGE_BOTTOM_RIGHT);
					break;
				case WINDOW_AREA_LEFT:
					xdg_toplevel_resize(xdg_pToplevel, wlSeat.waylandObject, u32Serial, XDG_TOPLEVEL_RESIZE_EDGE_LEFT);
					break;
				case WINDOW_AREA_RIGHT:
					xdg_toplevel_resize(xdg_pToplevel, wlSeat.waylandObject, u32Serial, XDG_TOPLEVEL_RESIZE_EDGE_RIGHT);
					break;
				case WINDOW_AREA_TOP:
					xdg_toplevel_resize(xdg_pToplevel, wlSeat.waylandObject, u32Serial, XDG_TOPLEVEL_RESIZE_EDGE_TOP);
					break;
				case WINDOW_AREA_BOTTOM:
					xdg_toplevel_resize(xdg_pToplevel, wlSeat.waylandObject, u32Serial, XDG_TOPLEVEL_RESIZE_EDGE_BOTTOM);
					break;
				case WINDOW_AREA_BUTTON_CLOSE:
					set_bits<decltype(u8WindowFlagBits)>(u8WindowFlagBits, true, WINDOW_CLOSE_FLAG_BIT);
					break;
				case WINDOW_AREA_BUTTON_MAXIMIZE:
					{
						const bool bMaximized = !are_bits_true<decltype(u8WindowFlagBits)>(u8WindowFlagBits, WINDOW_MAXIMIZED_BIT);
						set_bits<decltype(u8WindowFlagBits)>(u8WindowFlagBits, bMaximized, WINDOW_MAXIMIZED_BIT);
						if (bMaximized)
							xdg_toplevel_set_maximized(xdg_pToplevel);
						else
							xdg_toplevel_unset_maximized(xdg_pToplevel);
					}
					break;
				case WINDOW_AREA_BUTTON_MINIMIZE:
					xdg_toplevel_set_minimized(xdg_pToplevel);
					break;
				default:
					xdg_toplevel_move(xdg_pToplevel, wlSeat.waylandObject, u32Serial);
					break;
			}
		} else {
			switch (u32Button) {
				case BTN_LEFT:
					input_event(RE_INPUT_BUTTON_LEFT, 0, u32State == WL_POINTER_BUTTON_STATE_PRESSED, true);
					break;
				case BTN_RIGHT:
					input_event(RE_INPUT_BUTTON_RIGHT, 0, u32State == WL_POINTER_BUTTON_STATE_PRESSED, true);
					break;
				case BTN_MIDDLE:
					input_event(RE_INPUT_BUTTON_MIDDLE, 0, u32State == WL_POINTER_BUTTON_STATE_PRESSED, true);
					break;
			}
		}
	}

	static void wayland_pointer_scroll_callback(void *const pData, wl_pointer *const wl_pPointer, const uint32_t u32Time, const uint32_t u32Axis, const wl_fixed_t wl_value) {
		if (u32Axis == WL_POINTER_AXIS_VERTICAL_SCROLL && is_cursor_within_content())
			input_event(wl_fixed_to_double(wl_value) < 0 ? RE_INPUT_SCROLL_DOWN : RE_INPUT_SCROLL_UP, 0, true, true);
	}

	static constexpr wl_pointer_listener wl_pointerListener = {
		.enter = wayland_pointer_enter_callback,
		.leave = wayland_pointer_leave_callback,
		.motion = wayland_pointer_motion_callback,
		.button = wayland_pointer_button_callback,
		.axis = wayland_pointer_scroll_callback,
		.frame = nullptr,
		.axis_source = nullptr,
		.axis_stop = nullptr,
		.axis_discrete = nullptr,
		.axis_value120 = nullptr,
		.axis_relative_direction = nullptr
	};

	static void wayland_keyboard_keymap_callback(void *const pData, wl_keyboard *const wl_pKeyboard, const uint32_t u32Format, const int32_t i32FileDescriptor, const uint32_t u32Size) {
		if (waylandKeyboard.wl_pKeyboard != wl_pKeyboard)
			return;
		switch (u32Format) {
			case WL_KEYBOARD_KEYMAP_FORMAT_XKB_V1:
				{
					PRINT_DEBUG("Mapping keymap buffer from file descriptor ", i32FileDescriptor);
					char *const pacKeymapBuffer = static_cast<char*>(mmap(nullptr, u32Size, PROT_READ, MAP_SHARED, i32FileDescriptor, 0));
					if (pacKeymapBuffer) {
						PRINT_DEBUG("Creating XKB keymap from mapped keymap buffer ", pacKeymapBuffer);
						if ((waylandKeyboard.xkb_pKeymap = xkb_keymap_new_from_buffer(xkb_pContext, pacKeymapBuffer, u32Size, XKB_KEYMAP_FORMAT_TEXT_V1, XKB_KEYMAP_COMPILE_NO_FLAGS))) {
							PRINT_DEBUG("Creating XKB state from keymap ", waylandKeyboard.xkb_pKeymap);
							if ((waylandKeyboard.xkb_pState = xkb_state_new(waylandKeyboard.xkb_pKeymap))) {
								PRINT_DEBUG("Unmapping keymap buffer ", pacKeymapBuffer);
								munmap(pacKeymapBuffer, u32Size);
								return;
							} else
								RE_ERROR("Failed to create an XKB state from the keymap ", pacKeymapBuffer);
						} else
							RE_ERROR("Failed to create an XKB keymap");
						PRINT_DEBUG("Unmapping content of file descriptor from RAM due to failure creating XKB resources");
						munmap(pacKeymapBuffer, u32Size);
					} else
						RE_ERROR("Failed mapping the keymap from file descriptor ", i32FileDescriptor, " to RAM");
				}
				break;
			case WL_KEYBOARD_KEYMAP_FORMAT_NO_KEYMAP:
			default:
				break;
		}
	}

	static void wayland_keyboard_enter_callback(void *const pData, wl_keyboard *const wl_pKeyboard, const uint32_t u32Serial, struct wl_surface *const wl_pSurface, struct wl_array *const wl_pKeys) {
	}

	static void wayland_keyboard_leave_callback(void *const pData, wl_keyboard *const wl_pKeyboard, const uint32_t u32Serial, wl_surface *const wl_pSurface) {
	}

	static void wayland_keyboard_key_callback(void *const pData, wl_keyboard *const wl_pKeyboard, const uint32_t u32Serial, const uint32_t u32Time, const uint32_t u32Key, const uint32_t u32State) {
		const xkb_keysym_t xkb_keySym = xkb_state_key_get_one_sym(waylandKeyboard.xkb_pState, u32Key + KEYCODE_TO_XKB_OFFSET);
		switch (xkb_keySym) {
			case XKB_KEY_Super_L:
			case XKB_KEY_Super_R:
				return;
			case XKB_KEY_F11:
				if (u32State == WL_KEYBOARD_KEY_STATE_PRESSED) {
					const bool bFullscreen = !IS_FULLSCREEN();
					SET_FULLSCREEN(bFullscreen);
					if (bFullscreen) {
						PRINT_DEBUG("Enabling fullscreen on Wayland window");
						xdg_toplevel_set_max_size(xdg_pToplevel, std::numeric_limits<int32_t>::max(), std::numeric_limits<int32_t>::max());
						xdg_toplevel_set_fullscreen(xdg_pToplevel, wl_pCurrentOutput);
					} else {
						PRINT_DEBUG("Disabling fullscreen on Wayland window");
						xdg_toplevel_unset_fullscreen(xdg_pToplevel);
						xdg_toplevel_set_max_size(xdg_pToplevel, largestMonitorSize[0] + MAX_WINDOW_WIDTH_RELATIVE_TO_MONITOR, largestMonitorSize[1] + MAX_WINDOW_HEIGHT_RELATIVE_TO_MONITOR);
					}
				}
				[[fallthrough]];
			default:
				PRINT_DEBUG("Firing input event with key symbol ", xkb_keySym, " and scancode ", u32Key);
				input_event(key_from_virtual_xkb_keysym(xkb_keySym), u32Key, u32State == WL_KEYBOARD_KEY_STATE_PRESSED, false);
				break;
		}
	}

	static void wayland_keyboard_modifiers_callback(void *const pData, wl_keyboard *const wl_pKeyboard, const uint32_t u32Serial, const uint32_t u32ModsDepressed, const uint32_t u32ModsLatched, const uint32_t u32ModsLocked, const uint32_t u32Group) {
	}

	static void wayland_keyboard_repeat_info_callback(void *const pData, wl_keyboard *const wl_pKeyboard, const int32_t i32Rate, const int32_t i32Delay) {
	}

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
			waylandKeyboard.wl_pKeyboard = wl_seat_get_keyboard(wl_pSeat);
			PRINT_DEBUG("Adding listener to the keyboard ", waylandKeyboard.wl_pKeyboard);
			if (wl_keyboard_add_listener(waylandKeyboard.wl_pKeyboard, &wl_keyboardListener, nullptr) != 0) {
				RE_ERROR("Failed to add a listener to the Wayland keyboard ", waylandKeyboard.wl_pKeyboard);
				goto WAYLAND_KEYBOARD_INIT_FAILURE;
			}
		} else {
		WAYLAND_KEYBOARD_INIT_FAILURE:
			if (waylandKeyboard.wl_pKeyboard)
				destroy_wayland_keyboard();
			waylandKeyboard.wl_pKeyboard = nullptr;
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
					size_t monitorIndex = 0;
					for (const WaylandMonitorInfo &rMonitorInfo : waylandMonitors) {
						if (wl_output_add_listener(rMonitorInfo.wlOutput.waylandObject, &wl_outputListener, nullptr) != 0) {
							RE_ERROR("Failed to add a listener to Wayland output ", rMonitorInfo.wlOutput.waylandObject);
							break;
						}
						monitorIndex++;
					}
					if (monitorIndex == waylandMonitors.size()) {
						waylandMonitors.shrink_to_fit();
						return true;
					}
				} else
					RE_ERROR("Failed to get essential Wayland global objects or to initialize them");
				for (const WaylandMonitorInfo &rMonitorInfo : waylandMonitors) {
					if (wl_output_get_version(rMonitorInfo.wlOutput.waylandObject) >= static_cast<uint32_t>(WL_OUTPUT_RELEASE_SINCE_VERSION))
						wl_output_release(rMonitorInfo.wlOutput.waylandObject);
					else
						wl_output_destroy(rMonitorInfo.wlOutput.waylandObject);
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
		for (const WaylandMonitorInfo &rMonitorInfo : waylandMonitors) {
			PRINT_DEBUG("Destroying output ", rMonitorInfo.wlOutput.waylandObject);
			if (wl_output_get_version(rMonitorInfo.wlOutput.waylandObject) >= static_cast<uint32_t>(WL_OUTPUT_RELEASE_SINCE_VERSION))
				wl_output_release(rMonitorInfo.wlOutput.waylandObject);
			else
				wl_output_destroy(rMonitorInfo.wlOutput.waylandObject);
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
				PRINT_DEBUG("Adding a listener to the Wayland surface ", wl_pSurface);
				if (wl_surface_add_listener(wl_pSurface, &wl_surfaceListener, nullptr) == 0) {
					PRINT_DEBUG("Retrieving XDG surface from XDG wm base ", xdgWmBase.waylandObject, " and Wayland surface ", wl_pSurface);
					if ((xdg_pSurface = xdg_wm_base_get_xdg_surface(xdgWmBase.waylandObject, wl_pSurface))) {
						PRINT_DEBUG("Adding a listener to the XDG surface");
						if (xdg_surface_add_listener(xdg_pSurface, &xdg_surfaceListener, nullptr) == 0) {
							PRINT_DEBUG("Retrieving XDG toplevel from surface ", xdg_pSurface);
							if ((xdg_pToplevel = xdg_surface_get_toplevel(xdg_pSurface))) {
								PRINT_DEBUG("Adding listener to the XDG toplevel ", xdg_pToplevel);
								if (xdg_toplevel_add_listener(xdg_pToplevel, &xdg_toplevelListener, nullptr) == 0) {
									xdg_toplevel_set_min_size(xdg_pToplevel, MIN_WINDOW_WIDTH + WINDOW_WAYLAND_EXTRA_WIDTH, MIN_WINDOW_HEIGHT + WINDOW_WAYLAND_EXTRA_HEIGHT);
									if (!IS_FULLSCREEN())
										xdg_toplevel_set_max_size(xdg_pToplevel, largestMonitorSize[0] + MAX_WINDOW_WIDTH_RELATIVE_TO_MONITOR, largestMonitorSize[1] + MAX_WINDOW_HEIGHT_RELATIVE_TO_MONITOR);
									else
										xdg_toplevel_set_fullscreen(xdg_pToplevel, waylandMonitors[0].wlOutput.waylandObject);
									return true;
								} else
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
				} else
					RE_ERROR("Failed to add a listener to the Wayland surface ", wl_pSurface);
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
		actualWindowSize.fill(0);
	}

	static bool init_wayland_input() {
		PRINT_DEBUG("Creating an XKB context");
		if ((xkb_pContext = xkb_context_new(XKB_CONTEXT_NO_FLAGS))) {
			PRINT_DEBUG("Adding listener to the Wayland seat ", wlSeat.waylandObject);
			if (wl_seat_add_listener(wlSeat.waylandObject, &wl_seatListener, nullptr) == 0)
				return true;
			else
				RE_ERROR("Failed to add a listener to the Wayland seat ", wlSeat.waylandObject);
		} else
			RE_ERROR("Failed to create an XKB context");
		return false;
	}

	static void destroy_wayland_input() {
		PRINT_DEBUG("Destroying input-related resources");
		if (waylandKeyboard.wl_pKeyboard)
			destroy_wayland_keyboard();
		if (wl_pPointer)
			destroy_wayland_pointer();
		xkb_context_unref(xkb_pContext);
		actualCursorPosition.fill(0);
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
