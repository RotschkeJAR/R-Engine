#ifndef __RE_EXT_H__
#define __RE_EXT_H__

#include "RE.hpp"
#include "RE_Console.hpp"

#include <GL/gl.h>
#include <algorithm>

#ifdef RE_OS_WINDOWS
# define UNICODE
# define WIN32_LEAN_AND_MEAN /* speeds compilation up */
# include <windows.h>
#elif defined RE_OS_LINUX
# include <X11/Xlib.h>
# include <X11/Xatom.h>
# include <X11/Xlocale.h>
# include <X11/keysym.h>
# include <GL/glx.h>
 typedef Display XDisplay;
 typedef Window XWindow;
 typedef Atom XAtom;
 typedef XIM XInputMethod;
 typedef XIC XInputContext;
 typedef KeyCode XKeyCode;
 typedef KeySym XKeySym;
 typedef Status XStatus;
# define XKeyPress KeyPress
# define XKeyRelease KeyRelease
# define XButtonPress ButtonPress
# define XButtonRelease ButtonRelease
# define XMotionNotify MotionNotify
# define XClientMessage ClientMessage
# define XNone None
# define XNoSymbol NoSymbol
#endif /* RE_OS_WINDOWS, RE_OS_LINUX */

namespace RE {
	


}

#endif /* __RE_EXT_H__ */