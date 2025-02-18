#ifndef __RE_WINDOW_X11_H__
#define __RE_WINDOW_X11_H__

#include "RE_Window.hpp"

namespace RE {
	
#ifdef RE_OS_LINUX
	class Window_X11 : public Window {
		private:
			XWindow x11_hWindow;
			XAtom x11_hClose;
			XAtom x11_hUTF8, x11_hWindowName;
			XInputMethod x11_hInputMethod;
			XInputContext x11_hInputContext;
			XSizeHints* x11_pSizes;

		protected:
			void showInternal();
			void updateTitleInternal();

		public:
			XDisplay* const x11_pDisplay;
			
			Window_X11();
			~Window_X11();
			void processLoop();

		friend class Vulkan;
	};
#endif /* RE_OS_LINUX */

}

#endif /* __RE_WINDOW_X11_H__ */
