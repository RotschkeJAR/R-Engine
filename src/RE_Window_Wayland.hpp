#ifndef __RE_WINDOW_WAYLAND_H__
#define __RE_WINDOW_WAYLAND_H__ 1

#include "RE_Window_Internal.hpp"

#ifdef RE_OS_LINUX

namespace RE {
	
	extern wl_display *wl_pDisplay;
	extern wl_surface *wl_pSurface;

	bool wayland_create_window();
	void wayland_destroy_window();
	void wayland_show_window();
	void wayland_update_window_title();
	void wayland_update_fullscreen();
	void wayland_window_proc();
	uint32_t wayland_get_actual_window_width();
	uint32_t wayland_get_actual_window_height();

}

#endif /* RE_OS_LINUX */

#endif /* __RE_WINDOW_WAYLAND_H__ */
