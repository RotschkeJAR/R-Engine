#ifndef __RE_WINDOW_WIN64_H__
#define __RE_WINDOW_WIN64_H__

#include "RE_Internal Header.hpp"

#ifdef RE_OS_WINDOWS

# define UNICODE
# ifdef _MSC_VER
#  define NOMINMAX
# endif
# define WIN32_LEAN_AND_MEAN /* speeds compilation up */
# define WINVER _WIN32_WINNT
# include <windows.h>
# include <shellscalingapi.h>
# include <windowsx.h>

namespace RE {
	
	extern HINSTANCE win_hInstance;
	extern HWND win_hWindow;

	bool win64_create_window();
	void win64_destroy_window();
	void win64_show_window();
	void win64_update_fullscreen();
	void win64_update_window_title();
	void win64_window_proc();

}

#endif /* RE_OS_WINDOWS */

#endif /* __RE_WINDOW_WIN64_H__ */
