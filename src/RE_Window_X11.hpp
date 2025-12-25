#ifndef __RE_WINDOW_X11_H__
#define __RE_WINDOW_X11_H__

#include "RE_Internal Header.hpp"

#ifdef RE_OS_LINUX

namespace RE {
	
	extern XDisplay *x11_pDisplay;
	extern XWindow x11_hWindow;

	bool x11_create_window();
	void x11_destroy_window();
	void x11_show_window();
	void x11_update_fullscreen();
	void x11_update_window_title();
	void x11_window_proc();

}

#endif /* RE_OS_LINUX */

#endif /* __RE_WINDOW_X11_H__ */
