#ifndef __RE_WINDOW_H__
#define __RE_WINDOW_H__

#include "RE_Internal Header.hpp"
#include "RE_Input.hpp"
#include "RE_GPU.hpp"

namespace RE {

#define MIN_WINDOW_WIDTH 200
#define MIN_WINDOW_HEIGHT 200
#define MAX_WINDOW_WIDTH_RELATIVE_TO_MONITOR -100
#define MAX_WINDOW_HEIGHT_RELATIVE_TO_MONITOR -100

	typedef int WindowFlags;
	enum WindowFlagBit : WindowFlags {
		WINDOW_FLAG_CREATED_BIT = 0x1,
		WINDOW_FLAG_VISIBLE_BIT = 0x2,
		WINDOW_FLAG_MINIMIZED_BIT = 0x4,
		WINDOW_FLAG_MAXIMIZED_BIT = 0x8,
		WINDOW_FLAG_FULLSCREEN_BIT = 0x10,
		WINDOW_FLAG_CLOSE_BIT = 0x20
	};

#ifdef RE_OS_LINUX
#define WINDOW_WAYLAND_BORDER_TOTAL_SIZE 5
#define WINDOW_WAYLAND_SHADOW_SIZE 4
#define WINDOW_WAYLAND_EDGE_SIZE (WINDOW_WAYLAND_BORDER_TOTAL_SIZE - WINDOW_WAYLAND_SHADOW_SIZE)
#define WINDOW_WAYLAND_BAR_SIZE 30
#define WINDOW_WAYLAND_BUTTON_WIDTH 50
#define WINDOW_WAYLAND_EXTRA_WIDTH (WINDOW_WAYLAND_BORDER_TOTAL_SIZE * 2)
#define WINDOW_WAYLAND_EXTRA_HEIGHT (WINDOW_WAYLAND_BORDER_TOTAL_SIZE * 2 + WINDOW_WAYLAND_BAR_SIZE)
#define WINDOW_WAYLAND_X_OFFSET WINDOW_WAYLAND_BORDER_TOTAL_SIZE
#define WINDOW_WAYLAND_Y_OFFSET (WINDOW_WAYLAND_BORDER_TOTAL_SIZE + WINDOW_WAYLAND_BAR_SIZE)

	enum LinuxWindowType {
		LINUX_WINDOW_TYPE_X11 = 0,
		LINUX_WINDOW_TYPE_WAYLAND = 1
	};

	extern LinuxWindowType eLinuxWindowType;
#endif

	enum WindowArea {
		WINDOW_AREA_TOP_LEFT = 0,
		WINDOW_AREA_TOP_RIGHT = 1,
		WINDOW_AREA_BOTTOM_LEFT = 2,
		WINDOW_AREA_BOTTOM_RIGHT = 3,
		WINDOW_AREA_LEFT = 4,
		WINDOW_AREA_RIGHT = 5,
		WINDOW_AREA_TOP = 6,
		WINDOW_AREA_BOTTOM = 7,
		WINDOW_AREA_BAR = 8,
		WINDOW_AREA_BUTTON_CLOSE = 9,
		WINDOW_AREA_BUTTON_MAXIMIZE = 10,
		WINDOW_AREA_BUTTON_MINIMIZE = 11,
		WINDOW_AREA_CONTENT = 12
	};

	extern Vector2u windowSize,
		largestMonitorSize;
	extern const char* pacWindowTitle;
	extern Input eInputFullscreenToggle;
	extern WindowFlags mWindowFlagBits;

	bool create_window();
	void destroy_window();
	void window_resize_event(uint32_t u32NewWidth, uint32_t u32NewHeight);
	void show_window(bool bShowWindow);
	void window_proc();
	uint32_t get_window_actual_width();
	uint32_t get_window_actual_height();
	bool should_window_close();
	bool should_render();
#ifdef RE_OS_LINUX
	bool should_render_window_frame_bar();
	bool should_render_window_frame_edges();
#endif
	bool create_vulkan_surface();
	const char* get_vulkan_required_surface_extension_name();

}

#endif /* __RE_WINDOW_H__ */
