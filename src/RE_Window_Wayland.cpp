#include "RE_Window_Wayland.hpp"

namespace RE {

#ifdef RE_OS_LINUX
	void wayland_registry_handler(void *pData, wl_registry *wl_pRegistry, uint32_t u32Id, const char *pcInterface, uint32_t u32Version) {
		if (std::strcmp(pcInterface, "wl_compositor") == 0)
			wl_pCompositor = static_cast<wl_compositor*>(wl_registry_bind(wl_pRegistry, id, &wl_compositor_interface, 1));
	}

	void wayland_registry_remove(void *pData, wl_registry *wl_pRegistry, uint32_t u32Id) {}
	
	Window_Wayland::Window_Wayland() : wl_pRegistry(nullptr), wl_pCompositor(nullptr), wl_pWindow(nullptr), wl_pDisplay(wl_display_connect(nullptr)) {
		if (!wl_pDisplay) {
			RE_ERROR("Failed connecting to Wayland server");
			return;
		}
		wl_pRegistry = CATCH_SIGNAL_AND_RETURN(wl_display_get_registry(wl_pDisplay), wl_registry*);
		const wl_registry_listener wl_registryListeners = {wayland_registry_handler, wayland_registry_remove};
		CATCH_SIGNAL(wl_registry_add_listener(wl_pRegistry, wl_registryListeners, nullptr));
		CATCH_SIGNAL(wl_display_roundtrip(wl_pDisplay));
		if (!wl_pCompositor) {
			RE_ERROR("Failed to get Wayland compositor");
			return;
		}
		wl_pWindow = CATCH_SIGNAL_AND_RETURN(wl_compositor_create_surface(wl_pCompositor), wl_surface*);
		if (!wl_pWindow) {
			RE_ERROR("Wayland failed to create window");
			return;
		}
		bValid = true;
	}

	Window_Wayland::~Window_Wayland() {
		if (wl_pDisplay) {
			if (wl_pRegistry) {
				if (wl_pCompositor) {
					if (wl_pWindow)
						CATCH_SIGNAL(wl_surface_destroy(wl_pWindow));
					CATCH_SIGNAL(wl_compositor_destroy(wl_pCompositor));
				}
				CATCH_SIGNAL(wl_registry_destroy(wl_pRegistry));
			}
			CATCH_SIGNAL(wl_display_disconnect(wl_pDisplay));
		}
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
#endif /* RE_OS_LINUX */

}