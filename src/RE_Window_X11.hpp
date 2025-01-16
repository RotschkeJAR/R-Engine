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
			GLXContext glxContext;
			XInputMethod xInputMethod;
			XInputContext xInputContext;

		protected:
			void showInternal();
			void updateTitleInternal();

		public:
			XDisplay* const xDisplay;
			
			Window_X11();
			~Window_X11();
			void processLoop();
	};
#endif /* RE_OS_LINUX */

}

#endif /* __RE_WINDOW_X11_H__ */
