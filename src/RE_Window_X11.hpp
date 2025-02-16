#ifndef __RE_WINDOW_X11_H__
#define __RE_WINDOW_X11_H__

#include "RE_Window.hpp"

namespace RE {
	
#ifdef RE_OS_LINUX
	class Window_X11 : public Window {
		private:
			XWindow xWindow;
			XAtom xaClose;
			XAtom xaUTF8, xaWinName;
			XInputMethod xInputMethod;
			XInputContext xInputContext;
			XSizeHints* xSizes;

		protected:
			void showInternal();
			void updateTitleInternal();

		public:
			XDisplay* const xDisplay;
			
			Window_X11();
			~Window_X11();
			void processLoop();

		friend class Vulkan;
	};
#endif /* RE_OS_LINUX */

}

#endif /* __RE_WINDOW_X11_H__ */
