#include "RE_Window.hpp"

namespace RE {

#ifdef RE_OS_LINUX
	static void window_base_ping_handler(void *pData, xdg_wm_base *wl_pBase, uint32_t u32Serial) {
		CATCH_SIGNAL(xdg_wm_base_pong(wl_pBase, u32Serial));
	}

	void registry_handle_global(void *pData, wl_registry *wl_pRegistry, uint32_t u32Name, const char *pcInterface, uint32_t u32Version) {
		Window_Wayland *pWindowWayland = static_cast<Window_Wayland*>(pData);
		if (are_string_contents_equal(pcInterface, wl_compositor_interface.name))
			CATCH_SIGNAL(pWindowWayland->wl_pCompositor = static_cast<wl_compositor*>(wl_registry_bind(wl_pRegistry, u32Name, &wl_compositor_interface, u32Version)));
		else if (are_string_contents_equal(pcInterface, xdg_wm_base_interface.name))
			CATCH_SIGNAL(pWindowWayland->xdg_pWindowBase = static_cast<xdg_wm_base*>(wl_registry_bind(wl_pRegistry, u32Name, &xdg_wm_base_interface, u32Version)));
	}

	static void registry_handle_global_remove(void *pData, wl_registry *wl_pRegistry, uint32_t u32Name) {
	}

	static void wayland_configure_window_event(void *pData, xdg_surface *xdg_pSurface, uint32_t u32Serial) {
		CATCH_SIGNAL(xdg_surface_ack_configure(xdg_pSurface, u32Serial));
	}

	void wayland_frame_ready(void *pData, wl_callback *wl_pCallback, uint32_t u32Serial) {
		CATCH_SIGNAL(wl_callback_destroy(wl_pCallback));
		Window_Wayland *pWindowWayland = static_cast<Window_Wayland*>(pData);
		pWindowWayland->bShouldRenderFrame = true;
		PRINT_LN("called frame ready");
		CATCH_SIGNAL(pWindowWayland->register_next_ready_frame_callback());
	}

	Window_Wayland::Window_Wayland() : wl_pRegistry(nullptr), wl_pCompositor(nullptr), wl_pSurface(nullptr), xdg_pWindowBase(nullptr), xdg_pSurface(nullptr), xdg_pToplevel(nullptr), bShouldRenderFrame(false), wl_pDisplay(wl_display_connect(nullptr)) {
		if (!wl_pDisplay) {
			RE_FATAL_ERROR("Failed connecting to Wayland server");
			return;
		}
		wl_pRegistry = CATCH_SIGNAL_AND_RETURN(wl_display_get_registry(wl_pDisplay), wl_registry*);
		const wl_registry_listener wl_registryListeners = {
			.global = registry_handle_global,
			.global_remove = registry_handle_global_remove
		};
		CATCH_SIGNAL(wl_registry_add_listener(wl_pRegistry, &wl_registryListeners, this));
		CATCH_SIGNAL(wl_display_roundtrip(wl_pDisplay));
		if (!wl_pCompositor) {
			RE_FATAL_ERROR("Failed to get Wayland compositor");
			return;
		} else if (!xdg_pWindowBase) {
			RE_FATAL_ERROR("Failed to get Wayland window base");
			return;
		}
		const xdg_wm_base_listener xdg_listener = {
			.ping = window_base_ping_handler
		};
		CATCH_SIGNAL(xdg_wm_base_add_listener(xdg_pWindowBase, &xdg_listener, this));
		CATCH_SIGNAL(wl_pSurface = wl_compositor_create_surface(wl_pCompositor));
		if (!wl_pSurface) {
			RE_FATAL_ERROR("Failed to create a Wayland surface");
			return;
		}
		CATCH_SIGNAL(xdg_pSurface = xdg_wm_base_get_xdg_surface(xdg_pWindowBase, wl_pSurface));
		if (!xdg_pSurface) {
			RE_FATAL_ERROR("Failed to create an XDG window for Wayland");
			return;
		}
		CATCH_SIGNAL(xdg_pToplevel = xdg_surface_get_toplevel(xdg_pSurface));
		if (!xdg_pToplevel) {
			RE_FATAL_ERROR("Failed to get toplevel of the XDG window for Wayland");
			return;
		}
		const xdg_surface_listener xdg_surfaceListener = {
			.configure = wayland_configure_window_event
		};
		CATCH_SIGNAL(xdg_surface_add_listener(xdg_pSurface, &xdg_surfaceListener, this));
		CATCH_SIGNAL(xdg_toplevel_set_title(xdg_pToplevel, pcTitle));
		CATCH_SIGNAL(xdg_surface_set_window_geometry(xdg_pSurface, 0, 0, windowSize[0], windowSize[1]));
		CATCH_SIGNAL(xdg_toplevel_set_min_size(xdg_pToplevel, MIN_WINDOW_WIDTH, MIN_WINDOW_HEIGHT));
		CATCH_SIGNAL(register_next_ready_frame_callback());
		bValid = true;
	}

