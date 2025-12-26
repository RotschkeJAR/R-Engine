#ifndef __RE_WINDOW_X11_H__
#define __RE_WINDOW_X11_H__

#include "RE_Internal Header.hpp"

#ifdef RE_OS_LINUX

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>
#include <X11/Xlocale.h>
#include <X11/keysym.h>
#include <X11/XKBlib.h>
#include <X11/extensions/Xrandr.h>
#include <X11/extensions/Xinerama.h>
#define _NET_WM_STATE_REMOVE 0
#define _NET_WM_STATE_ADD 1
#define _NET_WM_STATE_TOGGLE 2

namespace RE {
	
	extern Display *x11_pDisplay;
	extern Window x11_hWindow;

	bool x11_create_window();
	void x11_destroy_window();
	void x11_show_window();
	void x11_update_fullscreen();
	void x11_update_window_title();
	void x11_window_proc();

}

#endif /* RE_OS_LINUX */

#endif /* __RE_WINDOW_X11_H__ */
