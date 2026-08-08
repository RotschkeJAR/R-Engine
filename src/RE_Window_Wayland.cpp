#include "RE_Window_Wayland.hpp"
#include "RE_Renderer.hpp"
#include "RE_Asset.hpp"
#include "RE_Settings.hpp"
#include "RE_KeycodeTranslator.hpp"

#ifdef RE_OS_LINUX

#include <dlfcn.h>
#include <linux/input-event-codes.h>
#include <sys/mman.h>
#include <fcntl.h>

namespace RE {

#define REQUIRED_VERSION_WL_COMPOSITOR       static_cast<uint32_t>(std::max(WL_COMPOSITOR_CREATE_SURFACE_SINCE_VERSION, WL_COMPOSITOR_CREATE_REGION_SINCE_VERSION))
#define REQUIRED_VERSION_XDG_WM_BASE         static_cast<uint32_t>(std::max(XDG_WM_BASE_PING_SINCE_VERSION, std::max(XDG_WM_BASE_DESTROY_SINCE_VERSION, std::max(XDG_WM_BASE_GET_XDG_SURFACE_SINCE_VERSION, XDG_WM_BASE_PONG_SINCE_VERSION))))
#define REQUIRED_VERSION_WL_SEAT             static_cast<uint32_t>(std::max(WL_SEAT_CAPABILITIES_SINCE_VERSION, std::max(WL_SEAT_GET_POINTER_SINCE_VERSION, WL_SEAT_GET_KEYBOARD_SINCE_VERSION)))
#define REQUIRED_VERSION_WL_OUTPUT           static_cast<uint32_t>(std::max(WL_OUTPUT_GEOMETRY_SINCE_VERSION, std::max(WL_OUTPUT_MODE_SINCE_VERSION, WL_OUTPUT_SCALE_SINCE_VERSION)))
#define REQUIRED_VERSION_WL_SHM              static_cast<uint32_t>(std::max(WL_SHM_FORMAT_SINCE_VERSION, WL_SHM_CREATE_POOL_SINCE_VERSION))

#define KEYCODE_TO_XKB_OFFSET   8

#define EDGE_EXTENSION     20

#define CURSOR_TEXTURE_SIZE     41
#define CURSOR_TEXTURE_COUNT    10

#define CURSOR_DEFAULT        0
#define CURSOR_TEXT           1
#define CURSOR_CROSSHAIR      2
#define CURSOR_HORIZONTAL     3
#define CURSOR_VERICAL        4
#define CURSOR_DIAGONAL_LTRB  5
#define CURSOR_DIAGONAL_LBRT  6
#define CURSOR_POINT          7
#define CURSOR_OPEN           8
#define CURSOR_GRAB           9

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
		int32_t i32Scale;
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
	static wl_shm *wl_pShm = nullptr;
	wl_surface *wl_pSurface;
	static wl_output *wl_pCurrentOutput = nullptr;
	static xdg_surface *xdg_pSurface;
	static xdg_toplevel *xdg_pToplevel;
	static std::vector<WaylandMonitorInfo> waylandMonitors;
	static xkb_context *xkb_pContext;
	static wl_pointer *wl_pPointer = nullptr;
	static wl_surface *wl_pCursorSurface;
	static wl_shm_pool *wl_pCursorImagePool;
	static wl_buffer *wl_apCursorImageBuffers[CURSOR_TEXTURE_COUNT];
	static void *pCursorImage;
	static WaylandKeyboardInfo waylandKeyboard = {};
	static Vector2i actualCursorPosition,
		actualWindowSize;
	static int iCursorImageFd,
		iLastCursorImage;

	static WindowArea get_wayland_window_area_cursor_is_in() {
		if ((mSettingsFlags & SETTINGS_FLAG_FULLSCREEN_BIT))
			return WINDOW_AREA_CONTENT;
		const int iWindowBorderWidth = (mWindowFlagBits & WINDOW_FLAG_MAXIMIZED_BIT) ? 0 : WINDOW_BORDER_TOTAL_SIZE;
		if (actualCursorPosition[0] < iWindowBorderWidth) {
			if (actualCursorPosition[1] < iWindowBorderWidth + EDGE_EXTENSION)
				return WINDOW_AREA_TOP_LEFT;
			else if (actualCursorPosition[1] >= actualWindowSize[1] - iWindowBorderWidth - EDGE_EXTENSION)
				return WINDOW_AREA_BOTTOM_LEFT;
			return WINDOW_AREA_LEFT;
		} else if (actualCursorPosition[0] >= actualWindowSize[0] - iWindowBorderWidth) {
			if (actualCursorPosition[1] < iWindowBorderWidth + EDGE_EXTENSION)
				return WINDOW_AREA_TOP_RIGHT;
			else if (actualCursorPosition[1] >= actualWindowSize[1] - iWindowBorderWidth - EDGE_EXTENSION)
				return WINDOW_AREA_BOTTOM_RIGHT;
			return WINDOW_AREA_RIGHT;
		} else if (actualCursorPosition[1] < iWindowBorderWidth) {
			if (actualCursorPosition[0] < iWindowBorderWidth + EDGE_EXTENSION)
				return WINDOW_AREA_TOP_LEFT;
			else if (actualCursorPosition[0] >= actualWindowSize[0] - iWindowBorderWidth - EDGE_EXTENSION)
				return WINDOW_AREA_TOP_RIGHT;
			return WINDOW_AREA_TOP;
		} else if (actualCursorPosition[1] >= actualWindowSize[1] - iWindowBorderWidth) {
			if (actualCursorPosition[0] < iWindowBorderWidth + EDGE_EXTENSION)
				return WINDOW_AREA_BOTTOM_LEFT;
			else if (actualCursorPosition[0] >= actualWindowSize[0] - iWindowBorderWidth - EDGE_EXTENSION)
				return WINDOW_AREA_BOTTOM_RIGHT;
			return WINDOW_AREA_BOTTOM;
		} else if (actualCursorPosition[1] < iWindowBorderWidth + WINDOW_BAR_SIZE) {
			if (actualCursorPosition[0] >= actualWindowSize[0] - iWindowBorderWidth - WINDOW_BUTTON_WIDTH)
				return WINDOW_AREA_BUTTON_CLOSE;
			else if (actualCursorPosition[0] >= actualWindowSize[0] - iWindowBorderWidth - WINDOW_BUTTON_WIDTH * 2)
				return WINDOW_AREA_BUTTON_MAXIMIZE;
			else if (actualCursorPosition[0] >= actualWindowSize[0] - iWindowBorderWidth - WINDOW_BUTTON_WIDTH * 3)
				return WINDOW_AREA_BUTTON_MINIMIZE;
			return WINDOW_AREA_BAR;
		} else
			return WINDOW_AREA_CONTENT;
	}

