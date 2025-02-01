#ifndef __RE_WINDOW_WIN64_H__
#define __RE_WINDOW_WIN64_H__

#include "RE_Window.hpp"

namespace RE {
	
#ifdef RE_OS_WINDOWS
	class Window_Win64 : public Window {
		private:
			HINSTANCE hInstance;
			HWND hWindow;
			MSG msg;
			HCURSOR hCursor;

		protected:
			void showInternal();
			void updateTitleInternal();

		public:
			Window_Win64();
			~Window_Win64();
			void processLoop();

		friend LRESULT CALLBACK windowProcess(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	};
#endif /* RE_OS_WINDOWS */

}

#endif /* __RE_WINDOW_WIN64_H__ */
