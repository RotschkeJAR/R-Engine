#include "RE_Window_Internal.hpp"
#include "RE_Renderer.hpp"
#include "RE_Settings.hpp"

namespace RE {

#ifdef RE_OS_LINUX
	LinuxWindowType eLinuxWindowType = LINUX_WINDOW_TYPE_X11;
#endif
	
	Vector2u windowSize,
		largestMonitorSize;
	const char* pacWindowTitle = "Untitled game window";
	WindowFlags mWindowFlagBits = 0;

	bool create_window() {
		PRINT_DEBUG("Creating window");
		windowSize[0] = 600;
		windowSize[1] = 400;
		largestMonitorSize[0] = 1920;
		largestMonitorSize[1] = 1080;
		bool bSuccess;
	#ifdef RE_OS_WINDOWS
		bSuccess = win64_create_window();
	#elif defined RE_OS_LINUX
		switch (eLinuxWindowType) {
			case LINUX_WINDOW_TYPE_X11:
				bSuccess = x11_create_window();
				break;
			case LINUX_WINDOW_TYPE_WAYLAND:
				bSuccess = wayland_create_window();
				break;
			default:
				RE_ABORT("The compositor is unknown to create a window");
		}
	#endif
		set_bitmasks(mWindowFlagBits, bSuccess, static_cast<WindowFlags>(WINDOW_FLAG_CREATED_BIT));
		return bSuccess;
	}

	void destroy_window() {
		PRINT_DEBUG("Destroying window");
	#ifdef RE_OS_WINDOWS
		win64_destroy_window();
	#elif defined RE_OS_LINUX
		switch (eLinuxWindowType) {
			case LINUX_WINDOW_TYPE_X11:
				x11_destroy_window();
				break;
			case LINUX_WINDOW_TYPE_WAYLAND:
				wayland_destroy_window();
				break;
			default:
				RE_ABORT("The compositor is unknown to destroy the window");
		}
	#endif
		mWindowFlagBits &= ~(WINDOW_FLAG_CREATED_BIT | WINDOW_FLAG_CLOSE_BIT | WINDOW_FLAG_MINIMIZED_BIT | WINDOW_FLAG_MAXIMIZED_BIT | WINDOW_FLAG_VISIBLE_BIT);
	}

	void window_resize_event(uint32_t u32NewWidth, uint32_t u32NewHeight) {
		PRINT_DEBUG("Updating window dimensions after resize");
		windowSize[0] = u32NewWidth;
		windowSize[1] = u32NewHeight;
		mark_swapchain_dirty();
	}

	void show_window(bool bShowWindow) {
		set_bitmasks(mWindowFlagBits, bShowWindow, static_cast<WindowFlags>(WINDOW_FLAG_VISIBLE_BIT));
	#ifdef RE_OS_WINDOWS
		win64_show_window();
	#elif defined RE_OS_LINUX
		switch (eLinuxWindowType) {
			case LINUX_WINDOW_TYPE_X11:
				x11_show_window();
				break;
			case LINUX_WINDOW_TYPE_WAYLAND:
				wayland_show_window();
				break;
			default:
				RE_ABORT("The compositor is unknown to show the window");
		}
	#endif
	}

	void update_fullscreen() {
	#ifdef RE_OS_WINDOWS
		win64_update_fullscreen();
	#elif defined RE_OS_LINUX
		switch (eLinuxWindowType) {
			case LINUX_WINDOW_TYPE_X11:
				x11_update_fullscreen();
				break;
			case LINUX_WINDOW_TYPE_WAYLAND:
				wayland_update_fullscreen();
				break;
			default:
				RE_ABORT("The compositor is unknown to update the fullscreen state of the window");
		}
	#endif
	}

	void window_proc() {
		update_input_buffers();
	#ifdef RE_OS_WINDOWS
		win64_window_proc();
	#elif defined RE_OS_LINUX
		switch (eLinuxWindowType) {
			case LINUX_WINDOW_TYPE_X11:
				x11_window_proc();
				break;
			case LINUX_WINDOW_TYPE_WAYLAND:
				wayland_window_proc();
				break;
			default:
				RE_ABORT("The compositor is unknown to call the window procedure");
		}
	#endif
	}

	uint32_t get_actual_window_width() {
	#ifdef RE_OS_WINDOWS
		return win64_get_actual_window_width();
	#elif defined RE_OS_LINUX
		switch (eLinuxWindowType) {
			case LINUX_WINDOW_TYPE_X11:
				return x11_get_actual_window_width();
			case LINUX_WINDOW_TYPE_WAYLAND:
				return wayland_get_actual_window_width();
			default:
				RE_ABORT("The compositor is unknown to get the actual window width");
		}
	#endif
		return 0;
	}
	
