#ifndef __RE_WINDOW_WIN64_H__
#define __RE_WINDOW_WIN64_H__ 1

#include "RE_Window_Internal.hpp"

#ifdef RE_OS_WINDOWS

namespace RE {
	
	extern HINSTANCE win_hInstance;
	extern HWND win_hWindow;

	bool win64_create_window();
	void win64_destroy_window();
	void win64_show_window();
	void win64_update_window_title();
	void win64_update_fullscreen();
	void win64_window_proc();
	uint32_t win64_get_actual_window_width();
	uint32_t win64_get_actual_window_height();

}

#endif /* RE_OS_WINDOWS */

#endif /* __RE_WINDOW_WIN64_H__ */
