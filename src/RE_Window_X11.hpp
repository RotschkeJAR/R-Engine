#ifndef __RE_WINDOW_X11_H__
#define __RE_WINDOW_X11_H__ 1

#include "RE_Window_Internal.hpp"

#ifdef RE_OS_LINUX

namespace RE {
	
	extern Display *x11_pDisplay;
	extern Window x11_hWindow;

	bool x11_create_window();
	void x11_destroy_window();
	void x11_show_window();
	void x11_update_window_title();
	void x11_update_fullscreen();
	void x11_window_proc();
	uint32_t x11_get_actual_window_width();
	uint32_t x11_get_actual_window_height();

}

#endif /* RE_OS_LINUX */

#endif /* __RE_WINDOW_X11_H__ */
