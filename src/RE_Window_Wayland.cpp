#include "RE_Window_Wayland.hpp"

namespace RE {

#ifdef RE_OS_LINUX
	Window_Wayland::Window_Wayland() : wl_pRegistry(nullptr), wl_pCompositor(nullptr), wl_pSurface(nullptr), wl_pDisplay(wl_display_connect(nullptr)) {
		if (!wl_pDisplay) {
			RE_ERROR("Failed connecting to Wayland server");
			return;
		}
		//bValid = true;
	}

	Window_Wayland::~Window_Wayland() {
		if (wl_pDisplay)
			CATCH_SIGNAL(wl_display_disconnect(wl_pDisplay));
	}

	void Window_Wayland::internal_window_proc() {
		int32_t i32PendingEvents;
		do {
			i32PendingEvents = wl_display_dispatch_pending(wl_pDisplay);
			if (i32PendingEvents == 0)
				return;
			else if (i32PendingEvents == -1) {
				RE_FATAL_ERROR("Failed to get next event of the Wayland window");
				return;
			}
			wl_display_dispatch(wl_pDisplay);
		} while (i32PendingEvents > 0);
	}
	
	void Window_Wayland::internal_show_window() {

	}
	
	void Window_Wayland::internal_update_title() {

	}

	wl_surface* Window_Wayland::get_wl_surface() {
		return wl_pSurface;
	}
#endif /* RE_OS_LINUX */

}