	uint32_t get_actual_window_height() {
	#ifdef RE_OS_WINDOWS
		return win64_get_actual_window_height();
	#elif defined RE_OS_LINUX
		switch (eLinuxWindowType) {
			case LINUX_WINDOW_TYPE_X11:
				return x11_get_actual_window_height();
			case LINUX_WINDOW_TYPE_WAYLAND:
				return wayland_get_actual_window_height();
			default:
				RE_ABORT("The compositor is unknown to get the actual window height");
		}
	#endif
		return 0;
	}

	bool should_window_close() {
		return (mWindowFlagBits & WINDOW_FLAG_CLOSE_BIT);
	}

	bool should_render() {
		return !(mWindowFlagBits & WINDOW_FLAG_MINIMIZED_BIT) && (mWindowFlagBits & WINDOW_FLAG_VISIBLE_BIT);
	}

#ifdef RENDERER_INCLUDE_WINDOW_FRAME
	bool should_render_window_frame() {
		return eLinuxWindowType == LINUX_WINDOW_TYPE_WAYLAND;
	}

	bool should_render_window_frame_bar() {
		return !static_cast<bool>(mSettingsFlags & SETTINGS_FLAG_FULLSCREEN_BIT);
	}

	bool should_render_window_frame_edges() {
		return should_render_window_frame_bar() && !(mWindowFlagBits & WINDOW_FLAG_MAXIMIZED_BIT);
	}
#endif

	bool create_vulkan_surface() {
	#ifdef RE_OS_WINDOWS
		PRINT_DEBUG("Creating Vulkan surface linked to Windows");
		const VkWin32SurfaceCreateInfoKHR vk_win32SurfaceCreateInfo = {
			.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR,
			.hinstance = win_hInstance,
			.hwnd = win_hWindow
		};
		return vkCreateWin32SurfaceKHR(vk_hInstance, &vk_win32SurfaceCreateInfo, nullptr, &vk_hSurface) == VK_SUCCESS;
	#elif defined RE_OS_LINUX
		switch (eLinuxWindowType) {
			case LINUX_WINDOW_TYPE_X11:
				{
					PRINT_DEBUG("Creating Vulkan surface linked to LINUX_WINDOW_TYPE_X11");
					const VkXlibSurfaceCreateInfoKHR vk_x11SurfaceCreateInfo = {
						.sType = VK_STRUCTURE_TYPE_XLIB_SURFACE_CREATE_INFO_KHR,
						.dpy = x11_pDisplay,
						.window = x11_hWindow
					};
					return vkCreateXlibSurfaceKHR(vk_hInstance, &vk_x11SurfaceCreateInfo, nullptr, &vk_hSurface) == VK_SUCCESS;
				}
			case LINUX_WINDOW_TYPE_WAYLAND:
				{
					PRINT_DEBUG("Creating Vulkan surface linked to Wayland");
					const VkWaylandSurfaceCreateInfoKHR vk_waylandSurfaceCreateInfo = {
						.sType = VK_STRUCTURE_TYPE_WAYLAND_SURFACE_CREATE_INFO_KHR,
						.display = wl_pDisplay,
						.surface = wl_pSurface
					};
					return vkCreateWaylandSurfaceKHR(vk_hInstance, &vk_waylandSurfaceCreateInfo, nullptr, &vk_hSurface) == VK_SUCCESS;
				}
			default:
				RE_ABORT("The compositor is unknown to create a Vulkan surface linked to it");
		}
	#endif
		return false;
	}

	const char* get_vulkan_required_surface_extension_name() {
	#ifdef RE_OS_WINDOWS
		return VK_KHR_WIN32_SURFACE_EXTENSION_NAME;
	#elif defined RE_OS_LINUX
		switch (eLinuxWindowType) {
			case LINUX_WINDOW_TYPE_X11:
				return VK_KHR_XLIB_SURFACE_EXTENSION_NAME;
			case LINUX_WINDOW_TYPE_WAYLAND:
				return VK_KHR_WAYLAND_SURFACE_EXTENSION_NAME;
			default:
				RE_ABORT("The compositor is unknown to get the extension name for the Vulkan surface linking to the window");
		}
	#endif
		return nullptr;
	}

	void set_window_title(const char *pacNewTitle) {
		if (are_string_contents_equal(pacWindowTitle, pacNewTitle))
			return;
		PRINT_DEBUG("Updating window title");
		pacWindowTitle = pacNewTitle;
		if (!(mWindowFlagBits & WINDOW_FLAG_CREATED_BIT))
			return;
	#ifdef RE_OS_WINDOWS
		win64_update_window_title();
	#elif defined RE_OS_LINUX
		switch (eLinuxWindowType) {
			case LINUX_WINDOW_TYPE_X11:
				x11_update_window_title();
				break;
			case LINUX_WINDOW_TYPE_WAYLAND:
				wayland_update_window_title();
				break;
			default:
				RE_ABORT("The compositor is unknown to update the title of the window");
		}
	#endif
	}

	const char* get_window_title() {
		return pacWindowTitle;
	}

}
