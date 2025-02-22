#ifndef __RE_EXT_H__
#define __RE_EXT_H__

#include <algorithm>
#include <fstream>
#include <optional>
#include <vector>
#include <stack>

#include "RE.hpp"
#undef ERROR
#undef WARNING
#undef NOTE
#include "RE_Console.hpp"

#ifdef RE_OS_WINDOWS
# define UNICODE
# ifdef _MSC_VER
#  define NOMINMAX
# endif
# define WIN32_LEAN_AND_MEAN /* speeds compilation up */
# include <windows.h>
# include <windowsx.h>
#elif defined RE_OS_LINUX
# include <X11/Xlib.h>
# include <X11/Xutil.h>
# include <X11/Xatom.h>
# include <X11/Xlocale.h>
# include <X11/keysym.h>
# include <X11/XKBlib.h>
# include <dlfcn.h>
typedef Display XDisplay;
typedef Window XWindow;
typedef Atom XAtom;
typedef XIM XInputMethod;
typedef XIC XInputContext;
typedef KeyCode XKeyCode;
typedef KeySym XKeySym;
typedef Status XStatus;
typedef Colormap XColormap;
# define XKeyPress KeyPress
# define XKeyRelease KeyRelease
# define XButtonPress ButtonPress
# define XButtonRelease ButtonRelease
# define XMotionNotify MotionNotify
# define XClientMessage ClientMessage
# define XResizeRequest ResizeRequest
# define XNone None
# define XNoSymbol NoSymbol
#endif /* RE_OS_WINDOWS, RE_OS_LINUX */

namespace RE {
	


}

#endif /* __RE_EXT_H__ */
