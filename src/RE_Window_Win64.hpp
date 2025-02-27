#ifndef __RE_WINDOW_WIN64_H__
#define __RE_WINDOW_WIN64_H__

#include "RE_Window.hpp"

namespace RE {
	
#ifdef RE_OS_WINDOWS
	class Window_Win64 : public Window {
		private:
			HWND win_hWindow;
			MSG win_msg;
			HCURSOR win_hCursor;

			Vector2i getAdjustedWindowSize(Vector<REushort, 2> size);

		protected:
			void showInternal();
			void updateTitleInternal();

		public:
			static HINSTANCE win_hInstance;
			
			Window_Win64();
			~Window_Win64();
			void processLoop();

		friend class Vulkan;
		friend LRESULT CALLBACK windowProcess(HWND win_hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	};

	void setHInstance(HINSTANCE win_hInstance);
#endif /* RE_OS_WINDOWS */

}

#endif /* __RE_WINDOW_WIN64_H__ */
