#include "RE_Window.hpp"

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

	bool Window_Wayland::create_vulkan_surface(VkSurfaceKHR &vk_rhSurface) const {
		VkWaylandSurfaceCreateInfoKHR vk_waylandSurfaceCreateInfo = {};
		vk_waylandSurfaceCreateInfo.sType = VK_STRUCTURE_TYPE_WAYLAND_SURFACE_CREATE_INFO_KHR;
		vk_waylandSurfaceCreateInfo.display = wl_pDisplay;
		vk_waylandSurfaceCreateInfo.surface = wl_pSurface;
		return CHECK_VK_RESULT(vkCreateWaylandSurfaceKHR(vk_hInstance, &vk_waylandSurfaceCreateInfo, nullptr, &vk_rhSurface));
	}

	const char* Window_Wayland::get_vulkan_required_surface_extension_name() const {
		return VK_KHR_WAYLAND_SURFACE_EXTENSION_NAME;
	}
#endif /* RE_OS_LINUX */

}