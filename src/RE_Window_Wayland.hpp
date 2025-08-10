#ifndef __RE_WINDOW_WAYLAND_H__
#define __RE_WINDOW_WAYLAND_H__

#include "RE_Internal Header.hpp"

namespace RE {
	
#ifdef RE_OS_LINUX
	extern wl_display *wl_pDisplay;
	extern wl_surface *wl_pSurface;

	bool wayland_create_window();
	void wayland_destroy_window();
	void wayland_show_window();
	void wayland_update_window_title();
	void wayland_window_proc();
	void wayland_post_rendering_window_proc();
#endif

}

#endif /* __RE_WINDOW_WAYLAND_H__ */
