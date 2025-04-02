#ifndef __RE_WINDOW_WAYLAND_H__
#define __RE_WINDOW_WAYLAND_H__

#include "RE_Window.hpp"

namespace RE {

#ifdef RE_OS_LINUX
	class Window_Wayland : public Window {
		private:
			wl_registry *wl_pRegistry;
			wl_compositor *wl_pCompositor;
			wl_surface *wl_pSurface;
			
		protected:
			void internal_window_proc();
			void internal_show_window();
			void internal_update_title();

		public:
			wl_display *const wl_pDisplay;

			Window_Wayland();
			~Window_Wayland();
			wl_surface* get_wl_surface();
	};
#endif /* RE_OS_LINUX */

}

#endif /* __RE_WINDOW_WAYLAND_H__ */
