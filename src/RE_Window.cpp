#include "RE_Window.hpp"
#include "RE_Render System.hpp"
#include "RE_Window_Win64.hpp"

namespace RE {

#ifdef RE_OS_LINUX
	enum LinuxWindowType {
		X11,
		Wayland
	};

	LinuxWindowType eLinuxWindowType;
#endif
	
	Vector2u windowSize(600, 400);
	const char* pacWindowTitle = "Untitled game window";
	uint8_t u8WindowFlagBits;

	bool create_window() {
		u8WindowFlagBits = 1 << WINDOW_WAYLAND_SHOULD_RENDER_FRAME_BIT;
		bool bSuccess;
#ifdef RE_OS_WINDOWS
		bSuccess = CATCH_SIGNAL_AND_RETURN(win64_create_window(), bool);
#elif defined RE_OS_LINUX
		switch (eLinuxWindowType) {
			case X11:
				bSuccess = CATCH_SIGNAL_AND_RETURN(x11_create_window(), bool);
				break;
			case Wayland:
				u8WindowFlagBits = 0;
				bSuccess = CATCH_SIGNAL_AND_RETURN(wayland_create_window(), bool);
				break;
		}
#endif
		set_bits<uint8_t>(u8WindowFlagBits, bSuccess, WINDOW_CREATED_BIT);
		return bSuccess;
	}

	void destroy_window() {
#ifdef RE_OS_WINDOWS
		CATCH_SIGNAL(win64_destroy_window());
#elif defined RE_OS_LINUX
		switch (eLinuxWindowType) {
			case X11:
				CATCH_SIGNAL(x11_destroy_window());
				break;
			case Wayland:
				CATCH_SIGNAL(wayland_destroy_window());
				break;
		}
#endif
		u8WindowFlagBits = 0;
	}

	void window_resize_event(const uint32_t u32NewWidth, const uint32_t u32NewHeight) {
		windowSize[0] = u32NewWidth;
		windowSize[1] = u32NewHeight;
		mark_swapchain_dirty();
	}

	void show_window(const bool bShowWindow) {
		if (are_bits_true<uint8_t>(u8WindowFlagBits, WINDOW_VISIBLE_BIT) == bShowWindow)
			return;
		set_bits<uint8_t>(u8WindowFlagBits, bShowWindow, WINDOW_VISIBLE_BIT);
#ifdef RE_OS_WINDOWS
		CATCH_SIGNAL(win64_show_window());
#elif defined RE_OS_LINUX
		switch (eLinuxWindowType) {
			case X11:
				CATCH_SIGNAL(x11_show_window());
				break;
			case Wayland:
				CATCH_SIGNAL(wayland_show_window());
				break;
		}
#endif
	}

	void window_proc() {
		CATCH_SIGNAL(update_input_buffers());
#ifdef RE_OS_WINDOWS
		CATCH_SIGNAL(win64_window_proc());
#elif defined RE_OS_LINUX
		switch (eLinuxWindowType) {
			case X11:
				CATCH_SIGNAL(x11_window_proc());
				break;
			case Wayland:
				CATCH_SIGNAL(wayland_window_proc());
				break;
		}
#endif
	}

	void post_rendering_window_proc() {
#ifdef RE_OS_LINUX
		if (eLinuxWindowType == Wayland)
			CATCH_SIGNAL(wayland_post_rendering_window_proc());
#endif
	}

	[[nodiscard]]
	bool should_window_close() {
		return are_bits_true<uint8_t>(u8WindowFlagBits, WINDOW_CLOSE_FLAG_BIT);
	}

	[[nodiscard]]
	bool should_render() {
		return !are_bits_true<uint8_t>(u8WindowFlagBits, WINDOW_MINIMIZED_BIT) && are_bits_true(u8WindowFlagBits, WINDOW_VISIBLE_BIT, WINDOW_WAYLAND_SHOULD_RENDER_FRAME_BIT);
	}

	bool create_vulkan_surface(VkSurfaceKHR &vk_rhSurface) {
#ifdef RE_OS_WINDOWS
		const VkWin32SurfaceCreateInfoKHR vk_win32SurfaceCreateInfo = {
			.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR,
			.hinstance = win_hInstance,
			.hwnd = win_hWindow
		};
		return CATCH_SIGNAL_AND_RETURN(vkCreateWin32SurfaceKHR(vk_hInstance, &vk_win32SurfaceCreateInfo, nullptr, &vk_rhSurface) == VK_SUCCESS, bool);
#elif defined RE_OS_LINUX
		switch (eLinuxWindowType) {
			case X11:
				const VkXlibSurfaceCreateInfoKHR vk_x11SurfaceCreateInfo = {
					.sType = VK_STRUCTURE_TYPE_XLIB_SURFACE_CREATE_INFO_KHR,
					.dpy = x11_pDisplay,
					.window = x11_hWindow
				};
				return CATCH_SIGNAL_AND_RETURN(vkCreateXlibSurfaceKHR(vk_hInstance, &vk_x11SurfaceCreateInfo, nullptr, &vk_rhSurface) == VK_SUCCESS, bool);
			case Wayland:
				const VkWaylandSurfaceCreateInfoKHR vk_waylandSurfaceCreateInfo = {
					.sType = VK_STRUCTURE_TYPE_WAYLAND_SURFACE_CREATE_INFO_KHR,
					.display = wl_pDisplay,
					.surface = wl_pSurface
				};
				return CATCH_SIGNAL_AND_RETURN(vkCreateWaylandSurfaceKHR(vk_hInstance, &vk_waylandSurfaceCreateInfo, nullptr, &vk_rhSurface) == VK_SUCCESS, bool);
		}
#endif
	}

	[[nodiscard]]
	const char* get_vulkan_required_surface_extension_name() {
#ifdef RE_OS_WINDOWS
		return VK_KHR_WIN32_SURFACE_EXTENSION_NAME;
#elif defined RE_OS_LINUX
		switch (eLinuxWindowType) {
			case X11:
				return VK_KHR_XLIB_SURFACE_EXTENSION_NAME;
			case Wayland:
				return VK_KHR_WAYLAND_SURFACE_EXTENSION_NAME;
		}
#endif
	}

	void set_window_title(const char *const pacNewTitle) {
		if (std::strcmp(pacWindowTitle, pacNewTitle) == 0)
			return;
		pacWindowTitle = pacNewTitle;
		if (!are_bits_true<uint8_t>(u8WindowFlagBits, WINDOW_CREATED_BIT))
			return;
#ifdef RE_OS_WINDOWS
		CATCH_SIGNAL(win64_update_window_title());
#elif defined RE_OS_LINUX
		switch (eLinuxWindowType) {
			case X11:
				CATCH_SIGNAL(x11_update_window_title());
				break;
			case Wayland:
				CATCH_SIGNAL(wayland_update_window_title());
				break;
		}
#endif
	}

}
