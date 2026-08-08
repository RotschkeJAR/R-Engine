#ifndef __RE_WINDOW_H__
#define __RE_WINDOW_H__ 1

#include "RE_Internal.hpp"
#include "RE_Input.hpp"
#include "RE_GPU.hpp"

#ifdef RE_OS_WINDOWS
#	define UNICODE
#	ifdef _MSC_VER
#		define NOMINMAX
#	endif
#	define WIN32_LEAN_AND_MEAN /* speeds compilation up */
#	define WINVER _WIN32_WINNT
#	include <windows.h>
#	include <shellscalingapi.h>
#	include <windowsx.h>
#elif defined RE_OS_LINUX
#	include <X11/Xlib.h>
#	include <X11/Xutil.h>
#	include <X11/Xatom.h>
#	include <X11/Xlocale.h>
#	include <X11/keysym.h>
#	include <X11/XKBlib.h>
#	include <X11/extensions/Xrandr.h>
#	include <X11/extensions/Xinerama.h>
#	define _NET_WM_STATE_REMOVE  0
#	define _NET_WM_STATE_ADD     1
#	define _NET_WM_STATE_TOGGLE  2
#	include <wayland-client.h>
#	include <xdg-shell-client-protocol.h>
#	include <xkbcommon/xkbcommon.h>

#	define RENDERER_INCLUDE_WINDOW_FRAME 1
#	define RENDERER_INCLUDE_EMPTY_PRESENT 1
#endif

#include "RE_Renderer.hpp"

namespace RE {

	enum WindowArea {
		WINDOW_AREA_NONE = 0,
		WINDOW_AREA_TOP_LEFT = 1,
		WINDOW_AREA_TOP_RIGHT = 2,
		WINDOW_AREA_BOTTOM_LEFT = 3,
		WINDOW_AREA_BOTTOM_RIGHT = 4,
		WINDOW_AREA_LEFT = 5,
		WINDOW_AREA_RIGHT = 6,
		WINDOW_AREA_TOP = 7,
		WINDOW_AREA_BOTTOM = 8,
		WINDOW_AREA_BAR = 9,
		WINDOW_AREA_BUTTON_CLOSE = 10,
		WINDOW_AREA_BUTTON_MAXIMIZE = 11,
		WINDOW_AREA_BUTTON_MINIMIZE = 12,
		WINDOW_AREA_CONTENT = 13
	};

#define WINDOW_SHADOW_SIZE            4
#define WINDOW_EDGE_SIZE              1
#define WINDOW_BORDER_TOTAL_SIZE      (WINDOW_SHADOW_SIZE + WINDOW_EDGE_SIZE)
#define WINDOW_BAR_SIZE               30
#define WINDOW_BUTTON_WIDTH           50
#define WINDOW_EXTRA_WIDTH            (WINDOW_BORDER_TOTAL_SIZE * 2)
#define WINDOW_EXTRA_HEIGHT           (WINDOW_BORDER_TOTAL_SIZE * 2 + WINDOW_BAR_SIZE)
#define WINDOW_X_OFFSET               WINDOW_BORDER_TOTAL_SIZE
#define WINDOW_Y_OFFSET               (WINDOW_BORDER_TOTAL_SIZE + WINDOW_BAR_SIZE)
#define WINDOW_CHAR_GAP_SIZE          2
#define WINDOW_BUTTON_TEXTURE_COUNT   3
#define WINDOW_BUTTON_TEXTURE_SIZE    16

#ifdef RE_OS_LINUX
	enum LinuxWindowType {
		LINUX_WINDOW_TYPE_X11 = 0,
		LINUX_WINDOW_TYPE_WAYLAND = 1
	};

	extern LinuxWindowType eLinuxWindowType;
#endif

	extern const char* pacWindowTitle;
	extern Vector2u windowSize,
		largestMonitorSize;

	bool create_window();
	void destroy_window();
	void show_window(bool bShowWindow);
	void update_fullscreen();
	void window_proc();
	uint32_t get_actual_window_width();
	uint32_t get_actual_window_height();
	bool should_window_close();
	bool should_render();
#ifdef RENDERER_INCLUDE_WINDOW_FRAME
	bool should_render_window_frame();
	bool should_render_window_frame_bar();
	bool should_render_window_frame_edges();
#else
	consteval bool should_render_window_frame() {
		return false;
	}
	consteval bool should_render_window_frame_bar() {
		return false;
	}
	consteval bool should_render_window_frame_edges() {
		return false;
	}
#endif
	bool create_vulkan_surface();
	const char* get_vulkan_required_surface_extension_name();

}

#endif /* __RE_WINDOW_H__ */