	Window_Wayland::~Window_Wayland() {
		if (!wl_pDisplay)
			return;
		if (xdg_pToplevel)
			CATCH_SIGNAL(xdg_toplevel_destroy(xdg_pToplevel));
		if (xdg_pSurface)
			CATCH_SIGNAL(xdg_surface_destroy(xdg_pSurface));
		if (xdg_pWindowBase)
			CATCH_SIGNAL(xdg_wm_base_destroy(xdg_pWindowBase));
		if (wl_pSurface)
			CATCH_SIGNAL(wl_surface_destroy(wl_pSurface));
		if (wl_pCompositor)
			CATCH_SIGNAL(wl_compositor_destroy(wl_pCompositor));
		if (wl_pRegistry)
			CATCH_SIGNAL(wl_registry_destroy(wl_pRegistry));
		CATCH_SIGNAL(wl_display_disconnect(wl_pDisplay));
	}

	void Window_Wayland::register_next_ready_frame_callback() {
		wl_callback *wl_pFrameReadyCallback = CATCH_SIGNAL_AND_RETURN(wl_surface_frame(wl_pSurface), wl_callback*);
		const wl_callback_listener wl_frameReadyCallback = {
			.done = wayland_frame_ready
		};
		CATCH_SIGNAL(wl_callback_add_listener(wl_pFrameReadyCallback, &wl_frameReadyCallback, this));
		CATCH_SIGNAL(commit_surface());
	}

	void Window_Wayland::commit_surface() {
		CATCH_SIGNAL(wl_surface_commit(wl_pSurface));
		CATCH_SIGNAL(wl_display_flush(wl_pDisplay));
	}

	void Window_Wayland::internal_window_proc() {
		//while (CATCH_SIGNAL_AND_RETURN(wl_display_dispatch(wl_pDisplay), int32_t) > 0) {
		//}
	}
	
	void Window_Wayland::internal_show_window() {
		CATCH_SIGNAL(commit_surface());
	}
	
	void Window_Wayland::internal_update_title() {
		CATCH_SIGNAL(xdg_toplevel_set_title(xdg_pToplevel, pcTitle));
		CATCH_SIGNAL(commit_surface());
	}

	bool Window_Wayland::create_vulkan_surface(VkSurfaceKHR &vk_rhSurface) const {
		const VkWaylandSurfaceCreateInfoKHR vk_waylandSurfaceCreateInfo = {
			.sType = VK_STRUCTURE_TYPE_WAYLAND_SURFACE_CREATE_INFO_KHR,
			.display = wl_pDisplay,
			.surface = wl_pSurface
		};
		return vkCreateWaylandSurfaceKHR(vk_hInstance, &vk_waylandSurfaceCreateInfo, nullptr, &vk_rhSurface) == VK_SUCCESS;
	}

	const char* Window_Wayland::get_vulkan_required_surface_extension_name() const {
		return VK_KHR_WAYLAND_SURFACE_EXTENSION_NAME;
	}

	void Window_Wayland::post_rendering_window_proc() {
		CATCH_SIGNAL(commit_surface());
	}

	bool Window_Wayland::should_render() {
		if (bShouldRenderFrame) {
			bShouldRenderFrame = false;
			PRINT_LN("allowed rendering");
			return true;
		} else
			return false;
	}

	WindowType Window_Wayland::get_window_type() const {
		return WindowType::Wayland;
	}
#endif /* RE_OS_LINUX */

}