	static void destroy_wayland_pointer() {
		PRINT_DEBUG("Destroying Wayland pointer ", wl_pPointer);
		if (wl_pointer_get_version(wl_pPointer) >= static_cast<uint32_t>(WL_POINTER_RELEASE_SINCE_VERSION))
			wl_pointer_release(wl_pPointer);
		else
			wl_pointer_destroy(wl_pPointer);
		wl_pPointer = nullptr;
	}

	static void destroy_wayland_keyboard() {
		PRINT_DEBUG("Destroying Wayland keyboard ", waylandKeyboard.wl_pKeyboard);
		if (wl_keyboard_get_version(waylandKeyboard.wl_pKeyboard) >= static_cast<uint32_t>(WL_KEYBOARD_RELEASE_SINCE_VERSION))
			wl_keyboard_release(waylandKeyboard.wl_pKeyboard);
		else
			wl_keyboard_destroy(waylandKeyboard.wl_pKeyboard);
		waylandKeyboard.wl_pKeyboard = nullptr;
	}

	static void wayland_registry_add_callback(void *pData, wl_registry *wl_pRegistry, uint32_t u32Name, const char *pacInterface, uint32_t u32Version) {
		if (are_string_contents_equal(pacInterface, wl_compositor_interface.name) && !wlCompositor.waylandObject) {
			if (u32Version >= REQUIRED_VERSION_WL_COMPOSITOR) {
				PRINT_DEBUG("Binding the Wayland compositor");
				wlCompositor.waylandObject = reinterpret_cast<wl_compositor*>(
						wl_registry_bind(
							wl_pRegistry,
							u32Name,
							&wl_compositor_interface,
							REQUIRED_VERSION_WL_COMPOSITOR));
				wlCompositor.u32Name = u32Name;
				wlCompositor.u32Version = u32Version;
			} else
				RE_ERROR("The version of Wayland compositor is too low (", u32Version, " < ", REQUIRED_VERSION_WL_COMPOSITOR, ")");
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
				PRINT_DEBUG("Binding the Wayland seat");
				wlSeat.waylandObject = reinterpret_cast<wl_seat*>(
						wl_registry_bind(
							wl_pRegistry,
							u32Name,
							&wl_seat_interface,
							REQUIRED_VERSION_WL_SEAT));
				wlSeat.u32Name = u32Name;
				wlSeat.u32Version = u32Version;
			} else
				RE_ERROR("The version of Wayland seat is too low (", u32Version, " < ", REQUIRED_VERSION_WL_SEAT, ")");
		} else if (are_string_contents_equal(pacInterface, wl_output_interface.name)) {
			if (u32Version >= REQUIRED_VERSION_WL_OUTPUT) {
				PRINT_DEBUG("Binding a Wayland output");
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
						Vector2u {},
						1);
			} else
				RE_ERROR("The version of Wayland output is too low (", u32Version, " < ", REQUIRED_VERSION_WL_OUTPUT, ")");
		} else if (are_string_contents_equal(pacInterface, wl_shm_interface.name)) {
			if (u32Version >= REQUIRED_VERSION_WL_SHM) {
				PRINT_DEBUG("Binding a Wayland shared memory");
				wl_pShm = reinterpret_cast<wl_shm*>(
								wl_registry_bind(
									wl_pRegistry,
									u32Name,
									&wl_shm_interface,
									REQUIRED_VERSION_WL_SHM));
			} else
				RE_ERROR("The version of Wayland shared memory is too low (", u32Version, " < ", REQUIRED_VERSION_WL_SHM, ")");
		}
	}

	static void wayland_registry_remove_callback(void *pData, wl_registry *wl_pRegistry, uint32_t u32Name) {
		for (auto xWaylandMonitorIter = waylandMonitors.begin(); xWaylandMonitorIter != waylandMonitors.end(); xWaylandMonitorIter++) {
			if (xWaylandMonitorIter->wlOutput.u32Name != u32Name)
				continue;
			PRINT_DEBUG("Removing the Wayland output object ", xWaylandMonitorIter->wlOutput.waylandObject);
		#ifdef WL_OUTPUT_RELEASE_SINCE_VERSION
			if (xWaylandMonitorIter->wlOutput.u32Version >= static_cast<uint32_t>(WL_OUTPUT_RELEASE_SINCE_VERSION))
				wl_output_release(xWaylandMonitorIter->wlOutput.waylandObject);
			else
				wl_output_destroy(xWaylandMonitorIter->wlOutput.waylandObject);
		#else
			wl_output_destroy(xWaylandMonitorIter->wlOutput.waylandObject);
		#endif
			waylandMonitors.erase(xWaylandMonitorIter);
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

	static void wayland_output_geometry_callback(void *pData, wl_output *wl_pOutput, int32_t i32X, int32_t i32Y, int32_t i32PhysicalWidth, int32_t i32PhysicalHeight, int32_t i32Subpixel, const char *pacMake, const char *pacModel, int32_t i32Transform) {
	}

	static void wayland_output_mode_callback(void *pData, wl_output *wl_pOutput, uint32_t u32Flags, int32_t i32Width, int32_t i32Height, int32_t i32RefreshRate) {
		largestMonitorSize[0] = std::max<uint32_t>(largestMonitorSize[0], i32Width);
		largestMonitorSize[1] = std::max<uint32_t>(largestMonitorSize[1], i32Height);
		auto xWaylandMonitorIter = std::find_if(
				waylandMonitors.begin(),
				waylandMonitors.end(),
				[&](const WaylandMonitorInfo &rMonitorInfo) {
					return rMonitorInfo.wlOutput.waylandObject == wl_pOutput;
				});
		xWaylandMonitorIter->size[0] = static_cast<uint32_t>(i32Width);
		xWaylandMonitorIter->size[1] = static_cast<uint32_t>(i32Height);
	}

	static void wayland_output_done_callback(void *pData, wl_output *wl_pOutput) {
	}

	static void wayland_output_scale_callback(void *pData, wl_output *wl_pOutput, int32_t i32Factor) {
		auto xWaylandMonitorIter = std::find_if(
				waylandMonitors.begin(),
				waylandMonitors.end(),
				[&](const WaylandMonitorInfo &rMonitorInfo) {
					return rMonitorInfo.wlOutput.waylandObject == wl_pOutput;
				});
		xWaylandMonitorIter->i32Scale = i32Factor;
	}

	static constexpr wl_output_listener wl_outputListener = {
		.geometry = wayland_output_geometry_callback,
		.mode = wayland_output_mode_callback,
		.done = wayland_output_done_callback,
		.scale = wayland_output_scale_callback
	};

	static void xdg_wm_base_ping(void *pData, xdg_wm_base *xdg_pWmBase, uint32_t u32Serial) {
		xdg_wm_base_pong(xdg_pWmBase, u32Serial);
	}

	static constexpr xdg_wm_base_listener xdg_wmBasePinger = {
		.ping = xdg_wm_base_ping
	};

	static void wayland_surface_enter_callback(void *pData, wl_surface *wl_pSurface, wl_output *wl_pNewOutput) {
		if (wl_pCurrentOutput != wl_pNewOutput)
			wl_pCurrentOutput = wl_pNewOutput;
	}

	static void wayland_surface_leave_callback(void *pData, wl_surface *wl_pSurface, wl_output *wl_pOldOutput) {
		if (wl_pCurrentOutput == wl_pOldOutput)
			wl_pCurrentOutput = nullptr;
	}

	static void wayland_surface_preferred_buffer_scale_callback(void *pData, wl_surface *wl_pSurface, int32_t i32Factor) {
		PRINT_DEBUG("Setting buffer scale of Wayland surface ", wl_pSurface, " to ", i32Factor);
		wl_surface_set_buffer_scale(wl_pSurface, i32Factor);
		wl_surface_commit(wl_pSurface);
	}

	static void wayland_surface_preferred_buffer_transform_callback(void *pData, wl_surface *wl_pSurface, uint32_t u32Transform) {
	}

	static constexpr wl_surface_listener wl_surfaceListener {
		.enter = wayland_surface_enter_callback,
		.leave = wayland_surface_leave_callback,
		.preferred_buffer_scale = wayland_surface_preferred_buffer_scale_callback,
		.preferred_buffer_transform = wayland_surface_preferred_buffer_transform_callback
	};

	static void xdg_surface_configure_callback(void *pData, xdg_surface *xdg_pSurface, uint32_t u32Serial) {
		xdg_surface_ack_configure(xdg_pSurface, u32Serial);
		wl_display_flush(wl_pDisplay);
	}

	static constexpr xdg_surface_listener xdg_surfaceListener = {
		.configure = xdg_surface_configure_callback
	};

	static void xdg_toplevel_configure_callback(void *pData, xdg_toplevel *xdg_pToplevel, int32_t i32Width, int32_t i32Height, wl_array *wl_pStates) {
		bool bMaximized = false,
			bFullscreen = false;
		for (xdg_toplevel_state *xdg_pToplevelState = static_cast<xdg_toplevel_state*>(wl_pStates->data);
				xdg_pToplevelState != (static_cast<xdg_toplevel_state*>(wl_pStates->data) + wl_pStates->size / sizeof(xdg_toplevel_state));
				xdg_pToplevelState++) {
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
		set_bitmasks(mWindowFlagBits, bMaximized, static_cast<WindowFlags>(WINDOW_FLAG_MAXIMIZED_BIT));
		set_bitmasks(mSettingsFlags, bFullscreen, static_cast<SettingsFlags_t>(SETTINGS_FLAG_FULLSCREEN_BIT));
		
		if (bFullscreen && !waylandMonitors.empty()) {
			bool bMonitorFound = false;
			for (WaylandMonitorInfo &rMonitorInfo : waylandMonitors) {
				if (wl_pCurrentOutput == rMonitorInfo.wlOutput.waylandObject) {
					i32Width = rMonitorInfo.size[0];
					i32Height = rMonitorInfo.size[1];
					bMonitorFound = true;
					break;
				}
			}
			if (!bMonitorFound)
				goto REGULAR_WINDOW_SIZE_SETUP;
		} else {
		REGULAR_WINDOW_SIZE_SETUP:
			if (i32Width <= 0)
				i32Width = largestMonitorSize[0] / 5 * 3 + WINDOW_EXTRA_WIDTH;
			if (i32Height <= 0)
				i32Height = largestMonitorSize[1] / 5 * 3 + WINDOW_EXTRA_HEIGHT;
		}
		actualWindowSize[0] = i32Width;
		actualWindowSize[1] = i32Height;
		if (should_render_window_frame_edges()) {
			i32Width -= WINDOW_EXTRA_WIDTH;
			i32Height -= WINDOW_BORDER_TOTAL_SIZE * 2;
		}
		if (should_render_window_frame_bar())
			i32Height -= WINDOW_BAR_SIZE;
		window_resize_event(static_cast<uint32_t>(i32Width), static_cast<uint32_t>(i32Height));

		PRINT_DEBUG("Updating regions on Wayland surface ", wl_pSurface);
		wl_region *const wl_pRegion = wl_compositor_create_region(wlCompositor.waylandObject);
		if (!bFullscreen && !bMaximized)
			wl_region_add(wl_pRegion, WINDOW_SHADOW_SIZE, WINDOW_SHADOW_SIZE, actualWindowSize[0] - WINDOW_SHADOW_SIZE * 2, actualWindowSize[1] - WINDOW_SHADOW_SIZE * 2);
		else
			wl_region_add(wl_pRegion, 0, 0, actualWindowSize[0], actualWindowSize[1]);
		wl_surface_set_opaque_region(wl_pSurface, wl_pRegion);
		wl_region_destroy(wl_pRegion);
	}

	static void xdg_toplevel_close_callback(void *pData, xdg_toplevel *xdg_pToplevel) {
		mWindowFlagBits |= WINDOW_FLAG_CLOSE_BIT;
	}

	static constexpr xdg_toplevel_listener xdg_toplevelListener = {
		.configure = xdg_toplevel_configure_callback,
		.close = xdg_toplevel_close_callback,
		.configure_bounds = nullptr,
		.wm_capabilities = nullptr
	};

	static void wayland_pointer_enter_callback(void *pData, wl_pointer *wl_pPointer, uint32_t u32Serial, wl_surface *wl_pSurface, wl_fixed_t wl_x, wl_fixed_t wl_y) {
		PRINT_DEBUG("Setting the Wayland surface ", wl_pCursorSurface, " for the cursor ", wl_pPointer);
		wl_surface_damage(
				wl_pCursorSurface,
				0,
				0,
				CURSOR_TEXTURE_SIZE,
				CURSOR_TEXTURE_SIZE);
		wl_surface_commit(wl_pCursorSurface);
		wl_pointer_set_cursor(
				wl_pPointer,
				u32Serial,
				wl_pCursorSurface,
				CURSOR_TEXTURE_SIZE / 2 + 1,
				CURSOR_TEXTURE_SIZE / 2 + 1);
		mWindowFlagBits &= ~WINDOW_FLAG_MINIMIZED_BIT;
	}

	static void wayland_pointer_leave_callback(void *pData, wl_pointer *wl_pPointer, uint32_t u32Serial, wl_surface *wl_pSurface) {
	}

	static void wayland_pointer_motion_callback(void *pData, wl_pointer *wl_pPointer, uint32_t u32Time, wl_fixed_t wl_x, wl_fixed_t wl_y) {
		actualCursorPosition[0] = wl_fixed_to_int(wl_x);
		actualCursorPosition[1] = wl_fixed_to_int(wl_y);
		const WindowArea eHoveredWindowArea = get_wayland_window_area_cursor_is_in();
		if (pWindowFrameUniformData) {
			pWindowFrameUniformData->u32HoveredWindowAreaIndex = static_cast<uint32_t>(eHoveredWindowArea);
		}
		int iNewCursorImage;
		switch (eHoveredWindowArea) {
			case WINDOW_AREA_TOP_LEFT:
			case WINDOW_AREA_BOTTOM_RIGHT:
				iNewCursorImage = CURSOR_DIAGONAL_LTRB;
				break;
			case WINDOW_AREA_TOP_RIGHT:
			case WINDOW_AREA_BOTTOM_LEFT:
				iNewCursorImage = CURSOR_DIAGONAL_LBRT;
				break;
			case WINDOW_AREA_LEFT:
			case WINDOW_AREA_RIGHT:
				iNewCursorImage = CURSOR_HORIZONTAL;
				break;
			case WINDOW_AREA_TOP:
			case WINDOW_AREA_BOTTOM:
				iNewCursorImage = CURSOR_VERICAL;
				break;
			case WINDOW_AREA_BUTTON_CLOSE:
			case WINDOW_AREA_BUTTON_MAXIMIZE:
			case WINDOW_AREA_BUTTON_MINIMIZE:
				iNewCursorImage = CURSOR_POINT;
				break;
			case WINDOW_AREA_BAR:
				iNewCursorImage = CURSOR_DEFAULT;
				break;
			case WINDOW_AREA_CONTENT:
				iNewCursorImage = CURSOR_DEFAULT;
				cursor_event(actualCursorPosition[0] - WINDOW_X_OFFSET, actualCursorPosition[1] - WINDOW_Y_OFFSET);
				break;
			default:
				break;
		}
		if (iNewCursorImage != iLastCursorImage) {
			wl_surface_attach(wl_pCursorSurface, wl_apCursorImageBuffers[iNewCursorImage], 0, 0);
			wl_surface_damage(
					wl_pCursorSurface,
					0,
					0,
					CURSOR_TEXTURE_SIZE,
					CURSOR_TEXTURE_SIZE);
			wl_surface_commit(wl_pCursorSurface);
			iLastCursorImage = iNewCursorImage;
		}
	}

	static void wayland_pointer_button_callback(void *pData, wl_pointer *wl_pPointer, uint32_t u32Serial, uint32_t u32Time, uint32_t u32Button, uint32_t u32State) {
		const WindowArea eWindowAreaOfCursor = get_wayland_window_area_cursor_is_in();
		if (eWindowAreaOfCursor != WINDOW_AREA_CONTENT) {
			if (u32State != WL_POINTER_BUTTON_STATE_PRESSED)
				return;
			if (u32Button == BTN_LEFT) {
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
						mWindowFlagBits |= WINDOW_FLAG_CLOSE_BIT;
						break;
					case WINDOW_AREA_BUTTON_MAXIMIZE:
						{
							const bool bMaximized = !(mWindowFlagBits & WINDOW_FLAG_MAXIMIZED_BIT);
							set_bitmasks(mWindowFlagBits, bMaximized, static_cast<WindowFlags>(WINDOW_FLAG_MAXIMIZED_BIT));
							if (bMaximized)
								xdg_toplevel_set_maximized(xdg_pToplevel);
							else
								xdg_toplevel_unset_maximized(xdg_pToplevel);
						}
						break;
					case WINDOW_AREA_BUTTON_MINIMIZE:
						xdg_toplevel_set_minimized(xdg_pToplevel);
						prepare_render_tasks_for_dummy_presentation();
						mWindowFlagBits |= WINDOW_FLAG_MINIMIZED_BIT;
						if (pWindowFrameUniformData) {
							for (uint uDimensionIndex = 0; uDimensionIndex < actualCursorPosition.dimensions(); uDimensionIndex++) {
								actualCursorPosition[uDimensionIndex] = 0xFFFFFFFFU;
							}
							pWindowFrameUniformData->u32HoveredWindowAreaIndex = static_cast<uint32_t>(WINDOW_AREA_NONE);
						}
						break;
					default:
						xdg_toplevel_move(xdg_pToplevel, wlSeat.waylandObject, u32Serial);
						break;
				}
			} else if (u32Button == BTN_RIGHT) {
				if (eWindowAreaOfCursor == WINDOW_AREA_BAR)
					xdg_toplevel_show_window_menu(xdg_pToplevel, wlSeat.waylandObject, u32Serial, actualCursorPosition[0], actualCursorPosition[1]);
			} else
				return;
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

	static void wayland_pointer_scroll_callback(void *pData, wl_pointer *wl_pPointer, uint32_t u32Time, uint32_t u32Axis, wl_fixed_t wl_value) {
		if (u32Axis == WL_POINTER_AXIS_VERTICAL_SCROLL && get_wayland_window_area_cursor_is_in() == WINDOW_AREA_CONTENT)
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

	static void wayland_keyboard_keymap_callback(void *pData, wl_keyboard *wl_pKeyboard, uint32_t u32Format, int32_t i32FileDescriptor, uint32_t u32Size) {
		if (waylandKeyboard.wl_pKeyboard != wl_pKeyboard)
			return;
		switch (u32Format) {
			case WL_KEYBOARD_KEYMAP_FORMAT_XKB_V1:
				{
					PRINT_DEBUG("Mapping keymap buffer from file descriptor ", i32FileDescriptor);
					char *const pacKeymapBuffer = static_cast<char*>(mmap(nullptr, u32Size, PROT_READ, MAP_SHARED, i32FileDescriptor, 0));
					if (pacKeymapBuffer) {
						PRINT_DEBUG("Creating XKB keymap from mapped keymap buffer");
						if ((waylandKeyboard.xkb_pKeymap = xkb_keymap_new_from_buffer(xkb_pContext, pacKeymapBuffer, u32Size, XKB_KEYMAP_FORMAT_TEXT_V1, XKB_KEYMAP_COMPILE_NO_FLAGS))) {
							PRINT_DEBUG("Creating XKB state from keymap");
							if ((waylandKeyboard.xkb_pState = xkb_state_new(waylandKeyboard.xkb_pKeymap))) {
								PRINT_DEBUG("Unmapping keymap buffer");
								munmap(pacKeymapBuffer, u32Size);
								return;
							} else
								RE_ERROR("Failed to create an XKB state from the keymap");
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

	static void wayland_keyboard_enter_callback(void *pData, wl_keyboard *wl_pKeyboard, uint32_t u32Serial, wl_surface *wl_pSurface, wl_array *wl_pKeys) {
		mWindowFlagBits &= ~WINDOW_FLAG_MINIMIZED_BIT;
	}

	static void wayland_keyboard_leave_callback(void *pData, wl_keyboard *wl_pKeyboard, uint32_t u32Serial, wl_surface *wl_pSurface) {
	}

	static void wayland_keyboard_key_callback(void *pData, wl_keyboard *wl_pKeyboard, uint32_t u32Serial, uint32_t u32Time, uint32_t u32Key, uint32_t u32State) {
		const xkb_keysym_t xkb_keySym = xkb_state_key_get_one_sym(waylandKeyboard.xkb_pState, u32Key + KEYCODE_TO_XKB_OFFSET);
		if (xkb_keySym == XKB_KEY_Super_L || xkb_keySym == XKB_KEY_Super_R)
			return;
		if ((mSettingsFlags & SETTINGS_FLAG_MENU_OPEN_BIT)) {
			settings_handle_keyboard_input(xkb_keySym);
		} else {
			const bool bKeyPressed = u32State == WL_KEYBOARD_KEY_STATE_PRESSED;
			if ((mSettingsFlags & SETTINGS_FLAG_MENU_OPEN_BIT)) {
				settings_handle_keyboard_input(xkb_keySym);
			} else {
				switch (xkb_keySym) {
					case XKB_KEY_F1:
						shortcut_settings_f1(bKeyPressed);
						break;
					case XKB_KEY_F2:
						shortcut_settings_f2(bKeyPressed);
						break;
					case XKB_KEY_F3:
						shortcut_settings_f3(bKeyPressed);
						break;
					case XKB_KEY_F4:
						shortcut_settings_f4(bKeyPressed);
						break;
					case XKB_KEY_F5:
						shortcut_settings_f5(bKeyPressed);
						break;
					case XKB_KEY_F6:
						shortcut_settings_f6(bKeyPressed);
						break;
					case XKB_KEY_F7:
						shortcut_settings_f7(bKeyPressed);
						break;
					case XKB_KEY_F8:
						shortcut_settings_f8(bKeyPressed);
						break;
					case XKB_KEY_F9:
						shortcut_settings_f9(bKeyPressed);
						break;
					case XKB_KEY_F10:
						shortcut_settings_f10(bKeyPressed);
						break;
					case XKB_KEY_F11:
						shortcut_settings_f11(bKeyPressed);
						break;
					case XKB_KEY_F12:
						shortcut_settings_f12(bKeyPressed);
						break;
					default:
						input_event(key_from_virtual_xkb_keysym(xkb_keySym), u32Key, bKeyPressed, false);
						break;
				}
			}
		}
	}

	static void wayland_keyboard_modifiers_callback(void *pData, wl_keyboard *wl_pKeyboard, uint32_t u32Serial, uint32_t u32ModsDepressed, uint32_t u32ModsLatched, uint32_t u32ModsLocked, uint32_t u32Group) {
	}

	static void wayland_keyboard_repeat_info_callback(void *pData, wl_keyboard *wl_pKeyboard, int32_t i32Rate, int32_t i32Delay) {
	}

	static constexpr wl_keyboard_listener wl_keyboardListener = {
		.keymap = wayland_keyboard_keymap_callback,
		.enter = wayland_keyboard_enter_callback,
		.leave = wayland_keyboard_leave_callback,
		.key = wayland_keyboard_key_callback,
		.modifiers = wayland_keyboard_modifiers_callback,
		.repeat_info = wayland_keyboard_repeat_info_callback
	};

	static void wayland_seat_capabilities_callback(void *pData, wl_seat *wl_pSeat, uint32_t m32Capabilities) {
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
			if (wl_pPointer)
				destroy_wayland_pointer();
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
				if (wlCompositor.waylandObject && xdgWmBase.waylandObject && wlSeat.waylandObject && !waylandMonitors.empty() && wl_pShm) {
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
				#ifdef WL_OUTPUT_RELEASE_SINCE_VERSION
					if (wl_output_get_version(rMonitorInfo.wlOutput.waylandObject) >= static_cast<uint32_t>(WL_OUTPUT_RELEASE_SINCE_VERSION))
						wl_output_release(rMonitorInfo.wlOutput.waylandObject);
					else
						wl_output_destroy(rMonitorInfo.wlOutput.waylandObject);
				#else
					wl_output_destroy(rMonitorInfo.wlOutput.waylandObject);
				#endif
				}
				waylandMonitors.clear();
				if (wlSeat.waylandObject) {
					PRINT_DEBUG("Destroying seat ", wlSeat.waylandObject, " due to failure creating the Wayland window");
				#ifdef WL_SEAT_RELEASE_SINCE_VERSION
					if (wl_seat_get_version(wlSeat.waylandObject) >= static_cast<uint32_t>(WL_SEAT_RELEASE_SINCE_VERSION))
						wl_seat_release(wlSeat.waylandObject);
					else
						wl_seat_destroy(wlSeat.waylandObject);
				#else
					wl_seat_destroy(wlSeat.waylandObject);
				#endif
					wlSeat.waylandObject = nullptr;
				}
				if (xdgWmBase.waylandObject) {
					PRINT_DEBUG("Destroying XDG wm base ", xdgWmBase.waylandObject, " due to failure obtaining all essential Wayland global objects");
					xdg_wm_base_destroy(xdgWmBase.waylandObject);
					xdgWmBase.waylandObject = nullptr;
				}
				if (wlCompositor.waylandObject) {
					PRINT_DEBUG("Destroying compositor ", wlCompositor.waylandObject, " due to failure obtaining all essential Wayland global objects");
					wl_compositor_destroy(wlCompositor.waylandObject);
					wlCompositor.waylandObject = nullptr;
				}
				if (wl_pShm) {
					PRINT_DEBUG("Releasing shared memory object ", wl_pShm, " due to failure obtaining all essential Wayland global objects");
				#ifdef WL_SHM_RELEASE_SINCE_VERSION
					if (wl_shm_get_version(wl_pShm) >= static_cast<uint32_t>(WL_SHM_RELEASE_SINCE_VERSION))
						wl_shm_release(wl_pShm);
					else
						wl_shm_destroy(wl_pShm);
				#else
					wl_shm_destroy(wl_pShm);
				#endif
					wl_pShm = nullptr;
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
		#ifdef WL_OUTPUT_RELEASE_SINCE_VERSION
			if (wl_output_get_version(rMonitorInfo.wlOutput.waylandObject) >= static_cast<uint32_t>(WL_OUTPUT_RELEASE_SINCE_VERSION))
				wl_output_release(rMonitorInfo.wlOutput.waylandObject);
			else
				wl_output_destroy(rMonitorInfo.wlOutput.waylandObject);
		#else
			wl_output_destroy(rMonitorInfo.wlOutput.waylandObject);
		#endif
		}
		waylandMonitors.clear();
		PRINT_DEBUG("Destroying seat ", wlSeat.waylandObject);
	#ifdef WL_SEAT_RELEASE_SINCE_VERSION
		if (wl_seat_get_version(wlSeat.waylandObject) >= static_cast<uint32_t>(WL_SEAT_RELEASE_SINCE_VERSION))
			wl_seat_release(wlSeat.waylandObject);
		else
			wl_seat_destroy(wlSeat.waylandObject);
	#else
		wl_seat_destroy(wlSeat.waylandObject);
	#endif
		wlSeat.waylandObject = nullptr;
		PRINT_DEBUG("Destroying XDG wm base ", xdgWmBase.waylandObject);
		xdg_wm_base_destroy(xdgWmBase.waylandObject);
		xdgWmBase.waylandObject = nullptr;
		PRINT_DEBUG("Destroying compositor ", wlCompositor.waylandObject);
		wl_compositor_destroy(wlCompositor.waylandObject);
		wlCompositor.waylandObject = nullptr;
		PRINT_DEBUG("Releasing shared memory object ", wl_pShm);
	#ifdef WL_SHM_RELEASE_SINCE_VERSION
		if (wl_shm_get_version(wl_pShm) >= static_cast<uint32_t>(WL_SHM_RELEASE_SINCE_VERSION))
			wl_shm_release(wl_pShm);
		else
			wl_shm_destroy(wl_pShm);
	#else
		wl_shm_destroy(wl_pShm);
	#endif
		wl_pShm = nullptr;
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
									xdg_toplevel_set_min_size(xdg_pToplevel, MIN_WINDOW_WIDTH + WINDOW_EXTRA_WIDTH, MIN_WINDOW_HEIGHT + WINDOW_EXTRA_HEIGHT);
									if (!(mSettingsFlags & SETTINGS_FLAG_FULLSCREEN_BIT))
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
						PRINT_DEBUG("Destroying XDG surface ", xdg_pSurface, " due to failure creating the window");
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
			if (wl_seat_add_listener(wlSeat.waylandObject, &wl_seatListener, nullptr) == 0) {
				if ((wl_pCursorSurface = wl_compositor_create_surface(wlCompositor.waylandObject))) {
					constexpr size_t sCursorBufferSize = sizeof(uint32_t) * CURSOR_TEXTURE_SIZE * CURSOR_TEXTURE_SIZE * CURSOR_TEXTURE_COUNT;
					PRINT_DEBUG("Allocating file for cursor images");
					iCursorImageFd = memfd_create("wayland_cursors", MFD_CLOEXEC);
					if (iCursorImageFd != -1) {
						PRINT_DEBUG("Resizing file descriptor ", iCursorImageFd, " to ", sCursorBufferSize, " bytes");
						if (ftruncate(iCursorImageFd, sCursorBufferSize) == 0) {
							PRINT_DEBUG("Mapping file descriptor ", iCursorImageFd, " to RAM");
							pCursorImage = mmap(
									nullptr,
									sCursorBufferSize,
									PROT_READ | PROT_WRITE,
									MAP_SHARED,
									iCursorImageFd,
									0);
							if (pCursorImage != MAP_FAILED) {
								PRINT_DEBUG("Creating Wayland shared memory pool with file descriptor ", iCursorImageFd);
								if ((wl_pCursorImagePool = wl_shm_create_pool(wl_pShm, iCursorImageFd, static_cast<int32_t>(sCursorBufferSize)))) {
									int iCursorImageBufferIndex;
									for (iCursorImageBufferIndex = 0; iCursorImageBufferIndex < CURSOR_TEXTURE_COUNT; iCursorImageBufferIndex++) {
										PRINT_DEBUG("Creating Wayland buffers from shared memory pool ", wl_pCursorImagePool, " at index ", iCursorImageBufferIndex);
										wl_apCursorImageBuffers[iCursorImageBufferIndex] = wl_shm_pool_create_buffer(
												wl_pCursorImagePool,
												(sizeof(uint32_t) * CURSOR_TEXTURE_SIZE * CURSOR_TEXTURE_SIZE) * iCursorImageBufferIndex,
												CURSOR_TEXTURE_SIZE,
												CURSOR_TEXTURE_SIZE,
												sizeof(uint32_t) * CURSOR_TEXTURE_SIZE,
												WL_SHM_FORMAT_ARGB8888);
										if (!wl_apCursorImageBuffers[iCursorImageBufferIndex]) {
											RE_ERROR("Failed to create a Wayland buffer from shared memory pool ", wl_pCursorImagePool, " at index ", iCursorImageBufferIndex);
											break;
										}
									}
									if (iCursorImageBufferIndex == CURSOR_TEXTURE_COUNT) {
										PRINT_DEBUG("Attaching Wayland buffer ", wl_apCursorImageBuffers[CURSOR_DEFAULT], " to surface ", wl_pCursorSurface);
										wl_surface_attach(wl_pCursorSurface, wl_apCursorImageBuffers[CURSOR_DEFAULT], 0, 0);
										wl_surface_commit(wl_pCursorSurface);
										iLastCursorImage = CURSOR_DEFAULT;
										PRINT_DEBUG("Opening cursor image file");
										AssetImage cursorImage;
										if (asset_open("rsc/Linux/wayland_cursors.png", cursorImage.file)) {
											PRINT_DEBUG("Loading cursor image");
											if (asset_image_load(cursorImage, 4)) {
												PRINT_DEBUG("Closing cursor image file");
												asset_close(cursorImage.file);
												PRINT_DEBUG("Populating Wayland buffers with cursor images");
												std::memcpy(
														pCursorImage,
														cursorImage.pBuffer,
														sCursorBufferSize);
												PRINT_DEBUG("Freeing cursor image");
												asset_image_free(cursorImage);
												return true;
											} else {
												RE_ERROR("Failed to load cursor image");
												PRINT_DEBUG("Closing cursor image file due to failure loading its content");
												asset_close(cursorImage.file);
											}
										} else
											RE_ERROR("Failed to open cursor image");
									}
									while (iCursorImageBufferIndex > 0) {
										PRINT_DEBUG("Destroying Wayland buffer ", wl_apCursorImageBuffers[iCursorImageBufferIndex], " after failure creating all");
										wl_buffer_destroy(wl_apCursorImageBuffers[iCursorImageBufferIndex]);
										iCursorImageBufferIndex--;
									}
									PRINT_DEBUG("Destroying Wayland shared memory pool ", wl_pCursorImagePool, " used for cursor image buffers");
									wl_shm_pool_destroy(wl_pCursorImagePool);
								} else
									RE_ERROR("Failed to create a Wayland shared memory pool for cursor images");
								PRINT_DEBUG("Unmapping file descriptor ", iCursorImageFd, " used for cursor images");
								if (munmap(pCursorImage, sCursorBufferSize) != 0)
									RE_ERROR("Failed to unmap the file descriptor ", iCursorImageFd);
							} else
								RE_ERROR("Failed to map the file descriptor ", iCursorImageFd, " to RAM");
						} else
							RE_ERROR("Failed to resize file descriptor ", iCursorImageFd, " to ", sCursorBufferSize, " bytes");
						PRINT_DEBUG("Closing file descriptor ", iCursorImageFd, " used for cursor images");
						if (close(iCursorImageFd) != 0)
							RE_ERROR("Failed to close file descriptor");
					} else
						RE_ERROR("Failed to allocate file for cursor images");
					PRINT_DEBUG("Destroying Wayland surface ", wl_pCursorSurface, " used for the cursor");
					wl_surface_destroy(wl_pCursorSurface);
				} else
					RE_ERROR("Failed to create Wayland surface for cursor");
			} else
				RE_ERROR("Failed to add a listener to the Wayland seat ", wlSeat.waylandObject);
			PRINT_DEBUG("Destroying XKB context ", xkb_pContext);
			xkb_context_unref(xkb_pContext);
		} else
			RE_ERROR("Failed to create an XKB context");
		return false;
	}

	static void destroy_wayland_input() {
		PRINT_DEBUG("Destroying input-related resources");
		for (wl_buffer *const wl_pCursorBuffer : wl_apCursorImageBuffers)
			wl_buffer_destroy(wl_pCursorBuffer);
		wl_shm_pool_destroy(wl_pCursorImagePool);
		if (munmap(pCursorImage, sizeof(uint32_t) * CURSOR_TEXTURE_SIZE * CURSOR_TEXTURE_SIZE * CURSOR_TEXTURE_COUNT) != 0)
			RE_ERROR("Failed unmapping contents of file descriptor ", iCursorImageFd);
		if (close(iCursorImageFd) != 0)
			RE_ERROR("Failed to close file descriptor ", iCursorImageFd);
		wl_surface_destroy(wl_pCursorSurface);
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
						PRINT_DEBUG("Committing Wayland surface ", wl_pSurface, " and flushing display ", wl_pDisplay);
						wl_surface_commit(wl_pSurface);
						wl_display_flush(wl_pDisplay);
						return true;
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
		for (pIndirectDrawWindowTitle->instanceCount = 0;
				pIndirectDrawWindowTitle->instanceCount < sizeof(WindowFrameUniformData::au32TitleChars) / sizeof(WindowFrameUniformData::au32TitleChars[0]);
				pIndirectDrawWindowTitle->instanceCount++) {
			const uint32_t u32CharCode = static_cast<uint32_t>(pacWindowTitle[pIndirectDrawWindowTitle->instanceCount]);
			pWindowFrameUniformData->au32TitleChars[pIndirectDrawWindowTitle->instanceCount] = u32CharCode;
			if (u32CharCode == 0)
				break;
		}
	}

	void wayland_update_fullscreen() {
		if ((mSettingsFlags & SETTINGS_FLAG_FULLSCREEN_BIT)) {
			PRINT_DEBUG("Enabling fullscreen on Wayland window");
			xdg_toplevel_set_max_size(xdg_pToplevel, std::numeric_limits<int32_t>::max(), std::numeric_limits<int32_t>::max());
			if (!wl_pCurrentOutput)
				wl_pCurrentOutput = waylandMonitors[0].wlOutput.waylandObject;
			xdg_toplevel_set_fullscreen(xdg_pToplevel, wl_pCurrentOutput);
		} else {
			PRINT_DEBUG("Disabling fullscreen on Wayland window");
			xdg_toplevel_unset_fullscreen(xdg_pToplevel);
			xdg_toplevel_set_max_size(xdg_pToplevel, largestMonitorSize[0] + MAX_WINDOW_WIDTH_RELATIVE_TO_MONITOR, largestMonitorSize[1] + MAX_WINDOW_HEIGHT_RELATIVE_TO_MONITOR);
		}
	}

	void wayland_window_proc() {
		PRINT_DEBUG("Calling Wayland window procedure");
		wl_display_dispatch_pending(wl_pDisplay);
		wl_display_flush(wl_pDisplay);
	}

	uint32_t wayland_get_actual_window_width() {
		return actualWindowSize[0];
	}
	
	uint32_t wayland_get_actual_window_height() {
		return actualWindowSize[1];
	}

}

#endif /* RE_OS_LINUX */
