#ifndef __RE_WINDOW_X11_H__
#define __RE_WINDOW_X11_H__

#include "RE_Window.hpp"

namespace RE {
	
#ifdef RE_OS_LINUX
	class Window_X11 : public Window {
		private:
			Display* const xDisplay;

		protected:
			void showInternal(bool showWindow);

		public:
			Window_X11();
			~Window_X11();
			void processLoop();
			void fullscreen();
			void windowedFullscreen();
			void window();
	};
#endif /* RE_OS_LINUX */

}

#endif /* __RE_WINDOW_X11_H__ */
