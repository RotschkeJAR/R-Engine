#ifndef __RE_WINDOW_H__
#define __RE_WINDOW_H__

#include "RE_Internal.hpp"
#include "RE_Input.hpp"
#include "RE_GPU.hpp"

namespace RE {

#ifdef RE_OS_LINUX
#	define WINDOW_WAYLAND_BORDER_TOTAL_SIZE     5
#	define WINDOW_WAYLAND_SHADOW_SIZE           4
#	define WINDOW_WAYLAND_EDGE_SIZE             (WINDOW_WAYLAND_BORDER_TOTAL_SIZE - WINDOW_WAYLAND_SHADOW_SIZE)
#	define WINDOW_WAYLAND_BAR_SIZE              30
#	define WINDOW_WAYLAND_BUTTON_WIDTH          50
#	define WINDOW_WAYLAND_EXTRA_WIDTH           (WINDOW_WAYLAND_BORDER_TOTAL_SIZE * 2)
#	define WINDOW_WAYLAND_EXTRA_HEIGHT          (WINDOW_WAYLAND_BORDER_TOTAL_SIZE * 2 + WINDOW_WAYLAND_BAR_SIZE)
#	define WINDOW_WAYLAND_X_OFFSET              WINDOW_WAYLAND_BORDER_TOTAL_SIZE
#	define WINDOW_WAYLAND_Y_OFFSET              (WINDOW_WAYLAND_BORDER_TOTAL_SIZE + WINDOW_WAYLAND_BAR_SIZE)

#	define WINDOW_BUTTON_TEXTURE_COUNT   3
#	define WINDOW_BUTTON_TEXTURE_SIZE    16

	enum LinuxWindowType {
		LINUX_WINDOW_TYPE_X11 = 0,
		LINUX_WINDOW_TYPE_WAYLAND = 1
	};

	extern LinuxWindowType eLinuxWindowType;
#endif

	extern const char* pacWindowTitle;

	bool create_window();
	void destroy_window();
	void show_window(bool bShowWindow);
	void update_fullscreen();
	void window_proc();
	uint32_t get_window_width();
	uint32_t get_window_height();
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
