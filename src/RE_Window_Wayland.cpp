#include "RE_Window_Wayland.hpp"

namespace RE {
	
	Window_Wayland::Window_Wayland() : wl_pRegistry(nullptr), wl_pCompositor(nullptr), wl_pWindow(nullptr), wl_pDisplay(wl_display_connect(nullptr)) {
		if (!wl_pDisplay) {
			RE_FATAL_ERROR("Failed connecting to Wayland server");
			return;
		}
		//bValid = true;
	}

	Window_Wayland::~Window_Wayland() {

	}

	void Window_Wayland::internal_window_proc() {

	}
	
	void Window_Wayland::internal_show_window() {

	}
	
	void Window_Wayland::internal_update_title() {

	}

	wl_surface* Window_Wayland::get_wl_surface() {
		return wl_pWindow;
	}

}