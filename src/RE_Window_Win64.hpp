#ifndef __RE_WINDOW_WIN64_H__
#define __RE_WINDOW_WIN64_H__

#include "RE_Window.hpp"

namespace RE {
	
#ifdef RE_OS_WINDOWS
	class Window_Win64 : public Window {
		private:
			HWND win_hWindow;
			HCURSOR win_hCursor;

			Vector2i get_adjusted_window_size(Vector<REushort, 2> size);

		protected:
			void internal_window_proc();
			void internal_show_window();
			void internal_update_title();

		public:
			const HKL win_keyboardLayout;
			static HINSTANCE win_hInstance;
			
			Window_Win64();
			~Window_Win64();
			HWND get_hwindow();

		friend LRESULT CALLBACK windows_window_proc(HWND win_hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	};

	void set_hinstance(HINSTANCE win_hInstance);
#endif /* RE_OS_WINDOWS */

}

#endif /* __RE_WINDOW_WIN64_H__ */
